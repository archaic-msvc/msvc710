// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// This module is unique -- no need for PCH

#include <limits.h>

#pragma warning(disable:4714)
#ifdef _AFXDLL
#include <afx.h>
#include <afxwin.h>
#include <afxdb.h>
#include <afxpriv.h>
#endif


#include <afxisapi.h>
#include <stddef.h>
#include <atlconv.h>
#include <afxres.h>
#include <stdio.h>
#include <malloc.h>
#include "dispimpl.h"

///////////////////////////////////////////////////////////////////////
// Globals

// pointers to single global server and filter objects

static CHttpServer* pServer = NULL;
static CHttpFilter* pFilter = NULL;

// stock server extension strings

static const TCHAR szGet[] = _T("GET");
static const TCHAR szPost[] = _T("POST");
static const TCHAR szDecimalFormat[] = _T("%d");
static const TCHAR szFloatFormat[] = _T("%f");

// stock HTML tags

static const TCHAR szContentType[] = _T("Content-Type: text/html\r\n");
static const TCHAR szEndBody[] = _T("</body></html>");
static const TCHAR szStartTitle[] = _T("<html><head><title>");
static const TCHAR szEndTitle[] = _T("</title></head><body>");
static const TCHAR szDefaultTitle[] = _T("Default MFC Web Server Extension");

// error texts
// these aren't localized as they are part of the HTTP spec

typedef struct _httpstatinfo {
	DWORD   dwCode;
	LPCTSTR pstrString;
} HTTPStatusInfo;

static HTTPStatusInfo statusStrings[] = {
	{ HTTP_STATUS_OK,                _T("OK") },
	{ HTTP_STATUS_CREATED,           _T("Created") },
	{ HTTP_STATUS_ACCEPTED,          _T("Accepted") },
	{ HTTP_STATUS_NO_CONTENT,        _T("No Content") },
	{ HTTP_STATUS_TEMP_REDIRECT,     _T("Moved Temporarily") },
	{ HTTP_STATUS_REDIRECT,          _T("Moved Permanently") },
	{ HTTP_STATUS_NOT_MODIFIED,      _T("Not Modified") },
	{ HTTP_STATUS_BAD_REQUEST,       _T("Bad Request") },
	{ HTTP_STATUS_AUTH_REQUIRED,     _T("Unauthorized") },
	{ HTTP_STATUS_FORBIDDEN,         _T("Forbidden") },
	{ HTTP_STATUS_NOT_FOUND,         _T("Not Found") },
	{ HTTP_STATUS_REQUEST_TOO_LARGE, _T("Request entity was too large") },
	{ HTTP_STATUS_SERVER_ERROR,      _T("Internal Server Error") },
	{ HTTP_STATUS_NOT_IMPLEMENTED,   _T("Not Implemented") },
	{ HTTP_STATUS_BAD_GATEWAY,       _T("Bad Gateway") },
	{ HTTP_STATUS_SERVICE_NA,        _T("Service Unavailable") },
	{ 0, NULL }
};

/////////////////////////////////////////////////////////////////////////////
// Root of all parse maps

const AFXIS_DATADEF AFX_PARSEMAP CHttpServer::parseMap =
{
	&CHttpServer::GetNumMapEntries,
	NULL,
	&CHttpServer::_parseEntries[0],
};

AFX_PARSEMAP_ENTRY CHttpServer::_parseEntries[] =
{
	{ NULL, NULL, NULL }    // nothing here
};

UINT PASCAL CHttpServer::GetNumMapEntries()
{
	return 0;
}

const AFX_PARSEMAP* CHttpServer::GetParseMap() const
{
	return NULL;
}

#ifdef _AFXDLL
const AFX_PARSEMAP* CHttpServer::GetThisParseMap()
{
	return NULL;
}
#endif

AFX_PARSEMAP::~AFX_PARSEMAP()
{
	// walk through parse map to find any dying parsed parameter entries

	UINT iEntry;
	UINT cEntries = (*pfnGetNumMapEntries)();
	AFX_PARSEMAP_ENTRY* pCurrent = (AFX_PARSEMAP_ENTRY*) lpEntries;

	for (iEntry = 0; iEntry < cEntries; iEntry++, pCurrent++)
	{
		if (pCurrent->pfn == NULL)
		{
			delete (AFX_PARSEMAP_ENTRY_PARAMS*) pCurrent->pszFnName;
			pCurrent->pszFnName = NULL;

			free(pCurrent->pszParamInfo);
			pCurrent->pszParamInfo = NULL;
		}
	}
}

AFX_PARSEMAP_ENTRY_PARAMS::~AFX_PARSEMAP_ENTRY_PARAMS()
{
	delete [] ppszInfo;
	delete [] ppszDefaults;
}

///////////////////////////////////////////////////////////////////////
// Entry points for HTTP Server Extensions

extern "C" DWORD WINAPI HttpExtensionProc(EXTENSION_CONTROL_BLOCK *pECB)
{
#ifdef _AFXDLL
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	DWORD dwRet;

	ISAPIASSERT(pServer != NULL);
	if (pServer == NULL)
	{
		dwRet = HSE_STATUS_ERROR;
		pECB->dwHttpStatusCode = HTTP_STATUS_SERVER_ERROR;
	}
	else
		dwRet = pServer->HttpExtensionProc(pECB);

	return dwRet;
}

extern "C" BOOL WINAPI GetExtensionVersion(HSE_VERSION_INFO *pVer)
{
#ifdef _AFXDLL
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	BOOL bRet;

	ISAPIASSERT(pServer != NULL);
	if (pServer == NULL)
		bRet = FALSE;
	else
		bRet = pServer->GetExtensionVersion(pVer);

	return bRet;
}

extern "C" BOOL WINAPI TerminateExtension(DWORD dwFlags)
{
#ifdef _AFXDLL
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

	if (pServer == NULL)
		return TRUE;
	return pServer->TerminateExtension(dwFlags);
}

///////////////////////////////////////////////////////////////////////
// CHttpAsyncContext implementation

class CHttpAsyncContext
{
public:
	CHttpAsyncContext(HANDLE hFile, LPVOID pvHeader, DWORD dwHeaderLen,
					LPVOID pvTail, DWORD dwTailLen);
	virtual ~CHttpAsyncContext();

protected:
	LPVOID m_pvHeader;
	DWORD	m_dwHeaderLen;
	LPVOID m_pvTail;
	DWORD m_dwTailLen;
	HANDLE m_hFile;
	
	friend class CHttpServerContext;
};

CHttpAsyncContext::CHttpAsyncContext(HANDLE hFile, LPVOID pvHeader,
		DWORD dwHeaderLen, LPVOID pvTail, DWORD dwTailLen)
	: m_hFile(hFile)
{
	if (pvHeader == NULL || dwHeaderLen == 0)
	{
		m_pvHeader = NULL;
		m_dwHeaderLen = 0;
	}
	else
	{
		m_pvHeader = new BYTE[dwHeaderLen+1];
		memcpy(m_pvHeader, pvHeader, dwHeaderLen);
		reinterpret_cast<BYTE*>(m_pvHeader)[dwHeaderLen] = '\0';
		m_dwHeaderLen = dwHeaderLen;
	}

	if (pvTail == NULL || dwTailLen == 0)
	{
		m_pvTail = NULL;
		m_dwTailLen = 0;
	}
	else
	{
		m_pvTail = new BYTE[dwTailLen+1];
		memcpy(m_pvTail, pvTail, dwTailLen);
		reinterpret_cast<BYTE*>(m_pvTail)[dwTailLen] = '\0';
		m_dwTailLen = dwTailLen;
	}
}

CHttpAsyncContext::~CHttpAsyncContext()
{
	if (m_hFile != NULL && m_hFile != INVALID_HANDLE_VALUE)
		CloseHandle(m_hFile);

	delete [] m_pvTail;
	delete [] m_pvHeader;
}

///////////////////////////////////////////////////////////////////////
// CHttpServerContext implementation

void CHttpServerContext::Reset()
{
#ifdef _DEBUG
	m_dwOldEndOfHeaders = 0;
#endif
	m_pStream->Reset();
}

DWORD CHttpServerContext::SetChunkSize(DWORD dwNewSize)
{
	DWORD dwReturn = m_dwChunkSize;
	m_dwChunkSize = dwNewSize;
	return dwReturn;
}

DWORD CHttpServerContext::GetChunkSize() const
{
	return m_dwChunkSize;
}

VOID WINAPI AfxIOCallback(EXTENSION_CONTROL_BLOCK* pECB,
	PVOID pvContext, DWORD /* cbIO */, DWORD /* dwError */)
{
	if (!pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_DONE_WITH_SESSION, NULL, NULL, NULL))
		ISAPITRACE("HttpExtensionProc io callback error\n");

	CHttpAsyncContext* pInfo = (CHttpAsyncContext*) pvContext;
	delete pInfo;
}

BOOL CHttpServerContext::TransmitFile(HANDLE hFile,
	DWORD dwFlags /* = HSE_IO_DISCONNECT_AFTER_SEND */,
	LPVOID pvHeader /* = NULL */, DWORD dwHeaderLen /* = 0 */,
	LPVOID pvTail /* = NULL */, DWORD dwTailLen /* = 0 */)
{
	if (hFile == INVALID_HANDLE_VALUE || GetFileType(hFile) != FILE_TYPE_DISK)
		return FALSE;

	ISAPIASSERT(dwHeaderLen == 0 || pvHeader != NULL);
	ISAPIASSERT(dwTailLen == 0 || pvTail != NULL);

	if (dwTailLen == 0 && pvTail != NULL)
		dwTailLen = lstrlen((LPCTSTR) pvTail);

	if (dwHeaderLen == 0 && pvHeader != NULL)
		dwHeaderLen = lstrlen((LPCTSTR) pvHeader);

	CHttpAsyncContext* pInfo =
		new CHttpAsyncContext(hFile, pvHeader, dwHeaderLen, pvTail, dwTailLen);
	if( pInfo == NULL )
		return FALSE;

	HSE_TF_INFO info;

	info.pszStatusCode = NULL;
	info.hFile = hFile;
	info.dwFlags = dwFlags;
	info.BytesToWrite = 0;
	info.Offset = 0;
	info.pfnHseIO = AfxIOCallback;
	info.pContext = pInfo;
	info.pHead = pInfo->m_pvHeader;
	info.HeadLength = pInfo->m_dwHeaderLen;
	info.pTail = pInfo->m_pvTail;
	info.TailLength = pInfo->m_dwTailLen;

	BOOL bResult = ServerSupportFunction(HSE_REQ_TRANSMIT_FILE, &info, 0, 0);

	if (bResult)
	{
		m_dwStatusCode = HSE_STATUS_PENDING;
		m_bSendHeaders = FALSE;
	}
	else
		delete pInfo;

	return bResult;
}

///////////////////////////////////////////////////////////////////////
// CHttpServer implementation

BOOL CHttpServer::TerminateExtension(DWORD /* dwFlags */)
{
	// okay to unload at any time, by default
	return TRUE;
}

LPTSTR CHttpServer::GetQuery(CHttpServerContext* pCtxt, LPTSTR lpszQuery)
{
	// If the request is a POST, get all of the data.  First get what's
	// already available, then read any additional data via the
	// ReadClient() call.

	memcpy(lpszQuery, (LPCTSTR) pCtxt->m_pECB->lpbData, pCtxt->m_pECB->cbAvailable);
	lpszQuery[pCtxt->m_pECB->cbAvailable] = '\0';

	if (pCtxt->m_pECB->cbAvailable < pCtxt->m_pECB->cbTotalBytes)
	{
		LPTSTR pstrTarget = lpszQuery + pCtxt->m_pECB->cbAvailable;
		DWORD cbRemaining = pCtxt->m_pECB->cbTotalBytes - pCtxt->m_pECB->cbAvailable;
		DWORD cbRead;

		while (cbRemaining > 0)
		{
			cbRead = cbRemaining;
			if (!pCtxt->ReadClient(pstrTarget, &cbRead))
			{
				ISAPITRACE("Error: only %d of %d bytes read!\n",
					pCtxt->m_pECB->cbTotalBytes - cbRemaining,
					pCtxt->m_pECB->cbTotalBytes);
				return NULL;
			}

			if (cbRead == 0)
				break;

			pstrTarget += cbRead;
			cbRemaining -= cbRead;
		}

		*pstrTarget = '\0';
	}

	pCtxt->m_dwBytesReceived = pCtxt->m_pECB->cbTotalBytes;
	return lpszQuery;
}

BOOL CHttpServer::OnWriteBody(CHttpServerContext* pCtxt, LPBYTE pbContent,
								DWORD dwSize, DWORD dwReserved /* = 0 */)
{
	BOOL bRetVal;

	if (pCtxt->m_dwChunkSize == 0)
		bRetVal = pCtxt->WriteClient(pbContent, &dwSize, dwReserved);
	else
	{
		LPBYTE pbStart = pbContent;
		DWORD dwBytesLeft = dwSize;
		bRetVal = TRUE;

		while (bRetVal && (dwBytesLeft > 0))
		{
			DWORD dwThisChunk = min(dwBytesLeft, pCtxt->m_dwChunkSize);
			bRetVal = pCtxt->WriteClient(pbStart, &dwThisChunk, dwReserved);
			pbStart += dwThisChunk;
			dwBytesLeft -= dwThisChunk;
		}
	}

	return bRetVal;
}

DWORD CHttpServer::HttpExtensionProc(EXTENSION_CONTROL_BLOCK *pECB)
{
	DWORD dwRet = HSE_STATUS_SUCCESS;
	LPTSTR pszPostBuffer = NULL;
	LPTSTR pszQuery;
	LPTSTR pszCommand = NULL;
	int nMethodRet;
	LPTSTR pstrLastChar;
	DWORD cbStream = 0;
	BYTE* pbStream = NULL;
	CHttpServerContext ctxtCall(pECB);

	pECB->dwHttpStatusCode = 0;

	ISAPIASSERT(NULL != pServer);
	if (pServer == NULL)
	{
		dwRet = HSE_STATUS_ERROR;
		goto CleanUp;
	}

	// get the query

	if (lstrcmpi(pECB->lpszMethod, szGet) == 0)
	{
		pszQuery = pECB->lpszQueryString;
		ctxtCall.m_dwBytesReceived = lstrlen(pszQuery);
	}
	else if (lstrcmpi(pECB->lpszMethod, szPost) == 0)
	{
		if(pECB->cbTotalBytes == 0xffffffff)
		{
			dwRet = HSE_STATUS_ERROR ;
			pECB->dwHttpStatusCode = HTTP_STATUS_REQUEST_TOO_LARGE ;
			goto CleanUp;
		}
		
		if(pECB->cbTotalBytes < pECB->cbAvailable)
		{
			dwRet = HSE_STATUS_ERROR ;
			pECB->dwHttpStatusCode = HTTP_STATUS_BAD_REQUEST;
			goto CleanUp;
		}
		
		pszCommand = pECB->lpszQueryString;
		pszPostBuffer = new TCHAR[pECB->cbTotalBytes + 1];
		if( pszPostBuffer == NULL )
		{
			dwRet = HSE_STATUS_ERROR;
			goto CleanUp;
		}
		pszQuery =  GetQuery(&ctxtCall, pszPostBuffer);
		if (pszQuery == NULL)
		{
			dwRet = HSE_STATUS_ERROR;
			goto CleanUp;
		}
	}
	else
	{
		ISAPITRACE1(_T("Error: Unrecognized method: %s\n"), pECB->lpszMethod);
		dwRet = HSE_STATUS_ERROR;
		goto CleanUp;
	}

	// trim junk that some browsers put at the very end

	pstrLastChar = pszQuery + ctxtCall.m_dwBytesReceived -1;
	while ((*pstrLastChar == ' ' || *pstrLastChar == '\n' ||
		   *pstrLastChar == '\r') && pstrLastChar > pszQuery)
	{
		*pstrLastChar-- = '\0';
	}

	// do something about it

	if (!pServer->InitInstance(&ctxtCall))
		dwRet = HSE_STATUS_ERROR;
	else
	{
		pECB->dwHttpStatusCode = HTTP_STATUS_OK;
		try {
			nMethodRet = pServer->CallFunction(&ctxtCall, pszQuery, pszCommand);
		}
		catch (...)
		{
			ISAPITRACE1(_T("Error: command %s caused an unhandled exception!\n"),
				pszQuery);
			nMethodRet = callNoStackSpace;
		}

		// was an error caused by trying to dispatch?

		if (nMethodRet != callOK && pECB->dwHttpStatusCode == HTTP_STATUS_OK)
		{
			dwRet = HSE_STATUS_ERROR;
			switch (nMethodRet)
			{
			case callNoStream:
				pECB->dwHttpStatusCode = HTTP_STATUS_NO_CONTENT;
				break;

			case callParamRequired:
			case callBadParamCount:
			case callBadParam:
				pECB->dwHttpStatusCode = HTTP_STATUS_BAD_REQUEST;
				break;

			case callBadCommand:
				pECB->dwHttpStatusCode = HTTP_STATUS_NOT_IMPLEMENTED;
				break;

			case callNoStackSpace:
			default:
				pECB->dwHttpStatusCode = HTTP_STATUS_SERVER_ERROR;
				break;
			}
		}

		// if there was no error or the user said they handled
		// the error, prepare to spit out the generated HTML

		if (nMethodRet == callOK ||
			OnParseError(&ctxtCall, nMethodRet))
		{
			cbStream = ctxtCall.m_pStream->GetStreamSize();
			pbStream = ctxtCall.m_pStream->Detach();
		}
	}

CleanUp:
	// if there was an error, return an appropriate status
	TCHAR szResponse[64];
	BuildStatusCode(szResponse, pECB->dwHttpStatusCode);

	DWORD dwSize = cbStream - ctxtCall.m_dwEndOfHeaders;
	LPBYTE pbContent = NULL;
	BYTE cSaved = 0;

	if (pbStream != NULL && ctxtCall.m_bSendHeaders)
	{
		cSaved = pbStream[ctxtCall.m_dwEndOfHeaders];
		pbStream[ctxtCall.m_dwEndOfHeaders] = '\0';
		pbContent = &pbStream[ctxtCall.m_dwEndOfHeaders];
	}

	if (ctxtCall.m_bSendHeaders &&
		!ctxtCall.ServerSupportFunction(HSE_REQ_SEND_RESPONSE_HEADER,
			szResponse, 0, (LPDWORD) pbStream) &&
		::GetLastError() != 10054)  // WSAECONNRESET
	{
		pECB->dwHttpStatusCode = HTTP_STATUS_SERVER_ERROR;
		dwRet = HSE_STATUS_ERROR;
#ifdef _DEBUG
		DWORD dwCause = ::GetLastError();
		ISAPITRACE1("Error: Unable to write headers: 0x%8.8X!\n", dwCause);
#endif
	}
	else
	{
		if (pbContent != NULL)
		{
			BOOL bWorked = TRUE;

			// write a newline to separate content from headers

			if (ctxtCall.m_bSendHeaders)
			{
				*pbContent = cSaved;
				DWORD dwNewLineSize = 2;
				bWorked = ctxtCall.WriteClient(_T("\r\n"), &dwNewLineSize, 0);
			}

			if (!bWorked || !OnWriteBody(&ctxtCall, pbContent, dwSize))
			{
				dwRet = HSE_STATUS_ERROR;
				pECB->dwHttpStatusCode = HTTP_STATUS_SERVER_ERROR;
#ifdef _DEBUG
				DWORD dwCause = ::GetLastError();
				ISAPITRACE1("Error: Unable to write content body: 0x%8.8X!\n",
					dwCause);
#endif
			}
		}
		else
			ISAPITRACE("Error: No body content!\n");
	}

	if (pbStream != NULL)
		ctxtCall.m_pStream->Free(pbStream);

	if (ctxtCall.m_dwStatusCode != DWORD(-1))
		dwRet = ctxtCall.m_dwStatusCode; 
	if (dwRet == HSE_STATUS_SUCCESS)
		pECB->dwHttpStatusCode = HTTP_STATUS_OK;

	if (pszPostBuffer != NULL)
		delete [] pszPostBuffer;

	return dwRet;
}

void CHttpServer::BuildStatusCode(LPTSTR pszResponse, DWORD dwCode)
{
	ISAPIASSERT(pszResponse != NULL);

	HTTPStatusInfo* pInfo = statusStrings;

	while (pInfo->pstrString != NULL)
	{
		if (dwCode == pInfo->dwCode)
			break;
		pInfo++;
	}
	if (pInfo->pstrString != NULL)
		wsprintf(pszResponse, _T("%d %s"), dwCode, pInfo->pstrString);
	else
	{
		ISAPIASSERT(dwCode != HTTP_STATUS_OK);
		ISAPITRACE1("Warning: Nonstandard status code %d\n", dwCode);
		BuildStatusCode(pszResponse, HTTP_STATUS_OK);
	}
}

BOOL CHttpServer::GetExtensionVersion(HSE_VERSION_INFO *pVer)
{
	pVer->dwExtensionVersion = MAKELONG(HSE_VERSION_MINOR, HSE_VERSION_MAJOR);
	pVer->lpszExtensionDesc[0] = '\0';
	return TRUE;
}


CHttpServer::CHttpServer(TCHAR cDelimiter /* = '&' */)
	: m_cTokenDelimiter(cDelimiter)
{
	ISAPIASSERT(pServer == NULL);   // only one server instance
	pServer = this;
}

CHttpServer::~CHttpServer()
{
	pServer = NULL;
}

BOOL CHttpServer::OnParseError(CHttpServerContext* pCtxt, int nMethodRet)
{
	UNUSED(nMethodRet);
	UINT nResource = 0;

	if (pCtxt->m_pStream != NULL)
	{
		TCHAR szBuffer[132];
		TCHAR szTitle[256];
		LPCTSTR pszObject = NULL;

		switch (pCtxt->m_pECB->dwHttpStatusCode)
		{
		case HTTP_STATUS_BAD_REQUEST:
			nResource = AFX_IDS_HTTP_BAD_REQUEST;

#ifndef _AFX_ISAPI_FULL_ERROR_MESSAGES
			pszObject = _T("");
#else
			if (pCtxt->m_pECB->lpszQueryString)
				pszObject = pCtxt->m_pECB->lpszQueryString;
			else
				pszObject = pCtxt->m_pECB->lpszPathInfo;
#endif	// _AFX_ISAPI_FULL_ERROR_MESSAGES
			break;

		case HTTP_STATUS_AUTH_REQUIRED:
			nResource = AFX_IDS_HTTP_AUTH_REQUIRED;
			break;

		case HTTP_STATUS_FORBIDDEN:
			nResource = AFX_IDS_HTTP_FORBIDDEN;
			break;

		case HTTP_STATUS_NOT_FOUND:
			nResource = AFX_IDS_HTTP_NOT_FOUND;
			break;

		case HTTP_STATUS_SERVER_ERROR:
			nResource = AFX_IDS_HTTP_SERVER_ERROR;
			break;

		case HTTP_STATUS_NOT_IMPLEMENTED:
			nResource = AFX_IDS_HTTP_NOT_IMPLEMENTED;
#ifndef _AFX_ISAPI_FULL_ERROR_MESSAGES
			pszObject = _T("");
#else
			pszObject = pCtxt->m_pECB->lpszQueryString;
#endif	// _AFX_ISAPI_FULL_ERROR_MESSAGES			
			break;

		default:
			nResource = AFX_IDS_HTTP_NO_TEXT;
			pszObject = (LPCTSTR)(DWORD_PTR) pCtxt->m_pECB->dwHttpStatusCode;
			break;
		}

		HINSTANCE hRes;
		hRes = AfxGetResourceHandle();

#ifdef _AFXDLL
		if (AfxLoadString(nResource, szBuffer, _countof(szBuffer)) > 0)
#else
		if (::LoadString(hRes, nResource, szBuffer, _countof(szBuffer)) > 0)
#endif
		{
			pCtxt->Reset();
			CHttpServer::StartContent(pCtxt);

			if (::LoadString(hRes, AFX_IDS_HTTP_TITLE, szTitle, _countof(szTitle)) > 0)
			{
				TCHAR szTitleCopy[512];
				_sntprintf(szTitleCopy, 512, szTitle, pCtxt->m_pECB->dwHttpStatusCode);
				szTitleCopy[511] = 0; // because _sntprintf does not guarantee NULL-termination
				*pCtxt << szTitleCopy;
			}

			if (pszObject != NULL)
			{
				UINT nLen = lstrlen(szBuffer) + lstrlen(pszObject) + 1;
				TCHAR* pstrFormat = new TCHAR[lstrlen(szBuffer) + lstrlen(pszObject) + 1];
				if (pstrFormat != NULL)
				{
					_sntprintf(pstrFormat, nLen, szBuffer, pszObject);
					pstrFormat[nLen-1] = 0;	// because _sntprintf does not guarantee NULL-termination
					*pCtxt << pstrFormat;
					delete [] pstrFormat;
				}
			}
			else
				*pCtxt << szBuffer;

			CHttpServer::EndContent(pCtxt);
		}
		else
		{
			ISAPITRACE1("Error: Couldn't load string %d\n", nResource);
			nResource = 0;
		}
	}

	if (nResource == 0)
		ISAPITRACE1("Error: Unhandled parsing error code %d\n", nMethodRet);

	return nResource != 0;
}

void CHttpServer::AddHeader(CHttpServerContext* pCtxt,
	LPCTSTR pszString) const
{

#ifdef _DEBUG
	// Can't call AddHeader() after writing directly to the stream.
	ISAPIASSERT(pCtxt->m_dwOldEndOfHeaders == pCtxt->m_pStream->GetStreamSize());
#endif

	*pCtxt << pszString;
	pCtxt->m_dwEndOfHeaders = pCtxt->m_pStream->GetStreamSize();

#ifdef _DEBUG
	pCtxt->m_dwOldEndOfHeaders = pCtxt->m_dwEndOfHeaders;
#endif
}

void CHttpServer::StartContent(CHttpServerContext* pCtxt) const
{
	AddHeader(pCtxt, szContentType);
}

void CHttpServer::WriteTitle(CHttpServerContext* pCtxt) const
{
	*pCtxt << szStartTitle;
	*pCtxt << GetTitle();
	*pCtxt << szEndTitle;
}

LPCTSTR CHttpServer::GetTitle() const
{
	return szDefaultTitle;
}

void CHttpServer::EndContent(CHttpServerContext* pCtxt) const
{
	*pCtxt << szEndBody;
}

BOOL CHttpServer::InitInstance(CHttpServerContext*)
{
	return TRUE;
}

int CHttpServer::CallFunction(CHttpServerContext* pCtxt,
	LPTSTR pszQuery, LPTSTR pszCommand)
{
	int nRet;

	AFX_PARSEMAP_ENTRY* pParams;
	const AFX_PARSEMAP* pMap;
	const AFX_PARSEMAP_ENTRY* pFn;

	ISAPIASSERT(pCtxt->m_pStream == NULL);
	pCtxt->m_pStream = ConstructStream();
	if (pCtxt->m_pStream == NULL)
		nRet = callNoStream;
	else
	{
		ISAPIASSERT(pszQuery != NULL);
		if (pszQuery == NULL)
			nRet = callBadCommand;
		else
		{
			LPTSTR pszMethod;
			LPTSTR pszParams;

			// did the user specify a command via "MfcISAPICommand"?

			LPTSTR pszHiddenCommand = _tcschr(pszQuery, '=');
			if (pszHiddenCommand != NULL)
			{
				*pszHiddenCommand = '\0';

				// is it there?

				if (lstrcmpi(pszQuery, _T("MfcISAPICommand")) == 0)
				{
					// did they have a method, too?

					pszMethod = pszHiddenCommand+1;
					if (*pszMethod == '\0')
						pszParams = pszMethod;
					else
					{
						pszParams = _tcschr(pszMethod, m_cTokenDelimiter);
						if (pszParams != NULL && *pszParams != '\0')
							*pszParams++ = '\0';
					}

					// if we find it, we can call it

					pFn = LookUp(pszMethod, pMap, pParams);
					if (pFn != NULL)
						goto MakeTheCall;
				}

				// we didn't find the command, or we didn't have
				// "MfcISAPICommand", so we'll try and process things
				// normally...

				*pszHiddenCommand = '=';
			}

			if (pszCommand == NULL)
			{
				// got something via a GET method
				// is the first thing a parameter or a command?

				LPTSTR pszEquals;
				LPTSTR pszQMark;

				pszParams = _tcschr(pszQuery, m_cTokenDelimiter);
				pszQMark = _tcschr(pszQuery, '?');

				// Parameters start at the first delimiter

				if (pszParams == NULL || (pszQMark != NULL && (pszQMark < pszParams)))
				{
					pszParams = pszQMark;

					// if the command ends in question mark
					// and nothing else, ignore it
					if (pszQMark != NULL && pszQMark[1] == '\0')
					{
						*pszQMark = '\0';
						pszParams = NULL;
					}
				}

				// Does an equals sign show up before the first delimiter?

				pszEquals = _tcschr(pszQuery, '=');
				if (pszEquals == NULL || pszEquals > pszParams)
				{
					// It might be a command. If it isn't blank,
					// try and find it in the parameter map--if
					// we can't, then assume it is a parameter.

					pszMethod = pszQuery;
					if (*pszMethod != '\0')
					{
						TCHAR cTemp = 0;
						if (pszParams != NULL)
						{
							cTemp = *pszParams;
							*pszParams++ = '\0';
						}

						pFn = LookUp(pszMethod, pMap, pParams);
						if (pFn != NULL)
							goto MakeTheCall;

						if (pszParams != NULL)
							*--pszParams = cTemp;
					}
				}

				// we can be sure it's a parameter
				if (pszQMark == NULL || pszQMark >= pszParams)
				{
					// default command, params as supplied
					pszMethod = NULL;
					pszParams = pszQuery;
				}
				else
				{
					pszMethod = pszQuery;
					*pszQMark++ = '\0';
					pszParams = pszQMark;
				}
			}
			else
			{
				// with a POST, the verb arrives seperately
				pszMethod = pszCommand;
				pszParams = pszQuery;
			}

			// is it a default verb?
			if (pszMethod != NULL && lstrlen(pszMethod) == 0)
				pszMethod = NULL;

			pFn = LookUp(pszMethod, pMap, pParams);

MakeTheCall:
			if (pFn == NULL)
				nRet = callBadCommand;
			else
			{
				pCtxt->m_pStream->InitStream();
				nRet = CallMemberFunc(pCtxt, pFn, pParams, pszParams);
			}
		}
	}

	return nRet;
}

CHtmlStream* CHttpServer::ConstructStream()
{
	return new CHtmlStream();
}

const AFX_PARSEMAP_ENTRY* CHttpServer::LookUp(LPCTSTR pszMethod,
	const AFX_PARSEMAP*& pMap, AFX_PARSEMAP_ENTRY*& pParams,
	AFX_PISAPICMD pCmdDefault /* = NULL */)
{
	UINT iEntry;
	LPCTSTR pszFnName;
	const AFX_PARSEMAP* pParseMap = GetParseMap();
	const AFX_PARSEMAP* pBaseMap;
	const AFX_PARSEMAP_ENTRY* pRet = NULL;

#ifdef _AFXDLL
	while (pRet == NULL)
#else
	while (pParseMap != NULL && pRet == NULL)
#endif
	{
		UINT cEntries = (*pParseMap->pfnGetNumMapEntries)();
		const AFX_PARSEMAP_ENTRY* pCurrent = pParseMap->lpEntries;

		for (iEntry = 0; iEntry < cEntries && pRet == NULL; iEntry++, pCurrent++)
		{
#ifdef _DEBUG
			// make sure not 2 parameter maps in a row

			if (pCurrent->pfn == NULL && (iEntry+1 < cEntries))
				ISAPIASSERT(pCurrent[1].pfn != NULL);
#endif

			// skip parameter maps

			if (pCurrent->pfn == NULL)
				continue;

			pszFnName = pCurrent->pszFnName;

			// if the caller wants the default command, find that--
			// if the caller wants something specific, perform a compare
			// otherwise, see if we recursed to look up the default command

			if (pCmdDefault == NULL)
			{
				if (pszMethod == NULL && pCurrent->pszArgs == NULL)
					pRet = pCurrent;
				else if (pszMethod != NULL && pCurrent->pszArgs != NULL
					&& lstrcmpi(pszFnName, pszMethod) == 0)
					pRet = pCurrent;
			}
			else if (pCurrent->pfn == pCmdDefault && pCurrent->pszArgs != NULL)
				pRet = pCurrent;

			if (pRet != NULL)
			{
				// if we need the default, recurse to find it by name
				if (pszMethod == NULL && pCmdDefault == NULL)
					return LookUp(NULL, pMap, pParams, pCurrent->pfn);

				// found it!  see if there are parameters

				if (iEntry+1 >= cEntries || pCurrent[1].pfn != NULL)
				{
					pParams = NULL;
					pMap = NULL;
				}
				else
				{
					pParams = (AFX_PARSEMAP_ENTRY*) &(pCurrent[1]);
					pMap = pParseMap;
				}
			}
		}

#ifdef _AFXDLL
		if (pParseMap->pfnGetBaseMap == NULL)
			break;
		pBaseMap = (*pParseMap->pfnGetBaseMap)();
#else
		pBaseMap = pParseMap->pBaseMap;
#endif

		// catch simple mistake of BEGIN_PARSE_MAP(CMyClass, CMyClass)
		ISAPIASSERT(pBaseMap != pParseMap);
		pParseMap = pBaseMap;
	}

	// no matching entry ?
	if (pRet == NULL)
		ISAPITRACE1(_T("Warning: no handler for command '%s'\n"), pszMethod);
	return pRet;
}

UINT PASCAL CHttpServer::GetStackSize(const BYTE* pbParams)
{
	// size of arguments on stack when pushed by value
	static const UINT rgnByValue[] =
	{
		sizeof(_STACK_SHORT),         // ITS_I2
		sizeof(_STACK_LONG),        // ITS_I4
		sizeof(_STACK_FLOAT),       // ITS_R4
		sizeof(_STACK_DOUBLE),      // ITS_R8
		sizeof(LPCTSTR),            // ITS_PSTR
		0,                          // ITS_EMPTY
		sizeof(LPCTSTR)+sizeof(_STACK_LONG),     // ITS_RAW
		sizeof(_STACK_LONGLONG),    // ITS_I8
		sizeof(LPCTSTR),			// ITS_ARGLIST
	};
	// sizeof 'this' pointer
	UINT nCount = sizeof(CHttpServer*);
#ifdef _ALIGN_STACK
	nCount = (nCount + (_ALIGN_STACK-1)) & ~(_ALIGN_STACK-1);
#endif

	// count arguments
	ISAPIASSERT(pbParams != NULL);
	while (*pbParams != 0 && *pbParams != IT_EMPTY)
	{
		// align if necessary
		// get and add appropriate byte count

#ifdef _ALIGN_DOUBLES
		// align doubles on 8 byte for some platforms
		if (*pbParams == IT_R8)
			nCount = (nCount + _ALIGN_DOUBLES-1) & ~(_ALIGN_DOUBLES-1);
#endif

		// *pbParams must fit in the rgnByValue array
		ISAPIASSERT(*pbParams >= 1 && *pbParams <= sizeof(rgnByValue)/sizeof(UINT));
		nCount += rgnByValue[*pbParams-1];

#ifdef _ALIGN_STACK
		nCount = (nCount + (_ALIGN_STACK-1)) & ~(_ALIGN_STACK-1);
#endif
		++pbParams;
	}
#if defined(_ALIGN_DOUBLES) && defined(_SHADOW_DOUBLES)
	// align doubles on 8 byte for some platforms
	nCount = (nCount + _ALIGN_DOUBLES-1) & ~(_ALIGN_DOUBLES-1);
#endif
	return nCount;
}


// indirect call helper (see OLECALL.CPP for implementation)

extern "C" DWORD AFXISAPI
_AfxParseCall(AFX_PISAPICMD pfn, void* pArgs, UINT nSizeArgs);

// invoke standard method given IDispatch parameters/return value, etc.

int CHttpServer::CallMemberFunc(CHttpServerContext* pCtxt,
	const AFX_PARSEMAP_ENTRY* pEntry,
	AFX_PARSEMAP_ENTRY* pParams, LPTSTR pszParams)
{
	ISAPIASSERT(NULL != pEntry);	
	AFX_PISAPICMD pFunc = pEntry->pfn;
	ISAPIASSERT(NULL != pFunc);
	int nRet = callOK;

	// get default function and parameters
	BYTE bNoParams = 0;

	const BYTE* pbParams = (const BYTE*)pEntry->pszArgs;
	if (NULL == pbParams)
		pbParams = &bNoParams;
	UINT nParams = lstrlenA((LPCSTR)pbParams);

	AFX_PARSEMAP_ENTRY_PARAMS *pDefaultParams = NULL;
	if (pParams != NULL)
	{
		if (pParams->pszFnName == NULL)
			nRet = ParseDefaultParams(pParams, nParams, pDefaultParams, pbParams);
		else
			pDefaultParams = (AFX_PARSEMAP_ENTRY_PARAMS*) pParams->pszFnName;
	}

	if (nRet == callOK)
	{
		// get default function and return value information
		AFX_PISAPICMD pfn = pEntry->pfn;

		// determine size of arguments and allocate stack space
		// include space for our context pointer
 		UINT nSizeArgs = GetStackSize(pbParams) + sizeof(_STACK_PTR);
		ISAPIASSERT(nSizeArgs != 0);

		if (nSizeArgs < _STACK_MIN)
			nSizeArgs = _STACK_MIN;

		BYTE* pStack = NULL;

		__try
		{
			pStack = (BYTE*)_alloca(nSizeArgs + _SCRATCH_SIZE);
		}
		__except ((EXCEPTION_STACK_OVERFLOW == GetExceptionCode()) ?
					EXCEPTION_EXECUTE_HANDLER :
					EXCEPTION_CONTINUE_SEARCH)
		{
			_resetstkoflw();
		}
		
		if (pStack == NULL)
		{
			ISAPITRACE0("Error: stack overflow in CHttpServer::CallMemberFunc()!\n");
			return callNoStackSpace;
		}

		if (pDefaultParams != NULL)
		{
#ifndef _SHADOW_DOUBLES
			nRet = PushDefaultStackArgs(pStack, pCtxt, pbParams, pszParams,
				pDefaultParams);
#else
			nRet = PushDefaultStackArgs(pStack, pCtxt, pbParams, pszParams,
				pDefaultParams, nSizeArgs);
#endif
		}		
		else
#ifndef _SHADOW_DOUBLES
			nRet = PushStackArgs(pStack, pCtxt, pbParams, pszParams);
#else
			nRet = PushStackArgs(pStack, pCtxt, pbParams, pszParams, nSizeArgs);
#endif
		pStack += _STACK_OFFSET;

		if (nRet == 0)
		{
			DWORD (AFXISAPI *pfnInet)(AFX_PISAPICMD, void*, UINT) =
				&_AfxParseCall;
			pfnInet(pfn, pStack, nSizeArgs);
		}
	}

	return nRet;
}

int CHttpServer::CountParams(LPCTSTR pszCommandLine, int& nCount)
{
	BOOL bInQuote = FALSE;
	BOOL bInSpace = TRUE;
	LPCTSTR pszWalker = pszCommandLine;
	int nRetCode = callOK;

	nCount = 0;
	if (pszCommandLine != NULL)
	{
		while (*pszWalker != '\0')
		{
			if (bInSpace)
			{
				// this is invalid syntax
				ISAPIASSERT(*pszWalker != '\'');
				if (*pszWalker == '\'')
				{
					nRetCode = callMissingQuote;
					break;
				}

				if (!_istspace(*pszWalker))
				{
					nCount++;
					bInSpace = FALSE;
				}
			}
			else
			{
				if (*pszWalker == '\'')
					bInQuote = !bInQuote;
				else if (!bInQuote &&
					(_istspace(*pszWalker) || *pszWalker == m_cTokenDelimiter))
					bInSpace = TRUE;
			}

			pszWalker++;
		}

		// can't have only whitespace
		if (nCount == 0 && bInSpace)
		{
			nRetCode = callMissingParams;
			ISAPIASSERT(nCount > 0 || !bInSpace);
		}
		// unclosed quoted string?
		else if (bInQuote)
		{
			nRetCode = callMissingQuote;
			ISAPIASSERT(!bInQuote);
		}
	}

	return nRetCode;
}

int CHttpServer::ParseDefaultParams(AFX_PARSEMAP_ENTRY* pParams,
	int nParams, AFX_PARSEMAP_ENTRY_PARAMS*& pBlock, const BYTE* pbParams)
{
	int nRet = callOK;

	LPSTR pszWalker;
	BOOL bInQuote;
	BOOL bInSpace;
	BOOL bInEquals;
	BOOL bMandatory = TRUE;

	// only if we haven't done it already

	AFX_PARSEMAP_ENTRY_PARAMS* pParBlock = NULL;
	if (pParams->pszFnName == NULL)
	{
		ISAPIASSERT(pParams->pszParamInfo == NULL);

		// can't have empty param string
		ISAPIASSERT(*pParams->pszArgs != '\0');

		if (*pParams->pszArgs != '\0')
		{
			pParBlock = new AFX_PARSEMAP_ENTRY_PARAMS;
			if( pParBlock == NULL )
			{
				nRet = callNoMemory;
				goto CleanUp;
			}
			pBlock = pParBlock;
			memset(pParBlock, 0, sizeof(AFX_PARSEMAP_ENTRY_PARAMS));
			pParams->pszFnName = (LPTSTR) pParBlock;

			// start by finding how many parameters we have
			nRet = CountParams(pParams->pszArgs, pParBlock->nParams);
			if (nRet == callOK)
				pParams->pszParamInfo = _tcsdup(pParams->pszArgs);
		}

		if (pParBlock == NULL || nRet != callOK)
		{
			if (nRet == callOK)
				nRet = callMissingParams;
			goto CleanUp;
		}

		// wrong number of parameters?
		if (nParams != pParBlock->nParams)
		{
			nRet = callBadParamCount;
			ISAPIASSERT(nParams == pParBlock->nParams);
		}
		// it's a winner!
		else
		{
			pParBlock->ppszInfo =
				new LPTSTR[pParBlock->nParams *2];
			pParBlock->ppszDefaults =
				new BYTE[pParBlock->nParams * sizeof(double)];

			if( pParBlock->ppszInfo == NULL || pParBlock->ppszDefaults == NULL )
			{
				nRet = callNoMemory;
				goto CleanUp;
			}

			bInQuote = FALSE;
			bInSpace = TRUE;
			bInEquals = FALSE;
			int nStorage = 0;

			for (pszWalker = pParams->pszParamInfo;
				*pszWalker != '\0'; pszWalker++)
			{
				if (bInSpace)
				{
					// this is invalid syntax
					ISAPIASSERT(*pszWalker != '\'' && *pszWalker != '=');
					if (*pszWalker == '\'' || *pszWalker == '=')
					{
						nRet = callMissingQuote;
						break;
					}

					if (!_istspace(*pszWalker))
					{
						pParBlock->ppszInfo[nStorage++] = pszWalker;
						bInSpace = FALSE;
					}
				}
				else
				{
					if (bInEquals)
					{
						if (_istspace(*pszWalker) && bInQuote)
							continue;

						if (*pszWalker == '\'' || _istspace(*pszWalker))
						{
							if (bInQuote || _istspace(*pszWalker))
							{
								*pszWalker = '\0';
								bInEquals = FALSE;
								bInSpace = TRUE;
								bInQuote = FALSE;
							}
							else
							{
								pParBlock->ppszInfo[nStorage-1]++;
								bInQuote = TRUE;
							}
						}
					}
					else
					{
						// parameter with no default
						if (_istspace(*pszWalker))
						{
							// can't have required param after optional params
							ISAPIASSERT(bMandatory);
							if (!bMandatory)
							{
								nRet = callBadParam;
								goto CleanUp;
							}

							*pszWalker = '\0';
							bInSpace = TRUE;
							pParBlock->ppszInfo[nStorage++] = NULL;
						}
						// end of parameter name with default
						else if (*pszWalker == '=')
						{
							bMandatory = FALSE;
							bInEquals = TRUE;
							*pszWalker = '\0';
							pParBlock->ppszInfo[nStorage++] = pszWalker+1;
						}
						// bad syntax--quote in param name
						else if (*pszWalker == '\'')
						{
							ISAPIASSERT(*pszWalker != '\'');
							nRet = callMissingQuote;
							break;
						}
					}
				}
			}

			// handle case of no default for final param
			if (nStorage & 1)
				pParBlock->ppszInfo[nStorage] = NULL;

			int nIndex;
			for (nIndex = 0; nIndex < pParBlock->nParams; nIndex++)
			{
				if (pParBlock->ppszInfo[2*nIndex+1] != NULL)
#ifndef _SHADOW_DOUBLES
					StoreStackParameter(
						&pParBlock->ppszDefaults[sizeof(double) * nIndex],
						pbParams[nIndex], pParBlock->ppszInfo[2*nIndex+1]);
#else
					StoreStackParameter(
						&pParBlock->ppszDefaults[sizeof(double) * nIndex],
						pbParams[nIndex], pParBlock->ppszInfo[2*nIndex+1],
						0, FALSE);
#endif
				else
					pParBlock->nRequired++;
			}
		}
	}
	return nRet;

CleanUp:
	if( pParBlock != NULL )
	{
		if( pParBlock->ppszInfo != NULL )
			delete [] pParBlock->ppszInfo;
		if( pParBlock->ppszDefaults != NULL )
			delete [] pParBlock->ppszDefaults;
		delete pParBlock;
		pBlock = NULL;
	}
	return nRet;
}


// push arguments on stack appropriate for C++ call (compiler dependent)
#ifndef _SHADOW_DOUBLES
int CHttpServer::PushDefaultStackArgs(BYTE* pStack,
	CHttpServerContext* pCtxt,
	const BYTE* pbParams, LPTSTR pszParams,
	AFX_PARSEMAP_ENTRY_PARAMS* pDefParams)
#else
int CHttpServer::PushDefaultStackArgs(BYTE* pStack,
	CHttpServerContext* pCtxt,
	const BYTE* pbParams, LPTSTR pszParams,
	AFX_PARSEMAP_ENTRY_PARAMS* pDefParams, int nSizeArgs)
#endif
{
	ISAPIASSERT(pStack != NULL);

	LPTSTR pszCurParam = NULL;
	int nExplicit = 0;
	LPBYTE pFlags = NULL;
	int nPushedExplicit = 0;
	int nRetVal = callOK;
	LPTSTR pszLineEnd;
	BYTE* ppszValues = NULL;	// pointers to coerced actual values

	// keep a list of flags to know what's pushed and what's not

	pFlags = new BYTE[pDefParams->nParams];
	if( pFlags == NULL )
	{
		nRetVal = callNoMemory;
		goto CleanUp;
	}

	memset(pFlags, 0, pDefParams->nParams);

	// C++ member functions use the __thiscall convention, where parameters
	//  are pushed last to first.  Assuming the stack grows down, this means
	//  that the first parameter is at the lowest memory address in the
	//  stack frame and the last parameter is at the highest address.

	// push the 'this' pointer

	*(_STACK_PTR*)pStack = (_STACK_PTR)this;
	pStack += sizeof(_STACK_PTR);

	// push our context pointer

	*(_STACK_PTR*)pStack = (_STACK_PTR)pCtxt;
	pStack += sizeof(_STACK_PTR);

	// copy the default argument list to the usable argument list
	ppszValues = new BYTE[pDefParams->nParams * sizeof(double)];
	if( ppszValues == NULL )
	{
		nRetVal = callNoMemory;
		goto CleanUp;
	}

	memcpy(ppszValues, pDefParams->ppszDefaults,
		sizeof(double) * pDefParams->nParams);

	// push the arguments, if explicitly supplied

	if (pszParams != NULL)
	{
		pszLineEnd = pszParams + lstrlen(pszParams);

		TCHAR szTokens[2];
		szTokens[0] = m_cTokenDelimiter;
		szTokens[1] = '\0';

		ISAPIASSERT(pbParams != NULL);
		const BYTE* pb;
		for (pb = pbParams; *pb != '\0'; ++pb)
		{
			if (*pb == IT_EMPTY || *pb == IT_RAW || *pb == IT_ARGLIST)
			{
				// can't have ITS_EMPTY, IT_RAW, or IT_ARGLIST with other types!
				ISAPIASSERT(pb == pbParams);
				break;
			}

			if (pszParams == NULL)
				break;
			pszCurParam = _tcstok(pszParams, szTokens);
			if (pszCurParam == NULL)
				break;

			// does this param have a name?
			LPTSTR pszValue = _tcschr(pszCurParam, '=');
			if (pszValue != NULL)
			{
				*pszValue++ = '\0';

				// find the parameter in our param block

				int nIndex;
				BOOL bFound = FALSE;
				for (nIndex = 0; nIndex < pDefParams->nParams; nIndex++)
				{
					if (lstrcmpi(pDefParams->ppszInfo[nIndex*2], pszCurParam) == 0)
					{
						bFound = TRUE;
						break;
					}
				}

				// something we don't recognize?
				if (!bFound)
				{
					nRetVal = callBadParam;
					goto CleanUp;
				}

				pszParams = pszValue + lstrlen(pszValue);
				if (pszParams != pszLineEnd)
					pszParams++;

				// if this parameter has a default and there's
				// no value for the parameter after the equal sign,
				// let the default value prevail

				if (*pszValue != '\0' ||
					pDefParams->ppszInfo[2*nIndex+1] == NULL)
				{
#ifndef _SHADOW_DOUBLES
					StoreStackParameter(
						&(ppszValues[nIndex*sizeof(double)]),
						pbParams[nIndex], pszValue);
#else
					StoreStackParameter(
						&(ppszValues[nIndex*sizeof(double)]),
						pbParams[nIndex], pszValue, 0, FALSE);
#endif

					// if this has no default, it counts as explicit, too
					if (pDefParams->ppszInfo[2*nIndex+1] == NULL)
						nExplicit++;
				}

				// if we've already pushed this parameter, or if we've
				// already pushed this many explicit params, make an error
				if (pFlags[nIndex] != 0 || nIndex < nPushedExplicit)
				{
					nRetVal = callBadParam;
					goto CleanUp;
				}
				pFlags[nIndex] = 1;
			}
			else
			{
				// not allowed to have optional params before required params
				if (nExplicit != 0)
				{
					nRetVal = callBadParam;
					goto CleanUp;
				}

				pszParams += lstrlen(pszCurParam);
				if (pszParams != pszLineEnd)
					pszParams++;

#ifndef _SHADOW_DOUBLES
				pStack = StoreStackParameter(pStack, *pb, pszCurParam);
#else
				pStack = StoreStackParameter(pStack, *pb, pszCurParam,
					nSizeArgs, TRUE);
#endif

				if (pFlags[nPushedExplicit] != 0)
				{
					nRetVal = callBadParam;
					goto CleanUp;
				}
				pFlags[nPushedExplicit] = 1;
				nPushedExplicit++;
			}
		}

		// any unused parameters?

		LPTSTR pszMoreParams;
		pszMoreParams = _tcschr(pszParams, m_cTokenDelimiter);

		if (*pb == '\0' && (pszMoreParams != NULL || *pszParams != '\0'))
		{
			nRetVal = callBadParamCount;
			goto CleanUp;
		}
	}

	// were any arguments without defaults missed?
	if (nPushedExplicit + nExplicit < pDefParams->nRequired)
	{
		nRetVal = callBadParamCount;
	}
	else if (nPushedExplicit < pDefParams->nParams)
	{
		int nIndex;
		for (nIndex = nPushedExplicit; nIndex < pDefParams->nParams; nIndex++)
		{
#ifndef _SHADOW_DOUBLES
			pStack = StoreRawStackParameter(pStack,
				pbParams[nIndex],
				&(ppszValues[nIndex*sizeof(double)]));
#else
			pStack = StoreRawStackParameter(pStack,
				pbParams[nIndex],
				&(ppszValues[nIndex*sizeof(double)]), 0);
#endif
		}
	}

CleanUp:
	if (pFlags != NULL)
		delete [] pFlags;
	if (ppszValues != NULL)
		delete [] ppszValues;
	return nRetVal;
}


// push arguments on stack appropriate for C++ call (compiler dependent)
#ifndef _SHADOW_DOUBLES
int CHttpServer::PushStackArgs(BYTE* pStack, CHttpServerContext* pCtxt,
	const BYTE* pbParams, LPTSTR pszParams)
#else
int CHttpServer::PushStackArgs(BYTE* pStack, CHttpServerContext* pCtxt,
	const BYTE* pbParams, LPTSTR pszParams, UINT nSizeArgs)
#endif
{
	LPTSTR pszCurParam = NULL;
	ISAPIASSERT(pStack != NULL);
	ISAPIASSERT(pbParams != NULL);

	// C++ member functions use the __thiscall convention, where parameters
	//  are pushed last to first.  Assuming the stack grows down, this means
	//  that the first parameter is at the lowest memory address in the
	//  stack frame and the last parameter is at the highest address.

	// push the 'this' pointer
	*(_STACK_PTR*)pStack = (_STACK_PTR)this;
	pStack += sizeof(_STACK_PTR);

	// push our context pointer
	*(_STACK_PTR*)pStack = (_STACK_PTR)pCtxt;
	pStack += sizeof(_STACK_PTR);

	// push the arguments (first to last, low address to high address)
	TCHAR szTokens[2];
	szTokens[0] = m_cTokenDelimiter;
	szTokens[1] = '\0';
	const BYTE* pb;
	int nRetCode = callOK;

	int nLength = (pszParams != NULL) ? lstrlen(pszParams) : 0;

	// if IT_EMPTY, any arguments mean an error!
	if (nLength > 0 && *pbParams == IT_EMPTY)
		nRetCode = callBadParamCount;

	// otherwise, step through each argument
	else if (nLength > 0 || *pbParams == IT_RAW || *pbParams == IT_ARGLIST)
	{
		LPTSTR pszLineEnd;
		pszLineEnd = pszParams + lstrlen(pszParams);

		for (pb = pbParams; *pb != '\0'; ++pb)
		{
			if (*pb == IT_EMPTY)
			{
				// can't have ITS_EMPTY with other types!
				ISAPIASSERT(pb == pbParams);
				if (pb != pbParams)
					nRetCode = callBadParam;
				break;
			}

			if (*pb == IT_RAW || *pb == IT_ARGLIST)
			{
				// Can't have ITS_RAW or ITS_ARGLIST with other types!
				// (But if they're alone, we have special work to do...)

				BYTE* pbParam;
				ISAPIASSERT(pb == pbParams);
				if (pb != pbParams)
					nRetCode = callBadParam;
				else if (*pb == IT_RAW)
				{
					pbParam = (BYTE*) &pszParams;
#ifndef _SHADOW_DOUBLES
					pStack = StoreRawStackParameter(pStack, IT_PSTR, pbParam);
					pStack = StoreRawStackParameter(pStack, IT_I4,
						(BYTE*) &(pCtxt->m_dwBytesReceived));
#else
					pStack = StoreRawStackParameter(pStack, IT_PSTR, pbParam, 0);
					pStack = StoreRawStackParameter(pStack, IT_I4,
						(BYTE*) &(pCtxt->m_dwBytesReceived), 0);
#endif
				}
				else if (*pb == IT_ARGLIST)
				{
					pCtxt->m_pArgList = new CHttpArgList;
					if( pCtxt->m_pArgList == NULL )
						return callNoMemory;

					if( !pCtxt->m_pArgList->BuildArgList(pszParams, m_cTokenDelimiter) )
						return callNoMemory;
					pbParam = (BYTE*) &(pCtxt->m_pArgList);
#ifndef _SHADOW_DOUBLES
					pStack = StoreRawStackParameter(pStack, IT_ARGLIST, pbParam);
#else
					pStack = StoreRawStackParameter(pStack, IT_ARGLIST, pbParam, 0);
#endif
				}
				else
					ISAPIASSERT(FALSE);

				return nRetCode;
			} 

			if (pszParams == NULL)
				break;
			pszCurParam = _tcstok(pszParams, szTokens);
			if (pszCurParam == NULL)
				break;

			pszParams = pszCurParam + lstrlen(pszCurParam);
			if (pszParams != pszLineEnd)
				pszParams++;

#ifndef _SHADOW_DOUBLES
			pStack = StoreStackParameter(pStack, *pb, pszCurParam);
#else
			pStack = StoreStackParameter(pStack, *pb, pszCurParam, nSizeArgs, TRUE);
#endif
		}

		// check that all source arguments were consumed

		if (nRetCode == callOK)
		{
			LPTSTR pszMoreParams;
			pszMoreParams = _tcschr(pszParams, m_cTokenDelimiter);

			if (*pb != '\0' && pszMoreParams == NULL)
				nRetCode = callBadParamCount;
			else if (*pb == '\0' && (pszMoreParams != NULL || *pszParams != '\0'))
				nRetCode = callBadParamCount;
		}
	}
	else
	{
		// if we have no parameters, we must be IT_EMPTY
		if (*pbParams != IT_EMPTY)
			nRetCode = callBadParamCount;
	}

	return nRetCode;
}

#ifndef _SHADOW_DOUBLES
BYTE* CHttpServer::StoreRawStackParameter(BYTE* pStack, BYTE nType,
	BYTE* pRawParam)
#else
BYTE* CHttpServer::StoreRawStackParameter(BYTE* pStack, BYTE nType,
	BYTE* pRawParam, int nSizeArgs)
#endif
{
	ISAPIASSERT(pStack != NULL);
	ISAPIASSERT(pRawParam != NULL);

#ifdef _SHADOW_DOUBLES
	double* pDoubleShadow = (double*)(pStack + nSizeArgs);
	double* pDoubleShadowMax = pDoubleShadow + _SHADOW_DOUBLES;
#endif

	// push parameter value on the stack
	switch (nType)
	{
	// by value parameters
	case IT_I8:
		*(_STACK_LONGLONG*)pStack = *((__int64*) pRawParam);
		pStack += sizeof(_STACK_LONGLONG);
		break;
	case IT_I2:
		*(_STACK_SHORT*)pStack = *((WORD*) pRawParam);
		pStack += sizeof(_STACK_SHORT);   // 'short' is passed as 'int'
		break;
	case IT_I4:
		*(_STACK_LONG*)pStack = *((DWORD*) pRawParam);
		pStack += sizeof(_STACK_LONG);
		break;
	case IT_R4:
		*(_STACK_FLOAT*)pStack = *((_STACK_FLOAT*) pRawParam);
		pStack += sizeof(_STACK_FLOAT);
#ifdef _SHADOW_DOUBLES
		if (pDoubleShadow < pDoubleShadowMax)
			*pDoubleShadow++ = *((_STACK_DOUBLE*) pRawParam);
#endif
		break;
	case IT_R8:
#ifdef _ALIGN_DOUBLES
		// align doubles on 8 byte for some platforms
		pStack = (BYTE*)(((DWORD_PTR)pStack + _ALIGN_DOUBLES-1) &
			~(_ALIGN_DOUBLES-1));
#endif
		*(_STACK_DOUBLE*)pStack = *((_STACK_DOUBLE*) pRawParam);
		pStack += sizeof(_STACK_DOUBLE);
#ifdef _SHADOW_DOUBLES
		if (pDoubleShadow < pDoubleShadowMax)
			*pDoubleShadow++ = *((_STACK_DOUBLE*) pRawParam);
#endif
		break;
	case IT_ARGLIST:
	case IT_PSTR:
		*(_STACK_PTR*)pStack = *((_STACK_PTR*) pRawParam);
		pStack += sizeof(_STACK_PTR);
		break;
	default:
		ISAPIASSERT(FALSE);
	}

#ifdef _ALIGN_STACK
	// align stack as appropriate for next parameter
	pStack = (BYTE*)(((DWORD_PTR)pStack + (_ALIGN_STACK-1)) &
		~(_ALIGN_STACK-1));
	ISAPIASSERT(((DWORD_PTR)pStack & (_ALIGN_STACK-1)) == 0);
#endif

	return pStack;
}

#ifndef _SHADOW_DOUBLES
BYTE* CHttpServer::StoreStackParameter(BYTE* pStack, BYTE nType,
	LPTSTR pszCurParam)
#else
BYTE* CHttpServer::StoreStackParameter(BYTE* pStack, BYTE nType,
	LPTSTR pszCurParam, UINT nSizeArgs, BOOL bDoShadow)
#endif
{
	ISAPIASSERT(pStack != NULL);
	ISAPIASSERT(pszCurParam != NULL);

#ifdef _SHADOW_DOUBLES
	double* pDoubleShadow = (double*)(pStack + nSizeArgs);
	double* pDoubleShadowMax = pDoubleShadow + _SHADOW_DOUBLES;
#endif

	// push parameter value on the stack
	switch (nType)
	{
	// by value parameters
	case IT_I8:
		*(_STACK_LONGLONG*)pStack = (__int64) _ttoi64(pszCurParam);
		pStack += sizeof(_STACK_LONGLONG);   // 'short' is passed as 'int'
		break;
	case IT_I2:
		*(_STACK_SHORT*)pStack = (WORD) _ttoi(pszCurParam);
		pStack += sizeof(_STACK_SHORT);   // 'short' is passed as 'int'
		break;
	case IT_I4:
		*(_STACK_LONG*)pStack = (DWORD) _ttol(pszCurParam);
		pStack += sizeof(_STACK_LONG);
		break;
	case IT_R4:
		*(_STACK_FLOAT*)pStack = (_STACK_FLOAT) atof(pszCurParam);
		pStack += sizeof(_STACK_FLOAT);
#ifdef _SHADOW_DOUBLES
		if (bDoShadow && pDoubleShadow < pDoubleShadowMax)
			*pDoubleShadow++ = (double) atof(pszCurParam);
#endif
		break;
	case IT_R8:
#ifdef _ALIGN_DOUBLES
		// align doubles on 8 byte for some platforms
		pStack = (BYTE*)(((DWORD_PTR)pStack + _ALIGN_DOUBLES-1) &
			~(_ALIGN_DOUBLES-1));
#endif
		*(_STACK_DOUBLE*)pStack = (_STACK_DOUBLE) atof(pszCurParam);
		pStack += sizeof(_STACK_DOUBLE);
#ifdef _SHADOW_DOUBLES
		if (bDoShadow && pDoubleShadow < pDoubleShadowMax)
			*pDoubleShadow++ = atof(pszCurParam);
#endif
		break;
	case IT_PSTR:
		*(_STACK_PTR*)pStack = (_STACK_PTR) PreprocessString(pszCurParam);
		pStack += sizeof(_STACK_PTR);
		break;
	default:
		ISAPIASSERT(FALSE);
	}

#ifdef _ALIGN_STACK
	// align stack as appropriate for next parameter
	pStack = (BYTE*)(((DWORD_PTR)pStack + (_ALIGN_STACK-1)) &
		~(_ALIGN_STACK-1));
	ISAPIASSERT(((DWORD_PTR)pStack & (_ALIGN_STACK-1)) == 0);
#endif

	return pStack;
}

LPVOID CHttpServer::PreprocessString(LPTSTR psz)
{
	CHttpArgList::Unescape(psz);
	return (LPVOID) psz;
}

void CHttpArgList::Unescape(LPTSTR psz)
{
	LPTSTR pszSource = psz;
	LPTSTR pszDest = psz;

	static const TCHAR szHex[] = _T("0123456789ABCDEF");

	// Unescape special characters. The number of characters
	// in the *pszDest is assumed to be <= the number of characters
	// in pszSource (they are both the same string anyway)

	while (*pszSource != '\0' && *pszDest != '\0')
	{
		if (*pszSource == '+')
			*pszDest++ = ' ';
		else if (*pszSource == '%')
		{
			// The next two chars following '%' should be digits
			if ( *(pszSource + 1) == '\0' ||
				 *(pszSource + 2) == '\0' )
			{
				// nothing left to do
				break;
			}

			TCHAR nValue = '?';
			LPCTSTR pszLow = NULL;
			LPCTSTR pszHigh = NULL;
			pszSource++;

			*pszSource = (TCHAR) _totupper(*pszSource);
			pszHigh = _tcschr(szHex, *pszSource);

			if (pszHigh != NULL)
			{
				pszSource++;
				*pszSource = (TCHAR) _totupper(*pszSource);
				pszLow = _tcschr(szHex, *pszSource);

				if (pszLow != NULL)
				{
					nValue = (TCHAR) (((pszHigh - szHex) << 4) +
									(pszLow - szHex));
				}
			}
			*pszDest++ = nValue;
		}
		else
			*pszDest++ = *pszSource;
			
		pszSource++;
	}

	*pszDest = '\0';
}

///////////////////////////////////////////////////////////////////////
// argument list manager class

CHttpArgList::~CHttpArgList()
{
	CHttpArg* pArg = m_pFirstArg;

	while (pArg != NULL)
	{
		CHttpArg* pTemp = pArg;
		pArg = pArg->m_pNext;

		delete pTemp;
	}
}

CHttpArg::CHttpArg(CHttpArg* pPrevious, LPCTSTR pstrRaw /* = NULL */)
{
	m_pstrValue = NULL;
	m_pNext = NULL;

	// if we have some data, parse it up
	if (pstrRaw != NULL)
	{
		// m_pstrRaw will point at the raw data, period.
		m_pstrRaw = _tcsdup(pstrRaw);

		// m_pstrParam will point at a duplicate of the raw data.
		// But that duplicated string will have a '\0' inserted in place
		// of the equals sign so we know where the parameter ends.
		// We can still free m_pstrParam--the location of the memory
		// hasn't been modified tho the placement has.

		m_pstrParam = _tcsdup(pstrRaw);

		LPTSTR pstrSource;
		for (pstrSource = (LPTSTR)m_pstrParam; *pstrSource != 0; pstrSource++)
		{
			if (*pstrSource == '=')
			{
				*pstrSource++ = '\0';
				m_pstrValue = pstrSource;
				break;
			}
		}
	}
	else
		m_pstrRaw = NULL;

	if (pPrevious != NULL)
		pPrevious->m_pNext = this;
}

bool CHttpArgList::BuildArgList(LPTSTR psz, TCHAR cDelimiter)
{
	// walk the list, tokenizing the strings
	LPTSTR pstrSource;
	LPCTSTR pstrRaw;
	CHttpArg* pPrevious = NULL;
	CHttpArg* pCurrent = m_pFirstArg;
	CHttpArg* pOriginalFirst = m_pFirstArg;

	for (pstrRaw = pstrSource = psz; *pstrSource != 0; pstrSource++)
	{
		// when we find a delimeter, we know to create another argument object
		if (*pstrSource == cDelimiter)
		{
			*pstrSource = '\0';
			if (pCurrent == NULL)
			{
				pCurrent = new CHttpArg(NULL, pstrRaw);
				if( pCurrent == NULL )
					goto CleanUp;
				m_pFirstArg = pCurrent;
			}
			else
			{
				pPrevious = pCurrent;
				pCurrent = new CHttpArg(pPrevious, pstrRaw);
				if( pCurrent == NULL )
					goto CleanUp;
			}

			pstrRaw = pstrSource + 1;
		}
	}

	// at the end, we need to add the current string to the tail of the list
	// (if there were any arguments in the first place)
	if (*psz != '\0')
	{
		// add the last one
		pCurrent = new CHttpArg(pCurrent, pstrRaw);
		if( pCurrent == NULL )
			goto CleanUp;
		if (m_pFirstArg == NULL)
			m_pFirstArg = pCurrent;
	}

	return true;

CleanUp:
	if( pOriginalFirst == NULL )
	{
		pCurrent = m_pFirstArg;
		m_pFirstArg = NULL;
	}
	else
	{
		pCurrent = pOriginalFirst->m_pNext;
		pOriginalFirst->m_pNext = NULL;
	}
		
	while (pCurrent != NULL)
	{
		CHttpArg* pTemp = pCurrent;
		pCurrent = pCurrent->m_pNext;
		delete pTemp;
	}
	return false;
}


///////////////////////////////////////////////////////////////////////
// in-memory HTML Stream

CHtmlStream::CHtmlStream(UINT nGrowBytes /* = 4096 */)
{
	ISAPIASSERT(nGrowBytes <= UINT_MAX && nGrowBytes >= 0);

	m_nGrowBytes = nGrowBytes;
	m_nPosition = 0;
	m_nBufferSize = 0;
	m_lpBuffer = NULL;
	m_bAutoDelete = TRUE;
	m_nStreamSize = 0;
}

CHtmlStream::CHtmlStream(BYTE* lpBuffer, UINT nBufferSize,
	UINT nGrowBytes /* = 0 */)
{
	ISAPIASSERT(nGrowBytes <= UINT_MAX && nGrowBytes >= 0);

	m_nGrowBytes = nGrowBytes;
	m_nPosition = 0;
	m_nBufferSize = nBufferSize;

	// if the caller provides a grow-by size, use it;
	// otherwise, incrememt by the initial buffer size
	m_nStreamSize = nGrowBytes == 0 ? nBufferSize : 0;

	m_lpBuffer = lpBuffer;
	m_bAutoDelete = FALSE;
}

void CHtmlStream::Attach(BYTE* lpBuffer, UINT nBufferSize, UINT nGrowBytes)
{
	ISAPIASSERT(m_lpBuffer == NULL);

	m_nGrowBytes = nGrowBytes;
	m_nPosition = 0;
	m_nBufferSize = nBufferSize;

	// if the caller provides a grow-by size, use it;
	// otherwise, incrememt by the initial buffer size
	m_nStreamSize = nGrowBytes == 0 ? nBufferSize : 0;

	m_lpBuffer = lpBuffer;
	m_bAutoDelete = FALSE;
}

BYTE* CHtmlStream::Detach()
{
	BYTE* lpBuffer = NULL;
	if (m_lpBuffer != NULL)
	{
		TCHAR chZero = 0;
		Write(&chZero, sizeof(TCHAR));
		lpBuffer = m_lpBuffer;
		m_lpBuffer = NULL;
	}
	m_nBufferSize = 0;
	m_nStreamSize = 0;
	m_nPosition = 0;

	return lpBuffer;
}

CHtmlStream::~CHtmlStream()
{
	// Close should have already been called, but we check anyway
	if (m_lpBuffer)
		Close();
	ISAPIASSERT(NULL == m_lpBuffer);

	m_nGrowBytes = 0;
	m_nPosition = 0;
	m_nStreamSize = 0;
	m_nBufferSize = 0;
}

BYTE* CHtmlStream::Alloc(DWORD nBytes)
{
	return (BYTE*)malloc((UINT)nBytes);
}

BYTE* CHtmlStream::Realloc(BYTE* lpMem, DWORD nBytes)
{
	return (BYTE*)realloc(lpMem, (UINT)nBytes);
}

#pragma intrinsic(memcpy)
BYTE* CHtmlStream::Memcpy(BYTE* lpMemTarget, const BYTE* lpMemSource,
	UINT nBytes)
{
	ISAPIASSERT(lpMemTarget != NULL);
	ISAPIASSERT(lpMemSource != NULL);

	return (BYTE*)memcpy(lpMemTarget, lpMemSource, nBytes);
}
#pragma function(memcpy)

void CHtmlStream::Free(BYTE* lpMem)
{
	ISAPIASSERT(lpMem != NULL);
	free(lpMem);
}

void CHtmlStream::GrowStream(DWORD dwNewLen)
{
	if (dwNewLen > m_nBufferSize)
	{
		// grow the buffer
		DWORD dwNewBufferSize = (DWORD)m_nBufferSize;

		// watch out for buffers which cannot be grown!
		ISAPIASSERT(m_nGrowBytes > 0);
		if (m_nGrowBytes == 0)
			throw;

		// determine new buffer size
		while (dwNewBufferSize < dwNewLen)
			dwNewBufferSize += m_nGrowBytes;

		// allocate new buffer
		BYTE* lpNew;
		if (m_lpBuffer == NULL)
			lpNew = Alloc(dwNewBufferSize);
		else
			lpNew = Realloc(m_lpBuffer, dwNewBufferSize);

		if (lpNew == NULL)
			throw;

		m_lpBuffer = lpNew;
		m_nBufferSize = dwNewBufferSize;
	}
}

CHtmlStream& CHtmlStream::operator<<(LPCTSTR psz)
{
	Write(psz, lstrlen(psz));
	return *this;
}

CHtmlStream& CHtmlStream::operator<<(short int w)
{
	TCHAR sz[16];
	int nLen = wsprintf(sz, szDecimalFormat, w);
	Write(sz, nLen);
	return *this;
}

CHtmlStream& CHtmlStream::operator<<(long int dw)
{
	TCHAR sz[32];
	int nLen = wsprintf(sz, szDecimalFormat, dw);
	Write(sz, nLen);
	return *this;
}

CHtmlStream& CHtmlStream::operator<<(float f)
{
	TCHAR sz[512];
	int nLen = _stprintf(sz, szFloatFormat, f);
	Write(sz, nLen);
	return *this;
}

CHtmlStream& CHtmlStream::operator<<(double d)
{
	TCHAR sz[512];
	int nLen = _stprintf(sz, szFloatFormat, d);
	Write(sz, nLen);
	return *this;
}

CHtmlStream& CHtmlStream::operator<<(const CHtmlStream& stream)
{
	DWORD dwSourceLen = stream.GetStreamSize();
	Write(stream.m_lpBuffer, dwSourceLen);
	return *this;
}

void CHtmlStream::Close()
{
	m_nGrowBytes = 0;
	m_nPosition = 0;
	m_nBufferSize = 0;
	m_nStreamSize = 0;
	if (m_lpBuffer && m_bAutoDelete)
		Free(m_lpBuffer);
	m_lpBuffer = NULL;
}

void CHtmlStream::Abort()
{
	Close();
}

void CHtmlStream::Write(const void* lpBuf, UINT nCount)
{
	if (nCount == 0)
		return;
	ISAPIASSERT(lpBuf != NULL);

	if (m_nPosition + nCount > m_nBufferSize)
		GrowStream(m_nPosition + nCount);

	ISAPIASSERT(m_nPosition + nCount <= m_nBufferSize);

	Memcpy((BYTE*)m_lpBuffer + m_nPosition, (BYTE*)lpBuf, nCount);

	m_nPosition += nCount;

	if (m_nPosition > m_nStreamSize)
		m_nStreamSize = m_nPosition;
}

void CHtmlStream::Reset()
{
	m_nPosition = 0;
	m_nStreamSize = 0;
}

void CHtmlStream::InitStream()
{
	// subclass can override for interesting applications
}

///////////////////////////////////////////////////////////////////////
// HTTP Filter entry points

extern "C" DWORD WINAPI HttpFilterProc(PHTTP_FILTER_CONTEXT pfc,
	DWORD dwNotificationType, LPVOID pvNotification)
{
#ifdef _AFXDLL
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

	DWORD dwRet;

	ISAPIASSERT(pFilter != NULL);
	if (pFilter == NULL)
		dwRet = SF_STATUS_REQ_NEXT_NOTIFICATION;
	else
		dwRet = pFilter->HttpFilterProc(pfc,
			dwNotificationType, pvNotification);

	return dwRet;
}

extern "C" BOOL WINAPI GetFilterVersion(PHTTP_FILTER_VERSION pVer)
{
#ifdef _AFXDLL
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

	BOOL bRet;

	ISAPIASSERT(pFilter != NULL);
	if (pFilter == NULL)
		bRet = FALSE;
	else
		bRet = pFilter->GetFilterVersion(pVer);

	return bRet;
}


///////////////////////////////////////////////////////////////////////
// CHttpFilter implementation

CHttpFilter::CHttpFilter()
{
	ISAPIASSERT(pFilter == NULL);
	pFilter = this;
}

CHttpFilter::~CHttpFilter()
{
	pFilter = NULL;
}

BOOL CHttpFilter::GetFilterVersion(PHTTP_FILTER_VERSION pVer)
{
	ISAPIASSERT(pVer != NULL);
	pVer->dwFlags = SF_NOTIFY_ORDER_DEFAULT;
	pVer->dwFilterVersion = HTTP_FILTER_REVISION;
	pVer->lpszFilterDesc[0] = '\0';
	return TRUE;
}

DWORD CHttpFilter::HttpFilterProc(PHTTP_FILTER_CONTEXT pfc,
	DWORD dwNotificationType, LPVOID pvNotification)
{
	DWORD dwRet = SF_STATUS_REQ_NEXT_NOTIFICATION;
	CHttpFilterContext callCtxt(pfc);

	switch (dwNotificationType)
	{
	case SF_NOTIFY_AUTH_COMPLETE:
		dwRet = OnAuthComplete(&callCtxt,
			(PHTTP_FILTER_AUTH_COMPLETE_INFO) pvNotification);
		break;

	case SF_NOTIFY_READ_RAW_DATA:
		dwRet = OnReadRawData(&callCtxt, (PHTTP_FILTER_RAW_DATA) pvNotification);
		break;

	case SF_NOTIFY_PREPROC_HEADERS:
		dwRet = OnPreprocHeaders(&callCtxt,
			(PHTTP_FILTER_PREPROC_HEADERS) pvNotification);
		break;

	case SF_NOTIFY_AUTHENTICATION:
		dwRet = OnAuthentication(&callCtxt,
			(PHTTP_FILTER_AUTHENT) pvNotification);
		break;

	case SF_NOTIFY_URL_MAP:
		dwRet = OnUrlMap(&callCtxt, (PHTTP_FILTER_URL_MAP) pvNotification);
		break;

	case SF_NOTIFY_SEND_RAW_DATA:
		dwRet = OnSendRawData(&callCtxt, (PHTTP_FILTER_RAW_DATA) pvNotification);
		break;

	case SF_NOTIFY_LOG:
		dwRet = OnLog(&callCtxt, (PHTTP_FILTER_LOG) pvNotification);
		break;

	case SF_NOTIFY_END_OF_NET_SESSION:
		dwRet = OnEndOfNetSession(&callCtxt);
		break;

	case SF_NOTIFY_END_OF_REQUEST:
		dwRet = OnEndOfRequest(&callCtxt);
		break;

	case SF_NOTIFY_SEND_RESPONSE:
		dwRet = OnSendResponse(&callCtxt, (PHTTP_FILTER_SEND_RESPONSE) pvNotification);
		break;

	case SF_NOTIFY_ACCESS_DENIED:
		dwRet = OnAccessDenied(&callCtxt, (PHTTP_FILTER_ACCESS_DENIED) pvNotification);


	default:
		ISAPITRACE1("Warning: unrecognized HTTP filter notification code %d\n", dwNotificationType);
		break;
	}

	return dwRet;
}

// The user will override these.  Here, the functions have no
// formal parameters to avoid warnings.


DWORD CHttpFilter::OnAuthComplete(CHttpFilterContext *, PHTTP_FILTER_AUTH_COMPLETE_INFO)
{
	return SF_STATUS_REQ_NEXT_NOTIFICATION;
}

DWORD CHttpFilter::OnReadRawData(CHttpFilterContext*, PHTTP_FILTER_RAW_DATA)
{
	return SF_STATUS_REQ_NEXT_NOTIFICATION;
}

DWORD CHttpFilter::OnPreprocHeaders(CHttpFilterContext*, PHTTP_FILTER_PREPROC_HEADERS)
{
	return SF_STATUS_REQ_NEXT_NOTIFICATION;
}

DWORD CHttpFilter::OnAuthentication(CHttpFilterContext*, PHTTP_FILTER_AUTHENT)
{
	return SF_STATUS_REQ_NEXT_NOTIFICATION;
}

DWORD CHttpFilter::OnUrlMap(CHttpFilterContext*, PHTTP_FILTER_URL_MAP)
{
	return SF_STATUS_REQ_NEXT_NOTIFICATION;
}

DWORD CHttpFilter::OnSendRawData(CHttpFilterContext*, PHTTP_FILTER_RAW_DATA)
{
	return SF_STATUS_REQ_NEXT_NOTIFICATION;
}

DWORD CHttpFilter::OnLog(CHttpFilterContext*, PHTTP_FILTER_LOG)
{
	return SF_STATUS_REQ_NEXT_NOTIFICATION;
}

DWORD CHttpFilter::OnEndOfNetSession(CHttpFilterContext*)
{
	return SF_STATUS_REQ_NEXT_NOTIFICATION;
}

DWORD CHttpFilter::OnEndOfRequest(CHttpFilterContext*)
{
	return SF_STATUS_REQ_NEXT_NOTIFICATION;
}

DWORD CHttpFilter::OnSendResponse(CHttpFilterContext*, PHTTP_FILTER_SEND_RESPONSE)
{
	return SF_STATUS_REQ_NEXT_NOTIFICATION;
}

DWORD CHttpFilter::OnAccessDenied(CHttpFilterContext*, PHTTP_FILTER_ACCESS_DENIED)
{
	return SF_STATUS_REQ_NEXT_NOTIFICATION;
}


///////////////////////////////////////////////////////////////////////
// tracing helper function for linking without MFC

#ifndef _AFX


void AFXISAPI_CDECL AfxISAPITrace(LPCTSTR lpszFormat, ...)
{
#ifdef _DEBUG
	va_list args;
	va_start(args, lpszFormat);

	// if the trace has been set to go to a window and the user
	// presses RETRY, we will break to the debugger here

	if (_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, lpszFormat, args) == 1)
		_CrtDbgBreak();

	va_end(args);
#else
	lpszFormat;
#endif
}

#endif

///////////////////////////////////////////////////////////////////////
// handle inline functions

#ifndef _AFX_ENABLE_INLINES

#define _AFXISAPI_INLINE
#include "afxisapi.inl"

#endif //!_AFX_ENABLE_INLINES
