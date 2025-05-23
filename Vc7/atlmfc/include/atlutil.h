// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLUTIL_H__
#define __ATLUTIL_H__

#pragma once
#include <stdio.h>
#include <string.h>
#include <crtdbg.h>
#include <stdlib.h>
#include <mbstring.h>
#include <atldef.h>
#include <imagehlp.h>
#include <atlbase.h>
#include <atlstr.h>
#include <atlcoll.h>
#include <atlsiface.h>
#include <atlenc.h>
#include <atlcom.h>
#include <atlpath.h>

#ifndef _ATL_NO_DEFAULT_LIBS

#pragma comment(lib, "dbghelp.lib")

#endif  // !_ATL_NO_DEFAULT_LIBS

#pragma warning( push )
#pragma warning( disable: 4127 )
#pragma warning(disable: 4625) // copy constructor could not be generated because a base class copy constructor is inaccessible
#pragma warning(disable: 4626) // assignment operator could not be generated because a base class assignment operator is inaccessible
#pragma warning(disable: 4061) // enumerate 'enum value' in switch of enum 'enum type' is not explicitly handled by a case label

namespace ATL {


inline BOOL IsFullPath(LPCTSTR szPath) throw()
{
	size_t nLen = _tcslen(szPath);
	if (nLen <= 1)
		return FALSE;
	if (*szPath == _T('"'))
	{
		szPath++;
	}
	if (szPath[1]==_T(':'))		// drive: case
		return TRUE;
	if (nLen > 2 && szPath[0]==_T('\\') &&
		szPath[1]==_T('\\'))	// unc path name
		return TRUE;
	return FALSE;
}

inline BOOL IsFullPathA(LPCSTR szPath) throw()
{
	DWORD nLen = (DWORD) strlen(szPath);
	if (nLen <= 1)
		return FALSE;
	if (*szPath == '"')
	{
		szPath++;
	}
	if (szPath[1]==':')		// drive: case
		return TRUE;
	if (nLen > 2 && szPath[0]=='\\' &&
		szPath[1]=='\\')	// unc path name
		return TRUE;
	return FALSE;
}

#pragma warning(push)
#pragma warning(disable : 4706)

// this function is different from the the CRT 
// strncpy in that it does not pad out the whole string with zeroes
inline char * __cdecl _strncpy(char * dest, const char * source, size_t count) throw()
{
	char *start = dest;

	while (count && (*dest++ = *source++))
	{
		count--;
	}

	return(start);
}
#pragma warning(pop)

inline bool _SafeStringCopy(char *szDest, const char *szSrc, size_t nLen) throw()
{
	// initialize for check below
	szDest[nLen-1] = '\0';

	_strncpy(szDest, szSrc, nLen);
	if ('\0' != szDest[nLen-1])
	{
		// string was too large
		szDest[nLen-1] = '\0';
		return false;
	}
	return true;
}

template <class T>
inline bool SafeStringCopy(T& Destination, const char* Source) throw()
{
	// Use cast to ensure that we only allow character arrays
	(static_cast<char[sizeof(Destination)]>(Destination));

	// Copy up to the size of the buffer
	return _SafeStringCopy(Destination, Source, sizeof(Destination));
}

#if(_WIN32_WINNT >= 0x0400)
// Helper class for reverting the thread impersonation token
// and then restoring it back to what it was
class CRevertThreadToken
{
public:
	HANDLE m_hThreadToken;

	CRevertThreadToken() throw()
	{
		m_hThreadToken = INVALID_HANDLE_VALUE;
	}

	~CRevertThreadToken() throw()
	{
		// should have been Restore()'d by now.
#ifdef _DEBUG
		if(m_hThreadToken != INVALID_HANDLE_VALUE)
			DebugBreak();
#endif
	}

	// When called, this function
	// makes a copy of the thread's impersonation token
	// and then calls RevertToSelf() to revert the impersonation
	// level to the process
	// call Restore() to restore the impersonation
	// token
	BOOL Initialize() throw()
	{
		if (OpenThreadToken(GetCurrentThread(), TOKEN_IMPERSONATE |	TOKEN_DUPLICATE, FALSE, &m_hThreadToken))
		{
			if (!RevertToSelf())
			{
				CloseHandle(m_hThreadToken);
				m_hThreadToken = INVALID_HANDLE_VALUE;
				return FALSE;
			}
			return TRUE;
		}
		return FALSE;
	}

	DWORD Restore() throw()
	{
		DWORD dwError = NO_ERROR;
		if (m_hThreadToken != INVALID_HANDLE_VALUE)
		{
			if (!SetThreadToken(NULL, m_hThreadToken))
			{
				dwError = ::GetLastError();
			}
			CloseHandle(m_hThreadToken);
			m_hThreadToken = INVALID_HANDLE_VALUE;
		}
		return dwError;
	}
};
#else
// Dummy version for downlevel support
class CRevertThreadToken
{
public:
	BOOL Initialize() throw()
	{
		return FALSE;
	}
	DWORD Restore() throw()
	{
		return 0;
	}
};
#endif // _WIN32_WINNT >= 0x0400)

#ifndef ATL_ISAPI_BUFFER_SIZE
#define ATL_ISAPI_BUFFER_SIZE 4096
#endif

//typedefs and defines for CUrl (essentially the same as the ones from wininet, but with an ATL_ prepended)
typedef WORD ATL_URL_PORT;

enum ATL_URL_SCHEME 
{
	ATL_URL_SCHEME_UNKNOWN = -1, 
	ATL_URL_SCHEME_FTP     = 0,
	ATL_URL_SCHEME_GOPHER  = 1,
	ATL_URL_SCHEME_HTTP    = 2,
	ATL_URL_SCHEME_HTTPS   = 3,
	ATL_URL_SCHEME_FILE    = 4,
	ATL_URL_SCHEME_NEWS    = 5,
	ATL_URL_SCHEME_MAILTO  = 6,
	ATL_URL_SCHEME_SOCKS   = 7,
};


#define ATL_URL_MAX_HOST_NAME_LENGTH   256
#define ATL_URL_MAX_USER_NAME_LENGTH   128
#define ATL_URL_MAX_PASSWORD_LENGTH    128
#define ATL_URL_MAX_PORT_NUMBER_LENGTH 5           // ATL_URL_PORT is unsigned short
#define ATL_URL_MAX_PORT_NUMBER_VALUE  65535       // maximum unsigned short value
#define ATL_URL_MAX_PATH_LENGTH        2048
#define ATL_URL_MAX_SCHEME_LENGTH      32          // longest protocol name length
#define ATL_URL_MAX_URL_LENGTH         (ATL_URL_MAX_SCHEME_LENGTH \
									   + sizeof("://") \
									   + ATL_URL_MAX_PATH_LENGTH)

#define ATL_URL_INVALID_PORT_NUMBER    0           // use the protocol-specific default

#define ATL_URL_DEFAULT_FTP_PORT       21          // default for FTP servers
#define ATL_URL_DEFAULT_GOPHER_PORT    70          //    "     "  gopher "
#define ATL_URL_DEFAULT_HTTP_PORT      80          //    "     "  HTTP   "
#define ATL_URL_DEFAULT_HTTPS_PORT     443         //    "     "  HTTPS  "
#define ATL_URL_DEFAULT_SOCKS_PORT     1080        // default for SOCKS firewall servers.


template <DWORD dwSizeT=ATL_ISAPI_BUFFER_SIZE>
class CAtlIsapiBuffer
{
protected:
	char m_szBuffer[dwSizeT];
	LPSTR m_pBuffer;
	DWORD m_dwLen;
	DWORD m_dwAlloc;
	HANDLE m_hProcHeap;

public:
	CAtlIsapiBuffer() throw()
	{
		if (dwSizeT > 0)
			m_szBuffer[0] = 0;

		m_pBuffer = m_szBuffer;
		m_dwLen = 0;
		m_dwAlloc = dwSizeT;
		m_hProcHeap = GetProcessHeap();
	}

	CAtlIsapiBuffer(LPCSTR sz)
	{
		m_pBuffer = m_szBuffer;
		m_dwLen = 0;
		m_dwAlloc = dwSizeT;
		m_hProcHeap = GetProcessHeap();

		if (!Append(sz))
			AtlThrow(E_OUTOFMEMORY);
	}

	~CAtlIsapiBuffer() throw()
	{
		Free();
	}

	BOOL Alloc(DWORD dwSize) throw()
	{
		if (m_dwAlloc >= dwSize)
		{
			return TRUE;
		}
		if (m_pBuffer != m_szBuffer)
		{
			HeapFree(m_hProcHeap, 0, m_pBuffer);
			m_dwLen = 0;
			m_dwAlloc = 0;
		}
		m_pBuffer = (LPSTR)HeapAlloc(m_hProcHeap, 0, dwSize);
		if (m_pBuffer)
		{
			m_dwAlloc = dwSize;
			return TRUE;
		}
		return FALSE;
	}

	BOOL ReAlloc(DWORD dwNewSize) throw()
	{
		if (dwNewSize <= m_dwAlloc)
			return TRUE;

		if (m_pBuffer == m_szBuffer)
		{
			BOOL bRet = Alloc(dwNewSize);
			if (bRet)
				memcpy(m_pBuffer, m_szBuffer, m_dwLen);
			return bRet;
		}

		LPSTR pvNew = (LPSTR )HeapReAlloc(m_hProcHeap, 0, m_pBuffer, dwNewSize);
		if (pvNew)
		{
			m_pBuffer = pvNew;
			m_dwAlloc = dwNewSize;
			return TRUE;
		}
		return FALSE;
	}

	void Free() throw()
	{
		if (m_pBuffer != m_szBuffer)
		{
			HeapFree(m_hProcHeap,0 , m_pBuffer);
			m_dwAlloc = dwSizeT;
			m_pBuffer = m_szBuffer;
		}
		Empty();
	}

	void Empty() throw()
	{
		if (m_pBuffer)
		{
			m_pBuffer[0]=0;
			m_dwLen  = 0;
		}
	}

	DWORD GetLength() throw()
	{
		return m_dwLen;
	}

	BOOL Append(LPCSTR sz, int nLen = -1) throw()
	{
		if (nLen == -1)
			nLen = (int) strlen(sz);

		if (m_dwLen + nLen + 1 > m_dwAlloc)
		{
			if (!ReAlloc(m_dwAlloc + (nLen+1 > ATL_ISAPI_BUFFER_SIZE ? nLen+1 : ATL_ISAPI_BUFFER_SIZE)))
				return FALSE;
		}
		memcpy(m_pBuffer + m_dwLen, sz, nLen);
		m_dwLen += nLen;
		m_pBuffer[m_dwLen]=0;
		return TRUE;
	}

	operator LPCSTR() throw()
	{
		return m_pBuffer;
	}

	CAtlIsapiBuffer& operator+=(LPCSTR sz)
	{
		if (!Append(sz))
			AtlThrow(E_OUTOFMEMORY);
		return *this;
	}
}; // class CAtlIsapiBuffer


__interface IStackDumpHandler
{
public:
	void __stdcall OnBegin();
	void __stdcall OnEntry(void *pvAddress, LPCSTR szModule, LPCSTR szSymbol);
	void __stdcall OnError(LPCSTR szError);
	void __stdcall OnEnd();
};

#define ATL_MODULE_NAME_LEN _MAX_PATH
#define ATL_SYMBOL_NAME_LEN 1024

// Helper class for generating a stack dump
// This is used internally by AtlDumpStack
class CStackDumper
{
public:
	struct _ATL_SYMBOL_INFO
	{
		ULONG_PTR dwAddress;
		ULONG_PTR dwOffset;
		CHAR	szModule[ATL_MODULE_NAME_LEN];
		CHAR	szSymbol[ATL_SYMBOL_NAME_LEN];
	};

	static LPVOID __stdcall FunctionTableAccess(HANDLE hProcess, ULONG_PTR dwPCAddress)
	{
#ifdef _WIN64
		return SymFunctionTableAccess(hProcess, dwPCAddress);
#else
		return SymFunctionTableAccess(hProcess, (ULONG)dwPCAddress);
#endif
	}

	static ULONG_PTR __stdcall GetModuleBase(HANDLE hProcess, ULONG_PTR dwReturnAddress)
	{
		IMAGEHLP_MODULE moduleInfo;

#ifdef _WIN64
		if (SymGetModuleInfo(hProcess, dwReturnAddress, &moduleInfo))
#else
		if (SymGetModuleInfo(hProcess, (ULONG)dwReturnAddress, &moduleInfo))
#endif
	   		return moduleInfo.BaseOfImage;
		else
		{
			MEMORY_BASIC_INFORMATION memoryBasicInfo;

			if (::VirtualQueryEx(hProcess, (LPVOID) dwReturnAddress,
				&memoryBasicInfo, sizeof(memoryBasicInfo)))
			{
				DWORD cch = 0;
				char szFile[MAX_PATH] = { 0 };

				cch = GetModuleFileNameA((HINSTANCE)memoryBasicInfo.AllocationBase,
											 szFile, MAX_PATH);

				// Ignore the return code since we can't do anything with it.
				SymLoadModule(hProcess,
					NULL, ((cch) ? szFile : NULL),
#ifdef _WIN64
					NULL, (DWORD_PTR) memoryBasicInfo.AllocationBase, 0);
#else
					NULL, (DWORD)(DWORD_PTR)memoryBasicInfo.AllocationBase, 0);
#endif
				return (DWORD_PTR) memoryBasicInfo.AllocationBase;
			}
		}

		return 0;
	}

	static BOOL ResolveSymbol(HANDLE hProcess, UINT_PTR dwAddress,
		_ATL_SYMBOL_INFO &siSymbol)
	{
		BOOL fRetval = TRUE;

		siSymbol.dwAddress = dwAddress;

		CHAR szUndec[ATL_SYMBOL_NAME_LEN];
		CHAR szWithOffset[ATL_SYMBOL_NAME_LEN];
		LPSTR pszSymbol = NULL;
		IMAGEHLP_MODULE mi;

		memset(&siSymbol, 0, sizeof(_ATL_SYMBOL_INFO));
		mi.SizeOfStruct = sizeof(IMAGEHLP_MODULE);

#ifdef _WIN64
		if (!SymGetModuleInfo(hProcess, dwAddress, &mi))
#else
		if (!SymGetModuleInfo(hProcess, (UINT)dwAddress, &mi))
#endif
	   		lstrcpyA(siSymbol.szModule, "<no module>");
		else
		{
			LPSTR pszModule = strchr(mi.ImageName, '\\');
			if (pszModule == NULL)
				pszModule = mi.ImageName;
			else
				pszModule++;

			lstrcpynA(siSymbol.szModule, pszModule, sizeof(siSymbol.szModule)/sizeof(siSymbol.szModule[0]));
		}

		__try
		{
			union 
			{
				CHAR rgchSymbol[sizeof(IMAGEHLP_SYMBOL) + ATL_SYMBOL_NAME_LEN];
				IMAGEHLP_SYMBOL  sym;
			} sym;
			memset(&sym.sym, 0x00, sizeof(sym.sym));
			sym.sym.SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
#ifdef _WIN64
			sym.sym.Address = dwAddress;
#else
			sym.sym.Address = (DWORD)dwAddress;
#endif
			sym.sym.MaxNameLength = ATL_SYMBOL_NAME_LEN;

#ifdef _WIN64
			if (SymGetSymFromAddr(hProcess, dwAddress, &(siSymbol.dwOffset), &sym.sym))
#else
			if (SymGetSymFromAddr(hProcess, (DWORD)dwAddress, &(siSymbol.dwOffset), &sym.sym))
#endif
			{
				pszSymbol = sym.sym.Name;

				if (UnDecorateSymbolName(sym.sym.Name, szUndec, sizeof(szUndec)/sizeof(szUndec[0]), 
					UNDNAME_NO_MS_KEYWORDS | UNDNAME_NO_ACCESS_SPECIFIERS))
				{
					pszSymbol = szUndec;
				}
				else if (SymUnDName(&sym.sym, szUndec, sizeof(szUndec)/sizeof(szUndec[0])))
				{
					pszSymbol = szUndec;
				}
				if (siSymbol.dwOffset != 0)
				{
					_snprintf(szWithOffset, ATL_SYMBOL_NAME_LEN, "%s + %d bytes", pszSymbol, siSymbol.dwOffset);

					// ensure null-terminated
					szWithOffset[ATL_SYMBOL_NAME_LEN-1] = '\0';

					pszSymbol = szWithOffset;
				}
		  }
		  else
			  pszSymbol = "<no symbol>";
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			pszSymbol = "<EX: no symbol>";
			siSymbol.dwOffset = dwAddress - mi.BaseOfImage;
		}

		lstrcpynA(siSymbol.szSymbol, pszSymbol, sizeof(siSymbol.szSymbol)/sizeof(siSymbol.szSymbol[0]));
		return fRetval;
	}
};

#ifndef _ATL_MAX_STACK_FRAMES
#define _ATL_MAX_STACK_FRAMES 256
#endif

// Helper function to produce a stack dump
ATL_NOINLINE inline void AtlDumpStack(IStackDumpHandler *pHandler)
{
	ATLASSERT(pHandler);

	pHandler->OnBegin();

	CAtlArray<void *> adwAddress;
	HANDLE hProcess = ::GetCurrentProcess();
	if (SymInitialize(hProcess, NULL, FALSE))
	{
		// force undecorated names to get params
		DWORD dw = SymGetOptions();
		dw &= ~SYMOPT_UNDNAME;
		SymSetOptions(dw);

		HANDLE hThread = ::GetCurrentThread();
		CONTEXT threadContext;

		threadContext.ContextFlags = CONTEXT_FULL;

		if (::GetThreadContext(hThread, &threadContext))
		{
			STACKFRAME stackFrame;
			memset(&stackFrame, 0, sizeof(stackFrame));
			stackFrame.AddrPC.Mode = AddrModeFlat;

			DWORD dwMachType;

#if defined(_M_IX86)
			dwMachType                  = IMAGE_FILE_MACHINE_I386;

			// program counter, stack pointer, and frame pointer
			stackFrame.AddrPC.Offset    = threadContext.Eip;
			stackFrame.AddrStack.Offset = threadContext.Esp;
			stackFrame.AddrStack.Mode   = AddrModeFlat;
			stackFrame.AddrFrame.Offset = threadContext.Ebp;
			stackFrame.AddrFrame.Mode   = AddrModeFlat;
#elif defined(_M_MRX000)
			// only program counter
			dwMachType                  = IMAGE_FILE_MACHINE_R4000;
			stackFrame.AddrPC. Offset    = threadContext.Fir;
#elif defined(_M_ALPHA)
			// only program counter
			dwMachType                  = IMAGE_FILE_MACHINE_ALPHA;
			stackFrame.AddrPC.Offset    = (unsigned long) threadContext.Fir;
#elif defined(_M_PPC)
			// only program counter
			dwMachType                  = IMAGE_FILE_MACHINE_POWERPC;
			stackFrame.AddrPC.Offset    = threadContext.Iar;
#elif defined(_M_IA64)
			// only program counter
			dwMachType                  = IMAGE_FILE_MACHINE_IA64;
			stackFrame.AddrPC.Offset =  threadContext.StIIP;

#elif defined(_M_ALPHA64)
		 // only program counter
		 dwMachType                  = IMAGE_FILE_MACHINE_ALPHA64;
		 stackFrame.AddrPC.Offset    = threadContext.Fir;
#else
#error("Unknown Target Machine");
#endif

			adwAddress.SetCount(0, 16);

			int nFrame;
			for (nFrame = 0; nFrame < _ATL_MAX_STACK_FRAMES; nFrame++)
			{
				if (!StackWalk(dwMachType, hProcess, hProcess,
					&stackFrame, &threadContext, NULL,
					CStackDumper::FunctionTableAccess, CStackDumper::GetModuleBase, NULL))
				{
					break;
				}
				adwAddress.SetAtGrow(nFrame, (void*)(DWORD_PTR)stackFrame.AddrPC.Offset);
			}
		}
	}
	else
	{
		DWORD dw = GetLastError();
		char sz[100];
		_snprintf(sz, 100, 
			"AtlDumpStack Error: IMAGEHLP.DLL wasn't found. "
			"GetLastError() returned 0x%8.8X\r\n", dw);

		// ensure null-terminated
		sz[sizeof(sz)-1] = '\0';

		pHandler->OnError(sz);
	}

	// dump it out now
	INT_PTR nAddress;
	INT_PTR cAddresses = adwAddress.GetCount();
	for (nAddress = 0; nAddress < cAddresses; nAddress++)
	{
		CStackDumper::_ATL_SYMBOL_INFO info;
		UINT_PTR dwAddress = (UINT_PTR)adwAddress[nAddress];

		LPCSTR szModule = NULL;
		LPCSTR szSymbol = NULL;

		if (CStackDumper::ResolveSymbol(hProcess, dwAddress, info))
		{
			szModule = info.szModule;
			szSymbol = info.szSymbol;
		}
		pHandler->OnEntry((void *) dwAddress, szModule, szSymbol);
	}
	pHandler->OnEnd();
}

#define STACK_TRACE_PART_DELIMITER ';'
#define STACK_TRACE_LINE_DELIMITER '~'

// CReportHookDumpHandler is a stack dump handler
// that gathers the stack dump into the format
// used by CDebugReportHook
class CReportHookDumpHandler : public IStackDumpHandler
{
public:
	CReportHookDumpHandler()
	{
		m_pstr = NULL;
	}

	void GetStackDump(CStringA *pstr)
	{
		ATLASSERT(pstr);
		SetString(pstr);
		AtlDumpStack(this);
		SetString(NULL);
	}

	void SetString(CStringA *pstr)
	{
		m_pstr = pstr;
	}

	// implementation
	// IStackDumpHandler methods
	void __stdcall OnBegin()
	{
	}

	void __stdcall OnEntry(void *pvAddress, LPCSTR szModule, LPCSTR szSymbol)
	{
		// make sure SetString was called before
		// trying to get a stack dump
		ATLASSERT(m_pstr);
		if (!m_pstr)
			return;

		char szBuf[100];
		_snprintf(szBuf, 100, "0x%p;", pvAddress);
		// ensure null-terminated
		szBuf[sizeof(szBuf)-1] = '\0';
		*m_pstr += szBuf;
		if (!szModule)
			szModule = "Unknown";
		if (!szSymbol)
			szSymbol = "<No Info>";

		*m_pstr += szModule;
		*m_pstr += STACK_TRACE_PART_DELIMITER;
		ATLASSERT(szSymbol);
		*m_pstr += szSymbol;
		*m_pstr += STACK_TRACE_PART_DELIMITER;
		*m_pstr += STACK_TRACE_LINE_DELIMITER;
	}

	void __stdcall OnError(LPCSTR /*szError*/)
	{
	}
	void __stdcall OnEnd()
	{
	}

protected:
	CStringA *m_pstr;

};

#define PIPE_INPUT_BUFFER_SIZE  4096
#define PIPE_OUTPUT_BUFFER_SIZE 2048

enum { DEBUG_SERVER_MESSAGE_TRACE, DEBUG_SERVER_MESSAGE_ASSERT, DEBUG_SERVER_MESSAGE_QUIT };

struct DEBUG_SERVER_MESSAGE
{
	DWORD dwType;		// one of DEBUG_SERVER_MESSAGE_*
	DWORD dwProcessId;	// process id of client
	DWORD dwClientNameLen;	// length of client name
	size_t dwTextLen;	// length of text message including null terminator
	BOOL bIsDebuggerAttached;	// TRUE if the debugger is already attached
};

#ifdef _DEBUG

extern "C" WINBASEAPI
BOOL
WINAPI
IsDebuggerPresent(
	VOID
	);

class CDebugReportHook
{
protected:

	static char m_szPipeName[MAX_PATH+1];
	static DWORD m_dwTimeout;
	static DWORD m_dwClientNameLen;
	static char m_szClientName[MAX_COMPUTERNAME_LENGTH+1];

public:
	CDebugReportHook(LPCSTR szMachineName = ".", LPCSTR szPipeName = "AtlsDbgPipe", DWORD dwTimeout = 20000) throw()
	{
		if (SetPipeName(szMachineName, szPipeName))
		{
			SetTimeout(dwTimeout);
			SetHook();
		}
		m_dwClientNameLen = sizeof(m_szClientName);
		GetComputerNameA(m_szClientName, &m_dwClientNameLen);
	}

	~CDebugReportHook() throw()
	{
		RemoveHook();
	}

	BOOL SetPipeName(LPCSTR szMachineName = ".", LPCSTR szPipeName = "AtlsDbgPipe") throw()
	{
		size_t nLen1 = strlen(szMachineName);
		size_t nLen2 = strlen(szPipeName);
		if (nLen1 + nLen2 + 8 < MAX_PATH)
		{
			_snprintf(m_szPipeName, MAX_PATH, "\\\\%s\\pipe\\%s", szMachineName, szPipeName);
			return TRUE;
		}
		return FALSE;
	}

	void SetTimeout(DWORD dwTimeout)
	{
		m_dwTimeout = dwTimeout;
	}

	void SetHook() throw()
	{
#ifdef _DEBUG
		ATLASSERT( _CrtSetReportHook2(_CRT_RPTHOOK_INSTALL, CDebugReportHookProc) != -1 );
#else
		_CrtSetReportHook2(_CRT_RPTHOOK_INSTALL, CDebugReportHookProc);
#endif
	}

	void RemoveHook() throw()
	{
#ifdef _DEBUG
		ATLASSERT( _CrtSetReportHook2(_CRT_RPTHOOK_REMOVE, CDebugReportHookProc) != -1 );
#else
		_CrtSetReportHook2(_CRT_RPTHOOK_REMOVE, CDebugReportHookProc);
#endif
	}

	static int __cdecl CDebugReportHookProc(int reportType, char *message, int *returnValue) throw()
	{
		DWORD dwWritten;

		*returnValue = 0;

		CRevertThreadToken revert;
		if (!revert.Initialize())
			return FALSE;

		CHandle hdlPipe;
		while (1)
		{
			// we're opening an untrusted server. prevent the server from getting our privileges through impersonation
			HANDLE hPipe = CreateFileA(m_szPipeName, GENERIC_WRITE | GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, SECURITY_SQOS_PRESENT | SECURITY_IDENTIFICATION | SECURITY_EFFECTIVE_ONLY, NULL);

			if (hPipe != INVALID_HANDLE_VALUE )
			{
				hdlPipe.Attach(hPipe);
				break;
			}

			if (GetLastError() != ERROR_PIPE_BUSY)
			{
				if (revert.Restore())
				{
#ifdef _DEBUG
					DebugBreak();
#endif
				}
				if (reportType == _CRT_ASSERT)
					return TRUE;
				return FALSE;
			}

			//If the pipe is busy, we wait for up to m_dwTimeout
			if (!WaitNamedPipeA(m_szPipeName, m_dwTimeout)) 
			{
				if (revert.Restore())
				{
#ifdef _DEBUG
					DebugBreak();
#endif
				}
				if (reportType == _CRT_ASSERT)
					return TRUE;
				return FALSE;
			}
		}

		DEBUG_SERVER_MESSAGE Message;

		Message.bIsDebuggerAttached = IsDebuggerPresent();

		if (reportType == _CRT_ASSERT)
		{
			Message.dwType = DEBUG_SERVER_MESSAGE_ASSERT;
		}
		else
		{
			Message.dwType = DEBUG_SERVER_MESSAGE_TRACE;
		}

		Message.dwProcessId = GetCurrentProcessId();
		Message.dwClientNameLen = m_dwClientNameLen+1;	// add 1 for the null terminator
		Message.dwTextLen = strlen(message)+1;

		int nRet = 1;

		WriteFile(hdlPipe, &Message, sizeof(DEBUG_SERVER_MESSAGE), &dwWritten, NULL);

		WriteFile(hdlPipe, m_szClientName, Message.dwClientNameLen, &dwWritten, NULL);

		WriteFile(hdlPipe, message, (DWORD)Message.dwTextLen, &dwWritten, NULL);

		//Check to see whether or not to send stack trace
		BOOL bRet = ReadFile(hdlPipe, &nRet, sizeof(nRet), &dwWritten, NULL);

		//if nRet == 1, the user wants stack trace info
		if (bRet && nRet)
		{
			_ATLTRY
			{
				CStringA str;
				CReportHookDumpHandler stackDumper;
				stackDumper.GetStackDump(&str);
				if (!WriteFile(hdlPipe, (LPCSTR)str, str.GetLength(), &dwWritten, NULL))
				{
					if (revert.Restore())
					{
	#ifdef _DEBUG
						DebugBreak();
	#endif
					}
					return (reportType == _CRT_ASSERT ? TRUE : FALSE);
				}
			}
			_ATLCATCHALL()
			{
				if (revert.Restore())
				{
#ifdef _DEBUG
					DebugBreak();
#endif
				}
				return (reportType == _CRT_ASSERT ? TRUE : FALSE);
			}
		}

		if (bRet)
			bRet = ReadFile(hdlPipe, &nRet, sizeof(nRet), &dwWritten, NULL);
		if (!bRet)
			nRet = 0;

		if (revert.Restore())
		{
#ifdef _DEBUG
			DebugBreak();
#endif
		}

		// possible return values
		// 0 -> Ignore or cancel
		// 1 -> Retry
		// 2 -> Abort
		if (nRet == 0)
		{
			return (reportType == _CRT_ASSERT ? TRUE : FALSE);
		}
		if (nRet == 1)
		{
			if (IsDebuggerPresent())
			{
				DebugBreak();
			}
		}

		if (nRet == 2)
			abort();

		return (reportType == _CRT_ASSERT ? TRUE : FALSE);
	}
}; // class CDebugReportHook


__declspec(selectany) char CDebugReportHook::m_szPipeName[MAX_PATH+1];
__declspec(selectany) DWORD CDebugReportHook::m_dwTimeout;
__declspec(selectany) DWORD CDebugReportHook::m_dwClientNameLen;
__declspec(selectany) char CDebugReportHook::m_szClientName[MAX_COMPUTERNAME_LENGTH+1];
#endif

#ifndef ATL_POOL_NUM_THREADS
	#define ATL_POOL_NUM_THREADS 0
#endif

#ifndef ATL_POOL_STACK_SIZE
	#define ATL_POOL_STACK_SIZE 0
#endif

#ifndef ATLS_DEFAULT_THREADSPERPROC
	#define ATLS_DEFAULT_THREADSPERPROC 2
#endif

#ifndef ATLS_DEFAULT_THREADPOOLSHUTDOWNTIMEOUT
	#define ATLS_DEFAULT_THREADPOOLSHUTDOWNTIMEOUT 36000
#endif

//
// CThreadPool
// This class is a simple IO completion port based thread pool
//	Worker:
//		is a class that is responsible for handling requests
//		queued on the thread pool.
//		It must have a typedef for RequestType, where request type
//		is the datatype to be queued on the pool
//		RequestType must be castable to (DWORD)
//		The value -1 is reserved for shutdown
//		of the pool
//		Worker must also have a void Execute(RequestType request, void *pvParam, OVERLAPPED *pOverlapped) function
//	ThreadTraits:
//		is a class that implements a static CreateThread function
//		This allows for overriding how the threads are created
#define ATLS_POOL_SHUTDOWN ((OVERLAPPED*) ((__int64) -1))
template <class Worker, class ThreadTraits=DefaultThreadTraits>
class CThreadPool : public IThreadPoolConfig
{
protected:

	CSimpleMap<DWORD, HANDLE> m_threadMap;

	DWORD m_dwThreadEventId;

	CComCriticalSection m_critSec;
	DWORD m_dwStackSize;
	DWORD m_dwMaxWait;

	void *m_pvWorkerParam;
	LONG m_bShutdown;

	HANDLE m_hThreadEvent;
	HANDLE m_hRequestQueue;

public:

	CThreadPool() throw() :
		m_hRequestQueue(NULL),
		m_pvWorkerParam(NULL),
		m_dwMaxWait(ATLS_DEFAULT_THREADPOOLSHUTDOWNTIMEOUT),
		m_bShutdown(FALSE),
		m_dwThreadEventId(0),
		m_dwStackSize(0)
	{
	}

	~CThreadPool() throw()
	{
		Shutdown();
	}

	// Initialize the thread pool
	// if nNumThreads > 0, then it specifies the number of threads
	// if nNumThreads < 0, then it specifies the number of threads per proc (-)
	// if nNumThreads == 0, then it defaults to two threads per proc
	// hCompletion is a handle of a file to associate with the completion port
	// pvWorkerParam is a parameter that will be passed to Worker::Execute
	//	dwStackSize:
	//		The stack size to use when creating the threads
	HRESULT Initialize(void *pvWorkerParam=NULL, int nNumThreads=0, DWORD dwStackSize=0, HANDLE hCompletion=INVALID_HANDLE_VALUE) throw()
	{
		ATLASSERT( m_hRequestQueue == NULL );

		if (m_hRequestQueue)   // Already initialized
			return AtlHresultFromWin32(ERROR_ALREADY_INITIALIZED);

		if (S_OK != m_critSec.Init())
			return E_FAIL;

		m_hThreadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (!m_hThreadEvent)
		{
			m_critSec.Term();
			return AtlHresultFromLastError();
		}

		// Create IO completion port to queue the requests
		m_hRequestQueue = CreateIoCompletionPort(hCompletion, NULL, 0, nNumThreads);
		if (m_hRequestQueue == NULL)
		{
			// failed creating the Io completion port
			m_critSec.Term();
			CloseHandle(m_hThreadEvent);
			return AtlHresultFromLastError();		
		}
		m_pvWorkerParam = pvWorkerParam;
		m_dwStackSize = dwStackSize;

		HRESULT hr = SetSize(nNumThreads);
		if (hr != S_OK)
		{
			// Close the request queue handle
			CloseHandle(m_hRequestQueue);

			// Clear the queue handle
			m_hRequestQueue = NULL;

			// Uninitialize the critical sections
			m_critSec.Term();
			CloseHandle(m_hThreadEvent);


			return hr;
		}

		return S_OK;
	}

	// Shutdown the thread pool
	// This function posts the shutdown request to all the threads in the pool
	// It will wait for the threads to shutdown a maximum of dwMaxWait MS.
	// If the timeout expires it just returns without terminating the threads.
	void Shutdown(DWORD dwMaxWait=0) throw()
	{
		if (!m_hRequestQueue)   // Not initialized
			return;

		CComCritSecLock<CComCriticalSection> lock(m_critSec, false);
		if (FAILED(lock.Lock()))
		{
			// out of memory
			ATLASSERT( FALSE );
			return;
		}


		if (dwMaxWait == 0)
			dwMaxWait = m_dwMaxWait;

		HRESULT hr = InternalResizePool(0, dwMaxWait);

		if (hr != S_OK)
			ATLTRACE(atlTraceUtil, 0, _T("Thread pool not shutting down cleanly : %08x"), hr);
			// If the threads have not returned, then something is wrong

		for (int i = m_threadMap.GetSize() - 1; i >= 0; i--)
		{
			HANDLE hThread = m_threadMap.GetValueAt(i);
			DWORD dwExitCode;
			GetExitCodeThread(hThread, &dwExitCode);
			if (dwExitCode == STILL_ACTIVE)
			{
				ATLTRACE(atlTraceUtil, 0, _T("Terminating thread"));
				TerminateThread(hThread, 0);
			}
			CloseHandle(hThread);
		}

		// Close the request queue handle
		CloseHandle(m_hRequestQueue);

		// Clear the queue handle
		m_hRequestQueue = NULL;

		ATLASSERT(m_threadMap.GetSize() == 0);

		// Uninitialize the critical sections
		lock.Unlock();
		m_critSec.Term();
		CloseHandle(m_hThreadEvent);

	}

	// IThreadPoolConfig methods
	HRESULT STDMETHODCALLTYPE SetSize(int nNumThreads) throw()
	{
		if (nNumThreads == 0)
			nNumThreads = -ATLS_DEFAULT_THREADSPERPROC;

		if (nNumThreads < 0)
		{
			SYSTEM_INFO si;
			GetSystemInfo(&si);
			nNumThreads = (int) (-nNumThreads) * si.dwNumberOfProcessors;
		}

		return InternalResizePool(nNumThreads, m_dwMaxWait);
	}

	HRESULT STDMETHODCALLTYPE GetSize(int *pnNumThreads) throw()
	{
		if (!pnNumThreads)
			return E_POINTER;

		*pnNumThreads = GetNumThreads();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SetTimeout(DWORD dwMaxWait) throw()
	{
		m_dwMaxWait = dwMaxWait;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetTimeout(DWORD *pdwMaxWait) throw()
	{
		if (!pdwMaxWait)
			return E_POINTER;

		*pdwMaxWait = m_dwMaxWait;

		return S_OK;
	}

	// IUnknown methods
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppv) throw()
	{
		if (!ppv)
			return E_POINTER;

		*ppv = NULL;

		if (InlineIsEqualGUID(riid, __uuidof(IUnknown)) ||
			InlineIsEqualGUID(riid, __uuidof(IThreadPoolConfig)))
		{
			*ppv = static_cast<IThreadPoolConfig*>(this);
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}

	ULONG STDMETHODCALLTYPE AddRef() throw()
	{
		return 1;
	}

	ULONG STDMETHODCALLTYPE Release() throw()
	{
		return 1;
	}


	HANDLE GetQueueHandle() throw()
	{
		return m_hRequestQueue;
	}

	int GetNumThreads() throw()
	{
		return m_threadMap.GetSize();
	}

	// QueueRequest adds a request to the thread pool
	// it will be picked up by one of the threads and dispatched to the worker
	// in WorkerThreadProc
	BOOL QueueRequest(typename Worker::RequestType request) throw()
	{
		if (!PostQueuedCompletionStatus(m_hRequestQueue, 0, (ULONG_PTR) request, NULL))
			return FALSE;
		return TRUE;
	}


protected:

	DWORD ThreadProc() throw()
	{
		DWORD dwBytesTransfered;
		ULONG_PTR dwCompletionKey;

		OVERLAPPED* pOverlapped;

		// this block is to ensure theWorker gets destructed before the 
		// thread handle is closed
		{
			// We instantiate an instance of the worker class on the stack
			// for the life time of the thread.
			Worker theWorker;

			if (theWorker.Initialize(m_pvWorkerParam) == FALSE)
			{
				return 1;
			}

			SetEvent(m_hThreadEvent);
			// Get the request from the IO completion port
			while (GetQueuedCompletionStatus(m_hRequestQueue, &dwBytesTransfered, &dwCompletionKey, &pOverlapped, INFINITE))
			{
				if (pOverlapped == ATLS_POOL_SHUTDOWN) // Shut down
				{
					LONG bResult = InterlockedExchange(&m_bShutdown, FALSE);
					if (bResult) // Shutdown has not been cancelled
						break;

					// else, shutdown has been cancelled -- continue as before
				}
				else										// Do work
				{
					Worker::RequestType request = (Worker::RequestType) dwCompletionKey;

					// Process the request.  Notice the following:
					// (1) It is the worker's responsibility to free any memory associated
					// with the request if the request is complete
					// (2) If the request still requires some more processing
					// the worker should queue the request again for dispatching
					theWorker.Execute(request, m_pvWorkerParam, pOverlapped);
				}
			}

			theWorker.Terminate(m_pvWorkerParam);
		}

		m_dwThreadEventId = GetCurrentThreadId();
		SetEvent(m_hThreadEvent);

		return 0; 
	}

	static DWORD WINAPI WorkerThreadProc(LPVOID pv) throw()
	{
		CThreadPool* pThis = 
			reinterpret_cast< CThreadPool* >(pv); 

		return pThis->ThreadProc();
	} 

	HRESULT InternalResizePool(int nNumThreads, int dwMaxWait) throw()
	{
		if (!m_hRequestQueue)   // Not initialized
			return E_FAIL;

		CComCritSecLock<CComCriticalSection> lock(m_critSec, false);
		if (FAILED(lock.Lock()))
		{
			// out of memory
			ATLASSERT( FALSE );
			return E_FAIL;
		}

		int nCurThreads = m_threadMap.GetSize();
		if (nNumThreads == nCurThreads)
		{
			return S_OK;
		}
		else if (nNumThreads < nCurThreads)
		{
			int nNumShutdownThreads = nCurThreads - nNumThreads;
			for (int nThreadIndex = 0; nThreadIndex < nNumShutdownThreads; nThreadIndex++)
			{    
				ResetEvent(m_hThreadEvent);

				InterlockedExchange(&m_bShutdown, TRUE);
				PostQueuedCompletionStatus(m_hRequestQueue, 0, 0, ATLS_POOL_SHUTDOWN);
				DWORD dwRet = WaitForSingleObject(m_hThreadEvent, dwMaxWait);

				if (dwRet == WAIT_TIMEOUT)
				{
					LONG bResult = InterlockedExchange(&m_bShutdown, FALSE);
					if (bResult) // Nobody picked up the shutdown message
					{
						return AtlHresultFromWin32(WAIT_TIMEOUT);
					}
				}
				else if (dwRet != WAIT_OBJECT_0)
				{
					return AtlHresultFromLastError();
				}

				int nIndex = m_threadMap.FindKey(m_dwThreadEventId);
				if (nIndex != -1)
				{
					HANDLE hThread = m_threadMap.GetValueAt(nIndex);
					// Wait for the thread to shutdown
					if (WaitForSingleObject(hThread, 60000) == WAIT_OBJECT_0) 
					{
						CloseHandle(hThread);
						m_threadMap.RemoveAt(nIndex);
					}
					else
					{
						// Thread failed to terminate
						return E_FAIL;
					}
				}
			}
		}
		else
		{
			int nNumNewThreads = nNumThreads - nCurThreads;
			// Create and initialize worker threads

			for (int nThreadIndex = 0; nThreadIndex < nNumNewThreads; nThreadIndex++)
			{
				DWORD dwThreadID;
				ResetEvent(m_hThreadEvent);
				CHandle hdlThread( ThreadTraits::CreateThread(NULL, m_dwStackSize, WorkerThreadProc, (LPVOID)this, 0, &dwThreadID) );

				if (!hdlThread)
				{
					HRESULT hr = AtlHresultFromLastError();
					ATLASSERT(hr != S_OK);
					return hr;
				}

				DWORD dwRet = WaitForSingleObject(m_hThreadEvent, dwMaxWait);
				if (dwRet != WAIT_OBJECT_0)
				{
					if (dwRet == WAIT_TIMEOUT)
					{
						return HRESULT_FROM_WIN32(WAIT_TIMEOUT);
					}
					else
					{
						return AtlHresultFromLastError();
					}
				}

				if (m_threadMap.Add(dwThreadID, hdlThread) != FALSE)
				{
					hdlThread.Detach();
				}
			}
		}
		return S_OK;
	}

}; // class CThreadPool

//
// CNonStatelessWorker
// This class is a simple wrapper for use with CThreadPool.
//	It instantiates one instance of Worker per request
//	this allows Worker to hold state for each request
//	and depend on the destructor being called
//	Worker:
//		is a class that is responsible for handling requests
//		queued on the thread pool (See CThreadPool)
template <class Worker>
class CNonStatelessWorker
{
public:
	typedef typename Worker::RequestType RequestType;

	BOOL Initialize(void * /*pvParam*/) throw()
	{
		return TRUE;
	}

	void Execute(typename Worker::RequestType request, void *pvWorkerParam, OVERLAPPED *pOverlapped)
	{
		Worker worker;
		if (worker.Initialize(pvWorkerParam))
		{
			worker.Execute(request, pvWorkerParam, pOverlapped);
			worker.Terminate(pvWorkerParam);
		}
	}
	void Terminate(void* /*pvParam*/) throw()
	{
	}
}; // class CNonStatelessWorker


//Flags
#define ATL_URL_ESCAPE             1   // (un)escape URL characters
#define ATL_URL_NO_ENCODE          2   // Don't convert unsafe characters to escape sequence
#define ATL_URL_DECODE             4   // Convert %XX escape sequences to characters
#define ATL_URL_NO_META            8   // Don't convert .. etc. meta path sequences
#define ATL_URL_ENCODE_SPACES_ONLY 16  // Encode spaces only
#define ATL_URL_BROWSER_MODE       32  // Special encode/decode rules for browser
#define ATL_URL_ENCODE_PERCENT     64  // Encode percent (by default, not encoded)
#define ATL_URL_CANONICALIZE       128 // Internal: used by Canonicalize for AtlEscapeUrl: Cannot be set via SetFlags
#define ATL_URL_COMBINE            256 // Internal: Cannot be set via SetFlags


//Get the decimal value of a hexadecimal character
inline short AtlHexValue(char chIn) throw()
{
	unsigned char ch = (unsigned char)chIn;
	if (ch >= '0' && ch <= '9')
		return (short)(ch - '0');
	if (ch >= 'A' && ch <= 'F')
		return (short)(ch - 'A' + 10);
	if (ch >= 'a' && ch <= 'f')
		return (short)(ch - 'a' + 10);
	return -1;
}


//Determine if the character is unsafe under the URI RFC document
inline BOOL AtlIsUnsafeUrlChar(char chIn) throw()
{
	unsigned char ch = (unsigned char)chIn;
	switch(ch)
	{
		case ';': case '\\': case '?': case '@': case '&':
		case '=': case '+': case '$': case ',': case ' ':
		case '<': case '>': case '#': case '%': case '\"':
		case '{': case '}': case '|':
		case '^': case '[': case ']': case '`':
			return TRUE;
		default:
		{
			if (ch < 32 || ch > 126)
				return TRUE;
			return FALSE;
		}
	}
}

//Get the default internet port for a particular scheme
inline ATL_URL_PORT AtlGetDefaultUrlPort(ATL_URL_SCHEME m_nScheme) throw()
{
	switch (m_nScheme)
	{
		case ATL_URL_SCHEME_FTP:
			return ATL_URL_DEFAULT_FTP_PORT;
		case ATL_URL_SCHEME_GOPHER:
			return ATL_URL_DEFAULT_GOPHER_PORT;
		case ATL_URL_SCHEME_HTTP:
			return ATL_URL_DEFAULT_HTTP_PORT;
		case ATL_URL_SCHEME_HTTPS:
			return ATL_URL_DEFAULT_HTTPS_PORT;
		case ATL_URL_SCHEME_SOCKS:
			return ATL_URL_DEFAULT_SOCKS_PORT;
		default:
			return ATL_URL_INVALID_PORT_NUMBER;
	}
}

//Escape a meta sequence with lpszOutUrl as the base url and lpszInUrl as the relative url
//i.e. lpszInUrl = ./* or ../*
ATL_NOINLINE inline BOOL AtlEscapeUrlMetaHelper(
	LPSTR* ppszOutUrl,
	LPCSTR szPrev,
	DWORD dwOutLen,
	LPSTR* ppszInUrl,
	DWORD* pdwLen, 
	DWORD dwFlags = 0,
	DWORD dwColonPos = ATL_URL_MAX_URL_LENGTH) throw()
{
	ATLASSERT( ppszOutUrl != NULL );
	ATLASSERT( szPrev != NULL );
	ATLASSERT( ppszInUrl != NULL );
	ATLASSERT( pdwLen != NULL);

	LPSTR szOut = *ppszOutUrl;
	LPSTR szIn = *ppszInUrl;
	DWORD dwUrlLen = dwOutLen;
	char chPrev = *szPrev;
	BOOL bRet = FALSE;

	//if the previous character is a directory delimiter
	if (chPrev == '/' || chPrev == '\\')
	{
		char chNext = *szIn;

		//if the next character is a directory delimiter
		if (chNext == '/' || chNext == '\\')
		{
			//the meta sequence is of the form /./*
			szIn++;
			bRet = TRUE;
		}
		else if (chNext == '.' && ((chNext = *(szIn+1)) == '/' || 
			chNext == '\\' || chNext == '\0'))
		{
			//otherwise if the meta sequence is of the form "/../"
			//skip the preceding "/"
			szOut--;

			//skip the ".." of the meta sequence
			szIn+= 2;
			DWORD dwOutPos = dwUrlLen-1;
			LPSTR szTmp = szOut;

			//while we are not at the beginning of the base url
			while (dwOutPos)
			{
				szTmp--;
				dwOutPos--;

				//if it is a directory delimiter
				if (*szTmp == '/' || *szTmp == '\\')
				{
					//if we are canonicalizing the url and NOT combining it
					//and if we have encountered the ':' or we are at a position before the ':'
					if ((dwFlags & ATL_URL_CANONICALIZE) && ((dwFlags & ATL_URL_COMBINE) == 0) &&
						(dwColonPos && (dwOutPos <= dwColonPos+1)))
					{
						//NOTE: this is to match the way that InternetCanonicalizeUrl and 
						//      InternetCombineUrl handle this case
						break;
					}

					//otherwise, set the current output string position to right after the '/'
					szOut = szTmp+1;

					//update the length to match
					dwUrlLen = dwOutPos+1;
					bRet = TRUE;
					break;
				}
			}

			//if we could not properly escape the meta sequence
			if (dwUrlLen != dwOutPos+1)
			{
				//restore everything to its original value
				szIn-= 2;
				szOut++;
			}
			else
			{
				bRet = TRUE;
			}
		}
	}
	//update the strings
	*ppszOutUrl = szOut;
	*ppszInUrl = szIn;
	*pdwLen = dwUrlLen;
	return bRet;
}

//Convert all unsafe characters in szStringIn to escape sequences
//lpszStringIn and lpszStringOut should be different strings
inline BOOL AtlEscapeUrl(
	LPCSTR szStringIn,
	LPSTR szStringOut,
	DWORD* pdwStrLen,
	DWORD dwMaxLength,
	DWORD dwFlags = 0) throw()
{
	ATLASSERT( szStringIn != NULL );
	ATLASSERT( szStringOut != NULL );
	ATLASSERT( szStringIn != szStringOut );

	char ch;
	DWORD dwLen = 0;
	BOOL bRet = TRUE;
	BOOL bSchemeFile = FALSE;
	DWORD dwColonPos = 0;
	DWORD dwFlagsInternal = dwFlags;
	while((ch = *szStringIn++) != '\0')
	{
		//if we are at the maximum length, set bRet to FALSE
		//this ensures no more data is written to szStringOut, but
		//the length of the string is still updated, so the user
		//knows how much space to allocate
		if (dwLen == dwMaxLength)
		{
			bRet = FALSE;
		}

		//Keep track of the first ':' position to match the weird way
		//InternetCanonicalizeUrl handles it
		if (ch == ':' && (dwFlagsInternal & ATL_URL_CANONICALIZE) && !dwColonPos)
		{
			if (bRet)
			{
				*szStringOut = '\0';
				_strlwr(szStringOut-dwLen);

				if (dwLen == 4 && !strncmp("file", (szStringOut-4), 4))
				{
					bSchemeFile = TRUE;
				}
			}

			dwColonPos = dwLen+1;
		}
		else if (ch == '%' && (dwFlagsInternal & ATL_URL_DECODE))
		{
			//decode the escaped sequence
			ch = (char)(16*AtlHexValue(*szStringIn++));
			ch = (char)(ch+AtlHexValue(*szStringIn++));
		}
		else if ((ch == '?' || ch == '#') && (dwFlagsInternal & ATL_URL_BROWSER_MODE))
		{
			//ATL_URL_BROWSER mode does not encode after a '?' or a '#'
			dwFlagsInternal |= ATL_URL_NO_ENCODE;
		}

		if ((dwFlagsInternal & ATL_URL_CANONICALIZE) && (dwFlagsInternal & ATL_URL_NO_ENCODE)==0)
		{
			//canonicalize the '\' to '/'
			if (ch == '\\' && (dwColonPos || (dwFlagsInternal & ATL_URL_COMBINE)) && bRet)
			{
				//if the scheme is not file or it is file and the '\' is in "file:\\"
				//NOTE: This is to match the way InternetCanonicalizeUrl handles this case
				if (!bSchemeFile || (dwLen < 7))
				{
					ch = '/';
				}
			}
			else if (ch == '.' && dwLen > 0 && (dwFlagsInternal & ATL_URL_NO_META)==0)
			{
				//if we are escaping meta sequences, attempt to do so
				if (AtlEscapeUrlMetaHelper(&szStringOut, szStringOut-1, dwLen, (char**)(&szStringIn), &dwLen, dwFlagsInternal, dwColonPos))
					continue;
			}
		}

		//if we are encoding and it is an unsafe character
		if (AtlIsUnsafeUrlChar(ch) && (dwFlagsInternal & ATL_URL_NO_ENCODE)==0)
		{
			//if we are only encoding spaces, and ch is not a space or
			//if we are not encoding meta sequences and it is a dot or
			//if we not encoding percents and it is a percent
			if (((dwFlagsInternal & ATL_URL_ENCODE_SPACES_ONLY) && ch != ' ') ||
				((dwFlagsInternal & ATL_URL_NO_META) && ch == '.') ||
				(((dwFlagsInternal & ATL_URL_ENCODE_PERCENT) == 0) && ch == '%'))
			{
				//just output it without encoding
				if (bRet)
					*szStringOut++ = ch;
			}
			else 
			{
				//if there is not enough space for the escape sequence
				if (dwLen >= (dwMaxLength-3))
				{
					bRet = FALSE;
				}
				if (bRet)
				{
					//output the percent, followed by the hex value of the character
					LPSTR pszTmp = szStringOut;
					*pszTmp++ = '%';
					if ((unsigned char)ch < 16)
					{
						*pszTmp++ = '0';
					}
					_ultoa((unsigned char)ch, pszTmp, 16);
					szStringOut+= sizeof("%FF")-1;
				}
				dwLen += sizeof("%FF")-2;
			}
		}
		else //safe character
		{
			if (bRet)
				*szStringOut++ = ch;
		}
		dwLen++;
	}

	if (bRet && dwLen < dwMaxLength)
		*szStringOut = '\0';

	if (pdwStrLen)
		*pdwStrLen = dwLen + 1;

	if (dwLen+1 > dwMaxLength)
		bRet = FALSE;

	return bRet;
}

inline BOOL AtlEscapeUrl(
	LPCWSTR szStringIn,
	LPWSTR szStringOut,
	DWORD* pdwStrLen,
	DWORD dwMaxLength,
	DWORD dwFlags = 0) throw()
{
	// convert to UTF8
	BOOL bRet = FALSE;

	int nSrcLen = (int) wcslen(szStringIn);
	int nCnt = AtlUnicodeToUTF8(szStringIn, nSrcLen, NULL, 0);
	if (nCnt != 0)
	{
		nCnt++;
		CHeapPtr<char> szIn;

		char szInBuf[ATL_URL_MAX_URL_LENGTH];
		char *pszIn = szInBuf;

		// try to avoid allocation
		if (nCnt > ATL_URL_MAX_URL_LENGTH)
		{
			if (!szIn.AllocateBytes(nCnt))
			{
				// out of memory
				return FALSE;
			}
			pszIn = szIn;
		}

		nCnt = AtlUnicodeToUTF8(szStringIn, nSrcLen, pszIn, nCnt);
		ATLASSERT( nCnt != 0 );

		pszIn[nCnt] = '\0';

		char szOutBuf[ATL_URL_MAX_URL_LENGTH];
		char *pszOut = szOutBuf;
		CHeapPtr<char> szTmp;

		// try to avoid allocation
		if (dwMaxLength > ATL_URL_MAX_URL_LENGTH)
		{
			if (!szTmp.AllocateBytes(dwMaxLength))
			{
				// out of memory
				return FALSE;
			}
			pszOut = szTmp;
		}

		DWORD dwStrLen = 0;
		bRet = AtlEscapeUrl(pszIn, pszOut, &dwStrLen, dwMaxLength, dwFlags);
		if (bRet != FALSE)
		{
			// it is now safe to convert using any codepage, since there
			// are no non-ASCII characters
			_ATLTRY
			{
				memcpy(szStringOut, CA2W( pszOut ), dwStrLen*sizeof(wchar_t));
			}
			_ATLCATCHALL()
			{
				bRet = FALSE;
			}
		}
		if (pdwStrLen)
		{
			*pdwStrLen = dwStrLen;
		}
	}

	return bRet;
}

//Convert all escaped characters in szString to their real values
//lpszStringIn and lpszStringOut can be the same string	
inline BOOL AtlUnescapeUrl(
	LPCSTR szStringIn,
	LPSTR szStringOut,
	LPDWORD pdwStrLen,
	DWORD dwMaxLength) throw()
{
	ATLASSERT(szStringIn != NULL);
	ATLASSERT(szStringOut != NULL);

	int nValue = 0;
	char ch;
	DWORD dwLen = 0;
	BOOL bRet = TRUE;
	while ((ch = *szStringIn) != 0)
	{
		if (dwLen == dwMaxLength)
			bRet = FALSE;

		if (bRet)
		{
			if (ch == '%')
			{
				if ((*(szStringIn+1) == '\0') || (*(szStringIn+2) == '\0'))
				{
					bRet = FALSE;
					break;
				}
				ch = *(++szStringIn);
				//currently assuming 2 hex values after '%'
				//as per the RFC 2396 document
				nValue = 16*AtlHexValue(ch);
				nValue+= AtlHexValue(*(++szStringIn));
				*szStringOut++ = (char) nValue;
			}
			else //non-escape character
			{
				if (bRet)
					*szStringOut++ = ch;
			}
		}
		dwLen++;
		szStringIn++;
	}

	if (bRet && dwLen < dwMaxLength)
		*szStringOut = '\0';

	if (pdwStrLen)
		*pdwStrLen = dwLen + 1;

	if (dwLen+1 > dwMaxLength)
		bRet = FALSE;

	return bRet;
}

inline BOOL AtlUnescapeUrl(
	LPCWSTR szStringIn,
	LPWSTR szStringOut,
	LPDWORD pdwStrLen,
	DWORD dwMaxLength) throw()
{
	/// convert to UTF8
	BOOL bRet = FALSE;

	int nSrcLen = (int) wcslen(szStringIn);
	int nCnt = AtlUnicodeToUTF8(szStringIn, nSrcLen, NULL, 0);
	if (nCnt != 0)
	{
		nCnt++;
		CHeapPtr<char> szIn;

		char szInBuf[ATL_URL_MAX_URL_LENGTH];
		char *pszIn = szInBuf;

		// try to avoid allocation
		if (nCnt > ATL_URL_MAX_URL_LENGTH)
		{
			if (!szIn.AllocateBytes(nCnt))
			{
				// out of memory
				return FALSE;
			}
			pszIn = szIn;
		}

		nCnt = AtlUnicodeToUTF8(szStringIn, nSrcLen, pszIn, nCnt);
		ATLASSERT( nCnt != 0 );

		pszIn[nCnt] = '\0';

		char szOutBuf[ATL_URL_MAX_URL_LENGTH];
		char *pszOut = szOutBuf;
		CHeapPtr<char> szTmp;

		// try to avoid allocation
		if (dwMaxLength > ATL_URL_MAX_URL_LENGTH)
		{
			if (!szTmp.AllocateBytes(dwMaxLength))
			{
				// out of memory
				return FALSE;
			}
			pszOut = szTmp;
		}

		DWORD dwStrLen = 0;
		bRet = AtlUnescapeUrl(pszIn, pszOut, &dwStrLen, dwMaxLength);
		if (bRet != FALSE)
		{
			// it is now safe to convert using any codepage, since there
			// are no non-ASCII characters
			_ATLTRY
			{
				memcpy(szStringOut, CA2W( pszOut ), dwStrLen*sizeof(wchar_t));
			}
			_ATLCATCHALL()
			{
				bRet = FALSE;
			}
		}
		if (pdwStrLen)
		{
			*pdwStrLen = dwStrLen;
		}
	}

	return bRet;
}

//Canonicalize a URL (same as InternetCanonicalizeUrl)
inline BOOL AtlCanonicalizeUrl(
	LPCTSTR szUrl,
	LPTSTR szCanonicalized,
	DWORD* pdwMaxLength,
	DWORD dwFlags = 0) throw()
{
	ATLASSERT( szUrl != NULL );
	ATLASSERT( szCanonicalized != NULL );
	ATLASSERT( pdwMaxLength != NULL);

	return AtlEscapeUrl(szUrl, szCanonicalized, pdwMaxLength, *pdwMaxLength, dwFlags | ATL_URL_CANONICALIZE);
}

//Combine a base and relative URL (same as InternetCombineUrl)
inline BOOL AtlCombineUrl(
	LPCTSTR szBaseUrl,
	LPCTSTR szRelativeUrl,
	LPTSTR szBuffer,
	DWORD* pdwMaxLength,
	DWORD dwFlags = 0) throw()
{
	ATLASSERT(szBaseUrl != NULL);
	ATLASSERT(szRelativeUrl != NULL);
	ATLASSERT(szBuffer != NULL);
	ATLASSERT(pdwMaxLength != NULL);

	size_t nLen1 = _tcslen(szBaseUrl);
	TCHAR szCombined[2*ATL_URL_MAX_URL_LENGTH];
	if (nLen1 >= 2*ATL_URL_MAX_URL_LENGTH)
	{
		return FALSE;
	}

	_tcscpy(szCombined, szBaseUrl);

	// if last char of szBaseUrl is not a slash, add it.
	if (nLen1 > 0 && szCombined[nLen1-1] != _T('/'))
	{
		szCombined[nLen1] = _T('/');
		nLen1++;
		szCombined[nLen1] = _T('\0');
	}

	size_t nLen2 = _tcslen(szRelativeUrl);

	if (nLen2+nLen1+1 >= 2*ATL_URL_MAX_URL_LENGTH)
	{
		return FALSE;
	}

	_tcsncpy(szCombined+nLen1, szRelativeUrl, nLen2+1);
	DWORD dwLen = (DWORD) (nLen1+nLen2);
	if (dwLen >= *pdwMaxLength)
	{
		*pdwMaxLength = dwLen;
		return FALSE;
	}
	return AtlEscapeUrl(szCombined, szBuffer, pdwMaxLength, *pdwMaxLength, dwFlags | ATL_URL_COMBINE | ATL_URL_CANONICALIZE);
}

class CUrl
{
private:
	//scheme names cannot contain escape/unsafe characters
	TCHAR m_szScheme[ATL_URL_MAX_SCHEME_LENGTH+1];

	//host names cannot contain escape/unsafe characters
	TCHAR m_szHostName[ATL_URL_MAX_HOST_NAME_LENGTH+1];

	TCHAR m_szUserName[ATL_URL_MAX_USER_NAME_LENGTH+1];
	TCHAR m_szPassword[ATL_URL_MAX_PASSWORD_LENGTH+1];
	TCHAR m_szUrlPath[ATL_URL_MAX_PATH_LENGTH+1];
	TCHAR m_szExtraInfo[ATL_URL_MAX_PATH_LENGTH+1];

	ATL_URL_PORT m_nPortNumber;
	ATL_URL_SCHEME m_nScheme;

	DWORD m_dwSchemeNameLength;
	DWORD m_dwHostNameLength;
	DWORD m_dwUserNameLength;
	DWORD m_dwPasswordLength;
	DWORD m_dwUrlPathLength;
	DWORD m_dwExtraInfoLength;

public:
	//Empty constructor
	CUrl() throw()
	{
		InitFields();
		SetScheme(ATL_URL_SCHEME_HTTP);
	}

	//Copy constructor--maybe make private
	CUrl(const CUrl& urlThat) throw()
	{
		CopyFields(urlThat);
	}

	//Destructor (empty)
	~CUrl() throw()
	{
	}

	CUrl& operator=(const CUrl& urlThat)  throw()
	{
		CopyFields(urlThat);
		return (*this);
	}

	//Set the url
	BOOL CrackUrl(LPCTSTR lpszUrl, DWORD dwFlags = 0) throw()
	{
		ATLASSERT(lpszUrl != NULL);

		InitFields();
		BOOL bRet = FALSE;
		if (dwFlags & ATL_URL_DECODE)
		{
			//decode the url before parsing it
			TCHAR szDecodedUrl[ATL_URL_MAX_URL_LENGTH];
			DWORD dwLen;
			if (!AtlUnescapeUrl(lpszUrl, szDecodedUrl, &dwLen, ATL_URL_MAX_URL_LENGTH))
				return FALSE;
			bRet = Parse(szDecodedUrl);
		}
		else
		{
			bRet = Parse(lpszUrl);
		}
		if (bRet && (dwFlags & ATL_URL_ESCAPE))
		{
			bRet = AtlUnescapeUrl(m_szUserName, m_szUserName, 
				&m_dwUserNameLength, ATL_URL_MAX_USER_NAME_LENGTH);
			if (bRet)
			{
				--m_dwUserNameLength;
				bRet = AtlUnescapeUrl(m_szPassword, m_szPassword, 
					&m_dwPasswordLength, ATL_URL_MAX_PASSWORD_LENGTH);
				if (bRet)
				{
					--m_dwPasswordLength;
					bRet = AtlUnescapeUrl(m_szUrlPath, m_szUrlPath, 
						&m_dwUrlPathLength, ATL_URL_MAX_PATH_LENGTH);
					if (bRet)
					{
						--m_dwUrlPathLength;
						bRet = AtlUnescapeUrl(m_szExtraInfo, m_szExtraInfo, 
							&m_dwExtraInfoLength, ATL_URL_MAX_PATH_LENGTH);
						if (bRet)
						{
							--m_dwExtraInfoLength;
						}
					}
				}
			}
		}
		return bRet;
	}

	inline BOOL CreateUrl(LPTSTR lpszUrl, DWORD* pdwMaxLength, DWORD dwFlags = 0) const throw()
	{
		ATLASSERT(lpszUrl != NULL);
		ATLASSERT(pdwMaxLength != NULL);

		if( pdwMaxLength == NULL )
			return FALSE;

		//build URL: <scheme>://<user>:<pass>@<domain>:<port><path><extra>
		TCHAR szPortNumber[ATL_URL_MAX_PORT_NUMBER_LENGTH+2];
		DWORD dwLength = *pdwMaxLength;
		*pdwMaxLength = GetUrlLength()+1;

		if( lpszUrl == NULL )
			return FALSE;

		if (*pdwMaxLength > dwLength)
			return FALSE;

		_stprintf(szPortNumber, _T(":%d"), m_nPortNumber);
		LPTSTR lpszOutUrl = lpszUrl;

		if (*m_szScheme)
		{
			_tcsncpy(lpszUrl, m_szScheme, m_dwSchemeNameLength);
			lpszUrl += m_dwSchemeNameLength;
			*lpszUrl++ = ':';
			if (m_nScheme != ATL_URL_SCHEME_MAILTO)
			{
				*lpszUrl++ = '/';
				*lpszUrl++ = '/';
			}
		}

		if (*m_szUserName)
		{
			_tcsncpy(lpszUrl, m_szUserName, m_dwUserNameLength);
			lpszUrl += m_dwUserNameLength;
			if (*m_szPassword)
			{
				*lpszUrl++ = ':';
				_tcsncpy(lpszUrl, m_szPassword, m_dwPasswordLength);
				lpszUrl += m_dwPasswordLength;
			}
			*lpszUrl++ = '@';
		}

		if (*m_szHostName)
		{
			_tcsncpy(lpszUrl, m_szHostName, m_dwHostNameLength);
			lpszUrl += m_dwHostNameLength;
			if (m_nPortNumber != AtlGetDefaultUrlPort(m_nScheme))
			{
				DWORD dwPortLen = (DWORD) _tcslen(szPortNumber);
				_tcsncpy(lpszUrl, szPortNumber, dwPortLen);
				lpszUrl += dwPortLen;
			}
			if (*m_szUrlPath && *m_szUrlPath != '/' && *m_szUrlPath != '\\')
				*lpszUrl++ = '/';
		}

		if (*m_szUrlPath)
		{
			_tcsncpy(lpszUrl, m_szUrlPath, m_dwUrlPathLength);
			lpszUrl+= m_dwUrlPathLength;
		}

		if (*m_szExtraInfo)
		{
			_tcsncpy(lpszUrl, m_szExtraInfo, m_dwExtraInfoLength);
			lpszUrl += m_dwExtraInfoLength;
		}
		*lpszUrl = '\0';

		(*pdwMaxLength)--;

		if (dwFlags & ATL_URL_ESCAPE)
		{
			TCHAR szUrl[ATL_URL_MAX_URL_LENGTH];
			_tcsncpy(szUrl, lpszOutUrl, *pdwMaxLength+1);
			if (AtlUnescapeUrl(szUrl, lpszOutUrl, pdwMaxLength, dwLength))
			{
				(*pdwMaxLength)--;
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}

		return TRUE;
	}

	inline void Clear() throw()
	{
		InitFields();
	}

	inline DWORD GetUrlLength() const throw()
	{
		//The conditionals in this method are related to the conditionals in the CreateUrl method
		//scheme + ':'
		DWORD dwUrlLength = m_dwSchemeNameLength+1;

		//i.e. "//"
		if (m_nScheme != ATL_URL_SCHEME_MAILTO)
			dwUrlLength += 2;  

		//i.e. "username@"
		if (m_dwUserNameLength > 0)
			dwUrlLength += m_dwUserNameLength+1;

		//i.e. ":password"
		if (m_dwPasswordLength > 0)
			dwUrlLength += m_dwPasswordLength+1;

		dwUrlLength += m_dwHostNameLength;

		// will need to add an extra '/' in this case
		if (m_dwHostNameLength && m_dwUrlPathLength && *m_szUrlPath != '/' && *m_szUrlPath != '\\')
			dwUrlLength++;

		//i.e. ":xx" where "xx" is the port number
		if (m_nPortNumber != AtlGetDefaultUrlPort(m_nScheme))
		{
			TCHAR szPortTmp[7];
			dwUrlLength += _stprintf(szPortTmp, _T(":%d"), m_nPortNumber);
		}

		dwUrlLength += m_dwUrlPathLength + m_dwExtraInfoLength;

		return dwUrlLength;
	}

	//Get the Scheme Name (i.e. http, ftp, etc.)
	inline LPCTSTR GetSchemeName() const throw()
	{
		return m_szScheme;
	}

	//Get the Scheme Name length
	inline DWORD GetSchemeNameLength() const throw()
	{
		return m_dwSchemeNameLength;
	}

	//This method will incur the cost of 
	//validating the scheme and updating the scheme name
	inline BOOL SetSchemeName(LPCTSTR lpszSchm) throw()
	{
		ATLASSERT(lpszSchm != NULL);

		const _schemeinfo *pSchemes = GetSchemes();

		ATLASSERT( pSchemes != NULL );

		int nScheme = -1;

		for (int i=0; i<s_nSchemes; i++)
		{
			if (_tcsicmp(lpszSchm, pSchemes[i].szSchemeName) == 0)
			{
				nScheme = i;
				break;
			}
		}

		if (nScheme != -1)
		{
			m_nScheme = (ATL_URL_SCHEME) nScheme;
			m_dwSchemeNameLength = pSchemes[nScheme].dwSchemeLength;
			m_nPortNumber = (ATL_URL_PORT) pSchemes[nScheme].nUrlPort;
		}
		else
		{
			// unknown scheme
			m_nScheme = ATL_URL_SCHEME_UNKNOWN;
			m_dwSchemeNameLength = (DWORD) _tcslen(lpszSchm);
			if (m_dwSchemeNameLength > ATL_URL_MAX_SCHEME_LENGTH)
			{
				// scheme name too long
				return FALSE;
			}

			m_nPortNumber = ATL_URL_INVALID_PORT_NUMBER;
		}

		_tcsncpy(m_szScheme, lpszSchm, m_dwSchemeNameLength);
		m_szScheme[m_dwSchemeNameLength] = '\0';

		return TRUE;
	}

	inline BOOL SetScheme(ATL_URL_SCHEME nScheme) throw()
	{
		if ((nScheme < 0) || (nScheme >= s_nSchemes))
		{
			// invalid scheme
			return FALSE;
		}

		const _schemeinfo *pSchemes = GetSchemes();

		ATLASSERT( pSchemes != NULL );

		m_nScheme = (ATL_URL_SCHEME) nScheme;
		m_dwSchemeNameLength = pSchemes[nScheme].dwSchemeLength;
		m_nPortNumber = (ATL_URL_PORT) pSchemes[nScheme].nUrlPort;
		_tcsncpy(m_szScheme, pSchemes[nScheme].szSchemeName, m_dwSchemeNameLength);

		return TRUE;
	}

	inline ATL_URL_SCHEME GetScheme() const throw()
	{
		return m_nScheme;
	}

	//Get the host name
	inline LPCTSTR GetHostName() const throw()
	{
		return m_szHostName;
	}

	//Get the host name's length
	inline DWORD GetHostNameLength() const throw()
	{
		return m_dwHostNameLength;
	}

	//Set the Host name
	inline BOOL SetHostName(LPCTSTR lpszHost) throw()
	{
		ATLASSERT(lpszHost != NULL);

		DWORD dwLen = (DWORD) _tcslen(lpszHost);
		if (dwLen > ATL_URL_MAX_HOST_NAME_LENGTH)
			return FALSE;

		_tcsncpy(m_szHostName, lpszHost, dwLen+1);
		m_dwHostNameLength = dwLen;

		return TRUE;
	}

	//Get the port number in terms of ATL_URL_PORT
	inline ATL_URL_PORT GetPortNumber() const throw()
	{
		return m_nPortNumber;
	}

	//Set the port number in terms of ATL_URL_PORT
	inline BOOL SetPortNumber(ATL_URL_PORT nPrt) throw()
	{
		m_nPortNumber = nPrt;
		return TRUE;
	}

	//Get the user name
	inline LPCTSTR GetUserName() const throw()
	{
		return m_szUserName;
	}

	//Get the user name's length
	inline DWORD GetUserNameLength() const throw()
	{
		return m_dwUserNameLength;
	}

	//Set the user name
	inline BOOL SetUserName(LPCTSTR lpszUser) throw()
	{
		ATLASSERT(lpszUser != NULL);

		DWORD dwLen = (DWORD) _tcslen(lpszUser);
		if (dwLen > ATL_URL_MAX_USER_NAME_LENGTH)
			return FALSE;

		_tcsncpy(m_szUserName, lpszUser, dwLen+1);
		m_dwUserNameLength = dwLen;

		return TRUE;
	}

	//Get the password
	inline LPCTSTR GetPassword() const throw()
	{
		return m_szPassword;
	}

	//Get the password's length
	inline DWORD GetPasswordLength() const throw()
	{
		return m_dwPasswordLength;
	}

	//Set the password
	inline BOOL SetPassword(LPCTSTR lpszPass) throw()
	{
		ATLASSERT(lpszPass != NULL);

		if (*lpszPass && !*m_szUserName)
			return FALSE;

		DWORD dwLen = (DWORD) _tcslen(lpszPass);
		if (dwLen > ATL_URL_MAX_PASSWORD_LENGTH)
			return FALSE;

		_tcsncpy(m_szPassword, lpszPass, dwLen+1);
		m_dwPasswordLength = dwLen;

		return TRUE;
	}

	//Get the url path (everything after scheme and
	//before extra info)
	inline LPCTSTR GetUrlPath() const throw()
	{
		return m_szUrlPath;
	}

	//Get the url path's length
	inline DWORD GetUrlPathLength() const throw()
	{
		return m_dwUrlPathLength;
	}

	//Set the url path
	inline BOOL SetUrlPath(LPCTSTR lpszPath) throw()
	{
		ATLASSERT(lpszPath != NULL);

		DWORD dwLen = (DWORD) _tcslen(lpszPath);
		if (dwLen > ATL_URL_MAX_PATH_LENGTH)
			return FALSE;

		_tcsncpy(m_szUrlPath, lpszPath, dwLen+1);
		m_dwUrlPathLength = dwLen;

		return TRUE;
	}

	//Get extra info (i.e. ?something or #something)
	inline LPCTSTR GetExtraInfo() const throw()
	{
		return m_szExtraInfo;
	}

	//Get extra info's length
	inline DWORD GetExtraInfoLength() const throw()
	{
		return m_dwExtraInfoLength;
	}

	//Set extra info
	inline BOOL SetExtraInfo(LPCTSTR lpszInfo) throw()
	{
		ATLASSERT(lpszInfo != NULL);

		DWORD dwLen = (DWORD) _tcslen(lpszInfo);
		if (dwLen > ATL_URL_MAX_PATH_LENGTH)
			return FALSE;

		_tcsncpy(m_szExtraInfo, lpszInfo, dwLen+1);
		m_dwExtraInfoLength = dwLen;

		return TRUE;
	}

	//Insert Escape characters into URL
	inline BOOL Canonicalize(DWORD dwFlags = 0) throw()
	{
		_tcslwr(m_szScheme);
		TCHAR szTmp[ATL_URL_MAX_URL_LENGTH];
		_tcscpy(szTmp, m_szUserName);
		// AtlEscapeUrl returns the size of the buffer required to hold the data
		// including the NULL terminator. However, CUrl stores the lengths of the 
		// URL components as length - NULL terminator, similar to what you'd get
		// if you did a strlen on the URL component so we have to adjust the lengths
		// that come back from AtlEscapeUrl
		BOOL bRet = AtlEscapeUrl(szTmp, m_szUserName, &m_dwUserNameLength, ATL_URL_MAX_USER_NAME_LENGTH, dwFlags);
		if (bRet)
		{
			m_dwUserNameLength--;
			_tcscpy(szTmp, m_szPassword);
			bRet = AtlEscapeUrl(szTmp, m_szPassword, &m_dwPasswordLength, ATL_URL_MAX_PASSWORD_LENGTH, dwFlags);
		}
		if (bRet)
		{
			m_dwPasswordLength--;
			_tcscpy(szTmp, m_szHostName);
			bRet = AtlEscapeUrl(szTmp, m_szHostName, &m_dwHostNameLength, ATL_URL_MAX_HOST_NAME_LENGTH, dwFlags);
		}
		if (bRet)
		{
			m_dwHostNameLength--;
			_tcscpy(szTmp, m_szUrlPath);
			bRet = AtlEscapeUrl(szTmp, m_szUrlPath, &m_dwUrlPathLength, ATL_URL_MAX_PATH_LENGTH, dwFlags);
			if (bRet)
				m_dwUrlPathLength--;
		}


		//in ATL_URL_BROWSER mode, the portion of the URL following the '?' or '#' is not encoded
		if (bRet && (dwFlags & ATL_URL_BROWSER_MODE) == 0)
		{
			_tcscpy(szTmp, m_szExtraInfo);
			bRet = AtlEscapeUrl(szTmp+1, m_szExtraInfo+1, &m_dwExtraInfoLength, ATL_URL_MAX_PATH_LENGTH-1, dwFlags);
		}

		return bRet;
	}

private:

	const static DWORD s_nSchemes = 8;

	struct _schemeinfo
	{
		LPCTSTR szSchemeName;
		DWORD dwSchemeLength;
		ATL_URL_PORT nUrlPort;
	};

	const _schemeinfo * GetSchemes() throw()
	{
		const static _schemeinfo s_schemes[] =
		{
			{ _T("ftp"), sizeof("ftp")-1, ATL_URL_DEFAULT_FTP_PORT },
			{ _T("gopher"), sizeof("gopher")-1, ATL_URL_DEFAULT_GOPHER_PORT },
			{ _T("http"), sizeof("http")-1, ATL_URL_DEFAULT_HTTP_PORT },
			{ _T("https"), sizeof("https")-1, ATL_URL_DEFAULT_HTTPS_PORT },
			{ _T("file"), sizeof("file")-1, ATL_URL_INVALID_PORT_NUMBER },
			{ _T("news"), sizeof("news")-1, ATL_URL_INVALID_PORT_NUMBER },
			{ _T("mailto"), sizeof("mailto")-1, ATL_URL_INVALID_PORT_NUMBER },
			{ _T("socks"), sizeof("socks")-1, ATL_URL_DEFAULT_SOCKS_PORT }
		};

		return s_schemes;
	}

	inline BOOL Parse(LPCTSTR lpszUrl) throw()
	{
		ATLASSERT(lpszUrl != NULL);

		TCHAR ch;
		BOOL bGotScheme = FALSE;
		BOOL bGotUserName = FALSE;
		BOOL bGotHostName = FALSE;
		BOOL bGotPortNumber = FALSE;
		TCHAR szCurrentUrl[ATL_URL_MAX_URL_LENGTH+6];
		TCHAR* pszCurrentUrl = szCurrentUrl;
		size_t nUrlSize = 0;

		//parse lpszUrl using szCurrentUrl to store temporary data

		//this loop will get the following if it exists:
		//<protocol>://user:pass@server:port
		while ((ch = *lpszUrl) != '\0')
		{
			if (nUrlSize >= ATL_URL_MAX_URL_LENGTH+5)
				goto error;

			if (ch == ':')
			{
				//3 cases:
				//(1) Just encountered a scheme
				//(2) Port number follows
				//(3) Form of username:password@

				// Check to see if we've just encountered a scheme
				*pszCurrentUrl = '\0';
				if (!bGotScheme)
				{
					if (!SetSchemeName(szCurrentUrl))
						goto error;

					//Set a flag to avoid checking for
					//schemes everytime we encounter a :
					bGotScheme = TRUE;

					if (*(lpszUrl+1) == '/')
					{
						if (*(lpszUrl+2) == '/')
						{
							//the mailto scheme cannot have a '/' following the "mailto:" portion
							if (bGotScheme && m_nScheme == ATL_URL_SCHEME_MAILTO)
								goto error;

							//Skip these characters and continue
							lpszUrl+= 2;
						}
						else 
						{
							//it is an absolute path
							//no domain name, port, username, or password is allowed in this case
							//break to loop that gets path
							lpszUrl++;
							pszCurrentUrl = szCurrentUrl;
							nUrlSize = 0;
							break;
						}
					}

					//reset pszCurrentUrl
					pszCurrentUrl = szCurrentUrl;
					nUrlSize = 0;
					lpszUrl++;

					//if the scheme is file, skip to getting the path information
					if (m_nScheme == ATL_URL_SCHEME_FILE)
						break;
					continue;
				}
				else if (!bGotUserName || !bGotPortNumber)
				{
					//It must be a username:password or a port number
					*pszCurrentUrl = '\0';

					pszCurrentUrl = szCurrentUrl;
					nUrlSize = 0;
					TCHAR tmpBuf[ATL_URL_MAX_PASSWORD_LENGTH+1];
					TCHAR* pTmpBuf = tmpBuf;
					int nCnt = 0;

					//get the user or portnumber (break on either '/', '@', or '\0'
					while (((ch = *(++lpszUrl)) != '/') && (ch != '@') && (ch != '\0'))
					{
						if (nCnt >= ATL_URL_MAX_PASSWORD_LENGTH)
							goto error;

						*pTmpBuf++ = ch;
						nCnt++;
					}
					*pTmpBuf = '\0';

					//if we broke on a '/' or a '\0', it must be a port number
					if (!bGotPortNumber && (ch == '/' || ch == '\0'))
					{
						//the host name must immediately preced the port number
						if (!SetHostName(szCurrentUrl))
							goto error;

						//get the port number
						m_nPortNumber = (ATL_URL_PORT) _ttoi(tmpBuf);
						if (m_nPortNumber < 0)
							goto error;

						bGotPortNumber = bGotHostName = TRUE;
					}
					else if (!bGotUserName && ch=='@')
					{
						//otherwise it must be a username:password
						if (!SetUserName(szCurrentUrl) || !SetPassword(tmpBuf))
							goto error;

						bGotUserName = TRUE;
						lpszUrl++;
					}
					else
					{
						goto error;
					}
				}
			}
			else if (ch == '@')
			{
				if (bGotUserName)
					goto error;

				//type is userinfo@
				*pszCurrentUrl = '\0';
				if (!SetUserName(szCurrentUrl))
					goto error;

				bGotUserName = TRUE;
				lpszUrl++;
				pszCurrentUrl = szCurrentUrl;
				nUrlSize = 0;
			}
			else if (ch == '/' || ch == '?' || (!*(lpszUrl+1)))
			{
				//we're at the end of this loop
				//set the domain name and break
				if (!*(lpszUrl+1) && ch != '/' && ch != '?')
				{
					if (nUrlSize >= ATL_URL_MAX_URL_LENGTH+4)
						goto error;

					*pszCurrentUrl++ = ch;
					nUrlSize++;
					lpszUrl++;
				}
				*pszCurrentUrl = '\0';
				if (!bGotHostName)
				{
					if (!SetHostName(szCurrentUrl))
						goto error;
				}
				pszCurrentUrl = szCurrentUrl;
				nUrlSize = 0;
				break;
			}
			else
			{
				*pszCurrentUrl++ = ch;
				lpszUrl++;
				nUrlSize++;
			}
		}

		if (!bGotScheme)
			goto error;

		//Now build the path
		while ((ch = *lpszUrl) != '\0')
		{
			if (nUrlSize >= ATL_URL_MAX_URL_LENGTH+5)
				goto error;

			//break on a '#' or a '?', which delimit "extra information"
			if (m_nScheme != ATL_URL_SCHEME_FILE && (ch == '#' || ch == '?'))
			{
				break;
			}
			*pszCurrentUrl++ = ch;
			nUrlSize++;
			lpszUrl++;
		}
		*pszCurrentUrl = '\0';

		if (*szCurrentUrl != '\0' && !SetUrlPath(szCurrentUrl))
			goto error;

		pszCurrentUrl = szCurrentUrl;
		nUrlSize = 0;

		while ((ch = *lpszUrl++) != '\0')
		{
			if (nUrlSize >= ATL_URL_MAX_URL_LENGTH+5)
				goto error;

			*pszCurrentUrl++ = ch;
			nUrlSize++;
		}

		*pszCurrentUrl = '\0';
		if (*szCurrentUrl != '\0' && !SetExtraInfo(szCurrentUrl))
			goto error;

		switch(m_nScheme)
		{
			case ATL_URL_SCHEME_FILE:
				m_nPortNumber = ATL_URL_INVALID_PORT_NUMBER;
				break;
			case ATL_URL_SCHEME_NEWS:
				m_nPortNumber = ATL_URL_INVALID_PORT_NUMBER;
				break;
			case ATL_URL_SCHEME_MAILTO:
				m_nPortNumber = ATL_URL_INVALID_PORT_NUMBER;
				break;
			default:
				if (!bGotPortNumber)
					m_nPortNumber = (unsigned short)AtlGetDefaultUrlPort(m_nScheme);
		}

		return TRUE;

error:
		InitFields();
		return FALSE;

	}

	ATL_NOINLINE void InitFields() throw()
	{
		m_nPortNumber = ATL_URL_INVALID_PORT_NUMBER;
		m_nScheme = ATL_URL_SCHEME_UNKNOWN;

		m_dwSchemeNameLength = 0;
		m_dwHostNameLength   = 0;
		m_dwUserNameLength   = 0;
		m_dwUrlPathLength    = 0;
		m_dwPasswordLength   = 0;
		m_dwExtraInfoLength  = 0;

		m_szScheme[0]        = '\0';
		m_szHostName[0]      = '\0';
		m_szUserName[0]      = '\0';
		m_szPassword[0]      = '\0';
		m_szUrlPath[0]       = '\0';
		m_szExtraInfo[0]     = '\0';
	}

	//copy all fields from urlThat
	inline void CopyFields(const CUrl& urlThat) throw()
	{
		_tcsncpy(m_szScheme, urlThat.m_szScheme, urlThat.m_dwSchemeNameLength+1);
		_tcsncpy(m_szHostName, urlThat.m_szHostName, urlThat.m_dwHostNameLength+1);
		_tcsncpy(m_szUserName, urlThat.m_szUserName, urlThat.m_dwUserNameLength+1);
		_tcsncpy(m_szPassword, urlThat.m_szPassword, urlThat.m_dwPasswordLength+1);
		_tcsncpy(m_szUrlPath, urlThat.m_szUrlPath, urlThat.m_dwUrlPathLength+1);
		_tcsncpy(m_szExtraInfo, urlThat.m_szExtraInfo, urlThat.m_dwExtraInfoLength+1);

		m_nPortNumber        = urlThat.m_nPortNumber;
		m_nScheme            = urlThat.m_nScheme;
		m_dwSchemeNameLength = urlThat.m_dwSchemeNameLength;
		m_dwHostNameLength   = urlThat.m_dwHostNameLength;
		m_dwUserNameLength   = urlThat.m_dwUserNameLength;
		m_dwPasswordLength   = urlThat.m_dwPasswordLength;
		m_dwUrlPathLength    = urlThat.m_dwUrlPathLength;
		m_dwExtraInfoLength  = urlThat.m_dwExtraInfoLength;
	}

}; // class CUrl

typedef CUrl* LPURL;
typedef const CUrl * LPCURL;


#ifndef ATL_WORKER_THREAD_WAIT
#define ATL_WORKER_THREAD_WAIT 10000	// time to wait when shutting down
#endif

//
// CWorkerThread
// This class creates a worker thread that waits on kernel
// object handles and executes a specified client
// function when the handle is signaled
// To use it, construct an instance, call Initialize
// then call add AddHandle with the handle of a kernel
// object and pass a pointer to your implementation
// of IWorkerThreadClient.  Execute on your IWorkerThreadClient
// implementation will be called when the handle is signaled
// You can also use AddTimer() to add a waitable timer
// to the worker thread.
// If the thread is still active when your object is destroyed
// you must call RemoveHandle() on each handle that your object
// owns.
// To terminate the thread, call Shutdown
//
template <class ThreadTraits=DefaultThreadTraits>
class CWorkerThread
{
protected:
	HANDLE m_hThread;
	DWORD m_dwThreadId;
	CWorkerThread<ThreadTraits> *m_pThread;
	struct WorkerClientEntry
	{
		IWorkerThreadClient *pClient;
		DWORD_PTR dwParam;
	};

	CSimpleArray<HANDLE> m_hWaitHandles;
	CSimpleArray<WorkerClientEntry, CSimpleArrayEqualHelperFalse<WorkerClientEntry> > m_ClientEntries;
	CComCriticalSection m_critSec;
	HANDLE m_hRefreshComplete;

	HRESULT Refresh() throw()
	{
		ATLASSERT(m_hRefreshComplete);

		DWORD dwRet = ERROR_SUCCESS;
		BOOL bRet = SetEvent(m_hWaitHandles[1]);
		if (bRet)
		{
			dwRet = WaitForSingleObject(m_hRefreshComplete, INFINITE);
			if (dwRet != WAIT_OBJECT_0)
			{
				bRet = FALSE;
			}
		}

		if (bRet)
		{
			return S_OK;
		}

		return (dwRet != ERROR_SUCCESS) ? AtlHresultFromWin32(dwRet) : AtlHresultFromLastError();
	}

public:
	CWorkerThread() throw() :
	  m_hThread(NULL),
	  m_dwThreadId(0),
	  m_hRefreshComplete(NULL),
	  m_pThread(NULL)
	{
	}

	~CWorkerThread() throw()
	{
		Shutdown();
	}

	DWORD GetThreadId() throw()
	{
		if (m_pThread)
			return m_pThread->GetThreadId();

		return m_dwThreadId;
	}

	HANDLE GetThreadHandle() throw()
	{
		if (m_pThread)
			return m_pThread->GetThreadHandle();

		return m_hThread;
	}

	HRESULT Initialize() throw()
	{
		if (m_pThread)
			return E_UNEXPECTED; // already initialized!

		// the object should be initialized first
		ATLASSERT(m_hWaitHandles.GetSize() == 0);

		HRESULT hr = m_critSec.Init();
		if (FAILED(hr))
		{
			return hr;
		}

		// create the refresh complete event
		m_hRefreshComplete = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (!m_hRefreshComplete)
		{
			m_critSec.Term();
			return AtlHresultFromLastError();
		}

		// add the shutdown event

		HANDLE hEventShutdown = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (!hEventShutdown)
		{
			hr = AtlHresultFromLastError();
			Shutdown();
			return hr;
		}

		hr = AddHandle(hEventShutdown, NULL, 0);
		if (FAILED(hr))
		{
			CloseHandle(hEventShutdown);
			Shutdown();
			return hr;
		}

		// create the refresh event
		HANDLE hEventRefresh = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (!hEventRefresh)
		{
			hr = AtlHresultFromLastError();
			Shutdown();
			return hr;
		}

		hr = AddHandle(hEventRefresh, NULL, 0);
		if (FAILED(hr))
		{
			CloseHandle(hEventRefresh);
			Shutdown();
			return hr;
		}

		m_hThread = ThreadTraits::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) _WorkerThreadProc, 
			this, 0, &m_dwThreadId);
		if (!m_hThread)
		{
			hr = AtlHresultFromLastError();
			Shutdown();
			return hr;
		}

		if (WaitForSingleObject(m_hRefreshComplete, INFINITE) != WAIT_OBJECT_0)
		{
			hr = AtlHresultFromLastError();
		}

		return hr;
	}

	HRESULT Initialize(CWorkerThread<ThreadTraits> *pThread) throw()
	{
		if (!pThread)
			return E_INVALIDARG;

		if (m_hThread)
			return E_UNEXPECTED; // already initialized

		if (m_pThread && (pThread != m_pThread))
			return E_UNEXPECTED; // already initialized

		m_pThread = pThread;
		return S_OK;
	}

	HRESULT AddHandle(HANDLE hObject, IWorkerThreadClient *pClient, DWORD_PTR dwParam) throw()
	{
		if (m_pThread)
			return m_pThread->AddHandle(hObject, pClient, dwParam);
		// Make sure the object has been initialized
		ATLASSERT(m_hRefreshComplete != NULL);

		CComCritSecLock<CComCriticalSection> lock(m_critSec, false);
		HRESULT hr = lock.Lock();
		if (FAILED(hr))
		{
			return hr;
		}

		if (m_hWaitHandles.GetSize() == MAXIMUM_WAIT_OBJECTS)
		{
			return AtlHresultFromWin32(ERROR_INVALID_PARAMETER);
		}
		BOOL bRet = m_hWaitHandles.Add(hObject);
		if (!bRet)
		{
			return E_OUTOFMEMORY;
		}

		WorkerClientEntry entry;
		entry.pClient = pClient;
		entry.dwParam = dwParam;
		bRet = m_ClientEntries.Add(entry);
		if (!bRet)
		{
			m_hWaitHandles.RemoveAt(m_hWaitHandles.GetSize()-1);
			return E_OUTOFMEMORY;
		}
		if (m_hWaitHandles.GetSize() > 2)
		{
			// tell the worker thread to refresh
			hr = Refresh();

			ATLTRACE( atlTraceUtil, 0, _T("Refresh result: 0x%08X"), hr );
		}

		return hr;
	}

#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
	HRESULT AddTimer(DWORD dwInterval, IWorkerThreadClient *pClient, DWORD_PTR dwParam, HANDLE *phTimer) throw()
	{
		if (m_pThread)
			return m_pThread->AddTimer(dwInterval, pClient, dwParam, phTimer);
		// Make sure the object has been initialized
		ATLASSERT(m_hRefreshComplete != NULL);

		ATLASSERT(phTimer);
		*phTimer = NULL;

		HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
		if (!hTimer)
		{
			return AtlHresultFromLastError();
		}

		HRESULT hr;
		LARGE_INTEGER liDueTime;

		liDueTime.QuadPart = -10000 * (__int64) dwInterval;

		BOOL bRet = SetWaitableTimer(hTimer, &liDueTime, dwInterval,  NULL, NULL, FALSE);
		if (!bRet)
		{
			hr = AtlHresultFromLastError();
			CloseHandle(hTimer);
			return hr;
		}

		hr = AddHandle(hTimer, pClient, dwParam);
		if (FAILED(hr))
		{
			CloseHandle(hTimer);
			return hr;
		}
		if (phTimer)
			*phTimer = hTimer;
		return S_OK;
	}
#endif

	HRESULT RemoveHandle(HANDLE hObject) throw()
	{
		if (m_pThread)
			return m_pThread->RemoveHandle(hObject);

		// Make sure the object has been initialized
		ATLASSERT(m_hRefreshComplete != NULL);

		CComCritSecLock<CComCriticalSection> lock(m_critSec, false);
		HRESULT hr = lock.Lock();
		if (FAILED(hr))
		{
			return hr;
		}

		int nIndex = m_hWaitHandles.Find(hObject);
		if (nIndex >= 0)
		{
			ATLASSERT(nIndex < m_ClientEntries.GetSize());

			IWorkerThreadClient *pClient = m_ClientEntries[nIndex].pClient;

			m_hWaitHandles.RemoveAt(nIndex);
			m_ClientEntries.RemoveAt(nIndex);

			// refresh 
			hr = Refresh();

			ATLTRACE( "Refresh result: 0x%08X", hr );

			// now it is safe to close the handle
			if (!pClient || FAILED(pClient->CloseHandle(hObject)))
			{
				CloseHandle(hObject);
			}
		}
		else
		{
			hr = E_FAIL;
		}

		return hr;
	}


	HRESULT Shutdown(DWORD dwWait=ATL_WORKER_THREAD_WAIT) throw()
	{
		if (m_pThread)
			return S_OK;

		if (!m_hThread)
		{
			RemoveAllClients();
			m_critSec.Term();
			if (m_hRefreshComplete)
			{
				CloseHandle(m_hRefreshComplete);
				m_hRefreshComplete = NULL;
			}
			return S_OK;
		}

		ATLASSERT(m_hWaitHandles.GetSize() > 0);

		SetEvent(m_hWaitHandles[0]);

		DWORD dwRet = WaitForSingleObject(m_hThread, dwWait);

		RemoveAllClients();

		CloseHandle(m_hThread);
		m_hThread = NULL;
		if (m_hRefreshComplete)
		{
			CloseHandle(m_hRefreshComplete);
			m_hRefreshComplete = NULL;
		}
		m_critSec.Term();
		return (dwRet == WAIT_OBJECT_0) ? S_OK : AtlHresultFromWin32(dwRet);
	}

private:

	int CopyHandles(HANDLE handles[MAXIMUM_WAIT_OBJECTS]) throw()
	{
		int i;
		for (i=0; i<m_hWaitHandles.GetSize(); i++)
		{
			handles[i] = m_hWaitHandles[i];
		}
		for (;i<MAXIMUM_WAIT_OBJECTS; i++)
		{
			handles[i] = INVALID_HANDLE_VALUE;
		}

		return m_hWaitHandles.GetSize();
	}

	int CopyClientEntries(WorkerClientEntry clientEntries[MAXIMUM_WAIT_OBJECTS]) throw()
	{
		int i;
		for (i=0; i<m_ClientEntries.GetSize(); i++)
		{
			clientEntries[i] = m_ClientEntries[i];
		}
		for (;i<MAXIMUM_WAIT_OBJECTS; i++)
		{
			clientEntries[i].pClient = NULL;
		}

		return m_ClientEntries.GetSize();
	}

protected:

	void RemoveAllClients() throw()
	{
		ATLASSERT(m_hWaitHandles.GetSize() == m_ClientEntries.GetSize());

		int nLen = m_hWaitHandles.GetSize();
		for (int i = 0; i < nLen; i++)
		{
			WorkerClientEntry& entry = m_ClientEntries[i];
			if (!entry.pClient || FAILED(entry.pClient->CloseHandle(m_hWaitHandles[i])))
				CloseHandle(m_hWaitHandles[i]);
		}
		m_hWaitHandles.RemoveAll();
		m_ClientEntries.RemoveAll();
	}

	DWORD WorkerThreadProc()
	{
		// Make sure the object has been initialized
		ATLASSERT(m_hRefreshComplete != NULL);

		HANDLE handles[MAXIMUM_WAIT_OBJECTS];
		WorkerClientEntry clientEntries[MAXIMUM_WAIT_OBJECTS];
		int nSize = 0;

		nSize = CopyHandles(handles);
		CopyClientEntries(clientEntries);

		// tell the main thread we're done copying
		SetEvent(m_hRefreshComplete);

		while (TRUE)
		{
			DWORD dwRet = WaitForMultipleObjects(nSize, handles, FALSE, INFINITE);
			// check for shutdown
			if (dwRet == WAIT_OBJECT_0)
				return 0;
			else if (dwRet == WAIT_OBJECT_0+1)	// check for refresh
			{
				nSize = CopyHandles(handles);
				CopyClientEntries(clientEntries);

				// tell the main thread we're done copying
				SetEvent(m_hRefreshComplete);
				continue;
			}
			else if (dwRet > WAIT_OBJECT_0 && dwRet < WAIT_OBJECT_0 + nSize)
			{
				// execute the approriate client
				WorkerClientEntry& entry = clientEntries[dwRet - WAIT_OBJECT_0];

				// We ignore the error code because nothing useful can be done with it in this
				// implementation
				entry.pClient->Execute(entry.dwParam, handles[dwRet - WAIT_OBJECT_0]);
			}
			else
			{
				// this probably means an invalid handle was added
				ATLASSERT(FALSE);
				return 1;
			}
		}

		return 0;
	}

#pragma warning(push)
#pragma warning(disable: 4702) // Unreachable code.

	static DWORD WINAPI _WorkerThreadProc(CWorkerThread *pThis) throw()
	{
		_ATLTRY
		{
			return pThis->WorkerThreadProc();
		}
		_ATLCATCHALL()
		{
			//
			// uncaught exception thrown from a worker thread client
			//

			ATLASSERT( FALSE );
			return 1;
		}
	}

#pragma warning(pop)

}; // class CWorkerThread

// Use CNoWorkerThread as a template argument for classes
// that need a worker thread type as a template argument but
// don't require the services of a worker thread. An example
// would be CDllCache (atlutil.h) when you want to create a
// CDllCache with no sweeper thread.
class CNoWorkerThread
{
public:
	DWORD GetThreadId() throw()
	{
		return 0;
	}

	HANDLE GetThreadHandle() throw()
	{
		return NULL;
	}

	HRESULT Initialize() throw()
	{
		return S_OK;
	}

	HRESULT AddHandle(HANDLE /*hObject*/, IWorkerThreadClient * /*pClient*/, DWORD_PTR /*dwParam*/) throw()
	{
		return S_OK;
	}


	HRESULT AddTimer(DWORD /*dwInterval*/, IWorkerThreadClient * /*pClient*/, DWORD_PTR /*dwParam*/, HANDLE * /*phTimer*/) throw()
	{
		return S_OK;
	}

	HRESULT RemoveHandle(HANDLE /*hObject*/) throw()
	{
		return S_OK;
	}

	HRESULT Shutdown(DWORD dwWait=ATL_WORKER_THREAD_WAIT) throw()
	{
		(dwWait);
		return S_OK;
	}
}; // CNoWorkerThread

class CBrowserCapsSvc : public IBrowserCapsSvc, 
						public CComObjectRootEx<CComSingleThreadModel>
{
public:
	virtual ~CBrowserCapsSvc()
	{
	}
	
	BEGIN_COM_MAP(CBrowserCapsSvc)
		COM_INTERFACE_ENTRY(IBrowserCapsSvc)
	END_COM_MAP()

	HRESULT GetCaps(IHttpServerContext * pContext, IBrowserCaps ** ppOut)
	{
		if (!pContext)
			return E_POINTER;

		if (!ppOut)
			return E_POINTER;

		*ppOut = NULL;

		char szUserAgent[256];
		DWORD dwSize = sizeof(szUserAgent);
		if (!pContext->GetServerVariable("HTTP_USER_AGENT", szUserAgent, &dwSize))
			return E_FAIL;

		return GetCapsUserAgent(CComBSTR(szUserAgent), ppOut);
	}

	HRESULT GetCapsUserAgent(BSTR bstrAgent, IBrowserCaps ** ppOut)
	{
		if (bstrAgent == NULL || ppOut == NULL)
			return E_POINTER;

		*ppOut = NULL;

		BrowserCaps* pCaps = NULL;

		_ATLTRY
		{
			CW2CT szUserAgent(bstrAgent);

			if (!m_mapAgent.Lookup(szUserAgent, pCaps))
			{
				pCaps = NULL;
				for (size_t i=0; i<m_caps.GetCount(); i++)
				{
					BrowserCaps& caps = m_caps[i];
					if (IsEqualAgentString(caps.m_strUserAgent, szUserAgent))
					{
						pCaps = &caps;
						break;
					}
				}
			}
		}
		_ATLCATCHALL()
		{
			return E_FAIL;
		}

		if (pCaps != NULL)
		{
			CComObjectNoLock<CBrowserCaps> *pRet = NULL;
			ATLTRY(pRet = new CComObjectNoLock<CBrowserCaps>);
			if (!pRet)
				return E_OUTOFMEMORY;
			pRet->AddRef();

			HRESULT hr = pRet->Initialize(pCaps);
			if (FAILED(hr))
			{
				pRet->Release();
				return hr;
			}

			*ppOut = pRet;
			return S_OK;
		}

		return E_FAIL;
	}

	HRESULT Initialize(HINSTANCE hInstance)
	{
		// tries loading browscap.ini from the same directory as the module
		if (hInstance != NULL)
		{
			_ATLTRY
			{
				CPath strBrowscapPath;

				LPTSTR sz = strBrowscapPath.m_strPath.GetBuffer(MAX_PATH);
				UINT nChars = ::GetModuleFileName(hInstance, sz, MAX_PATH);
				strBrowscapPath.m_strPath.ReleaseBuffer(nChars);
				if (nChars != 0 &&
					nChars != MAX_PATH &&
					strBrowscapPath.RemoveFileSpec())
				{
					strBrowscapPath += _T("\\browscap.ini");
					if (SUCCEEDED(Load(strBrowscapPath)))
						return S_OK;
				}
			}
			_ATLCATCHALL()
			{
				return E_FAIL;
			}
		}

		// falls back to the system browscap.ini if previous Load failed
		return Load();
	}

	HRESULT Uninitialize()
	{
		Clear();
		return S_OK;
	}

private:
	static bool IsEqualAgentString(LPCTSTR szPattern, LPCTSTR szInput)
	{
		while (*szPattern && *szInput && (*szPattern == *szInput || *szPattern == '?'))
		{
			szPattern++;
			szInput++;
		}

		if (*szPattern == *szInput)
		{
			return true;
		}

		if (*szPattern == '*')
		{
			szPattern++;
			if (!*szPattern)
			{
				return true;
			}
			while(*szInput)
			{
				if (IsEqualAgentString(szPattern, szInput))
				{
					return true;
				}

				szInput++;
			}
		}

		return false;
	}

	HRESULT Load(LPCTSTR szPath = NULL)
	{
		_ATLTRY
		{
			Clear();

			CString strBrowscapPath(szPath);

			// use default load path if a path isn't specified
			if (strBrowscapPath.IsEmpty())
			{
				LPTSTR sz = strBrowscapPath.GetBuffer(MAX_PATH);
				UINT nChars = ::GetSystemDirectory(sz, MAX_PATH);
				strBrowscapPath.ReleaseBuffer(nChars);
				if (nChars == 0 || nChars == MAX_PATH)
					return E_FAIL;

				strBrowscapPath += _T("\\inetsrv\\browscap.ini");
			}

			// 10381 is needed as of Win2k SP1. it will grow as new browsers are added
			size_t nCurrent = 16384;
			CHeapPtr<TCHAR> data;

			if (!data.Allocate(nCurrent))
				return E_OUTOFMEMORY;

			// load the list of all the user agents
			bool bRetrieved = false;

			do
			{
				DWORD dwRetrieved = ::GetPrivateProfileSectionNames(data, (DWORD) nCurrent, strBrowscapPath);
				if (dwRetrieved == 0)
					return AtlHresultFromWin32(ERROR_FILE_NOT_FOUND);
				else if (dwRetrieved < nCurrent-2)
					bRetrieved = true;
				else if (!data.Reallocate(nCurrent *= 2))
					return E_OUTOFMEMORY;
			} while (!bRetrieved);

			// figure out how many user agents there are
			// and set them in the structure
			LPTSTR sz = data;
			int nSections = 0;
			while (*sz)
			{
				nSections++;
				sz += (lstrlen(sz)+1);
			}

			if (!m_caps.SetCount(nSections))
				return E_OUTOFMEMORY;

			sz = data;
			nSections = 0;
			while (*sz)
			{
				BrowserCaps& caps = m_caps[nSections++];
				caps.m_strUserAgent = sz;
				m_mapAgent[caps.m_strUserAgent] = &caps;
				sz += (caps.m_strUserAgent.GetLength()+1);
			}

			// for each user agent, load the properties
			for (size_t i=0; i<m_caps.GetCount(); i++)
			{
				bool bRetrieved = false;
				BrowserCaps& caps = m_caps[i];
				caps.m_pParent = NULL;

				do
				{
					DWORD dwRetrieved = ::GetPrivateProfileSection(caps.m_strUserAgent, data, (DWORD) nCurrent, strBrowscapPath);
					if (dwRetrieved == 0)
						return AtlHresultFromWin32(ERROR_FILE_NOT_FOUND);
					else if (dwRetrieved < nCurrent-2)
						bRetrieved = true;
					else if (!data.Reallocate(nCurrent *= 2))
						return E_OUTOFMEMORY;
				} while (!bRetrieved);

				sz = data;
				while (*sz)
				{
					CString str = sz;
					int nChar = str.Find('=');
					if (nChar != -1)
					{
						CString strPropName = str.Left(nChar);
						CString strPropVal = str.Mid(nChar+1);
						strPropName.Trim();
						strPropVal.Trim();
						caps.m_props.SetAt(strPropName, strPropVal);

						// if it's the parent property, set up the parent pointer
						if (strPropName.CompareNoCase(_T("parent")) == 0)
						{
							BrowserCaps* pParent = NULL;
							if (m_mapAgent.Lookup(strPropVal, pParent))
								caps.m_pParent = pParent;
						}
					}
					sz += (str.GetLength()+1);
				}
			}
		}
		_ATLCATCHALL()
		{
			return E_FAIL;
		}

		return S_OK;
	}

	void Clear()
	{
		m_caps.RemoveAll();
		m_mapAgent.RemoveAll();
	}

	friend class CBrowserCaps;

	struct BrowserCaps
	{
		CString m_strUserAgent; // user agent string to match against (with wildcards)
		BrowserCaps* m_pParent;
		CAtlMap<CString, CString, CStringElementTraitsI<CString>, CStringElementTraits<CString> > m_props;
	};

	// map from UserAgent string to caps
	// used for non-wildcard lookup and parent lookup
	CAtlMap<CString, BrowserCaps*, CStringElementTraits<CString> > m_mapAgent;

	// all of the caps
	CAtlArray<BrowserCaps> m_caps;

	class CBrowserCaps : public IBrowserCaps, public CComObjectRootEx<CComSingleThreadModel>
	{
	public:

		BEGIN_COM_MAP(CBrowserCaps)
			COM_INTERFACE_ENTRY(IBrowserCaps)
		END_COM_MAP()

		CBrowserCaps()
		{
		}

		HRESULT Initialize(CBrowserCapsSvc::BrowserCaps * pCaps)
		{
			m_pCaps = pCaps;
			return S_OK;
		}

		HRESULT GetPropertyString(BSTR bstrProperty, BSTR * pbstrOut)
		{
			_ATLTRY
			{
				ATLASSERT(m_pCaps);
				if (!m_pCaps)
					return E_UNEXPECTED;

				if (!pbstrOut)
					return E_POINTER;

				*pbstrOut = NULL;

				CString strName(bstrProperty);
				CString strVal;

				CBrowserCapsSvc::BrowserCaps * pCaps = m_pCaps;
				while (pCaps)
				{
					if (pCaps->m_props.Lookup(strName, strVal))
					{
						CComBSTR bstrVal(strVal);
						*pbstrOut = bstrVal.Detach();
						return S_OK;
					}

					pCaps = pCaps->m_pParent;
				}

				return S_FALSE;
			}
			_ATLCATCHALL()
			{
				return E_FAIL;
			}
		}

		HRESULT GetBooleanPropertyValue(BSTR bstrProperty, BOOL* pbOut)
		{
			if (!pbOut)
				return E_POINTER;

			CComBSTR bstrOut;
			HRESULT hr = GetPropertyString(bstrProperty, &bstrOut);
			if (FAILED(hr) || S_FALSE == hr)
				return hr;

			if (_wcsicmp(bstrOut, L"true") == 0)
				*pbOut = TRUE;
			else
				*pbOut = FALSE;

			return S_OK;
		}

		HRESULT GetBrowserName(BSTR * pbstrName)
		{
			return GetPropertyString(CComBSTR(L"browser"), pbstrName);
		}

		HRESULT GetPlatform(BSTR * pbstrPlatform)
		{
			return GetPropertyString(CComBSTR(L"platform"), pbstrPlatform);
		}

		HRESULT GetVersion(BSTR * pbstrVersion)
		{
			return GetPropertyString(CComBSTR(L"version"), pbstrVersion);
		}

		HRESULT GetMajorVer(BSTR * pbstrMajorVer)
		{
			return GetPropertyString(CComBSTR(L"majorver"), pbstrMajorVer);
		}

		HRESULT GetMinorVer(BSTR * pbstrMinorVer)
		{
			return GetPropertyString(CComBSTR(L"minorver"), pbstrMinorVer);
		}

		HRESULT SupportsFrames(BOOL* pbFrames)
		{
			return GetBooleanPropertyValue(CComBSTR(L"frames"), pbFrames);
		}

		HRESULT SupportsTables(BOOL* pbTables)
		{
			return GetBooleanPropertyValue(CComBSTR(L"tables"), pbTables);
		}
		HRESULT SupportsCookies(BOOL* pbCookies)
		{
			return GetBooleanPropertyValue(CComBSTR(L"cookies"), pbCookies);
		}
		HRESULT SupportsBackgroundSounds(BOOL* pbBackgroundSounds)
		{
			return GetBooleanPropertyValue(CComBSTR(L"backgroundsounds"), pbBackgroundSounds);
		}
		HRESULT SupportsVBScript(BOOL* pbVBScript)
		{
			return GetBooleanPropertyValue(CComBSTR(L"vbscript"), pbVBScript);
		}
		HRESULT SupportsJavaScript(BOOL* pbJavaScript)
		{
			return GetBooleanPropertyValue(CComBSTR(L"javascript"), pbJavaScript);
		}
		HRESULT SupportsJavaApplets(BOOL* pbJavaApplets)
		{
			return GetBooleanPropertyValue(CComBSTR(L"javaapplets"), pbJavaApplets);
		}
		HRESULT SupportsActiveXControls(BOOL* pbActiveXControls)
		{
			return GetBooleanPropertyValue(CComBSTR(L"ActiveXControls"), pbActiveXControls);
		}
		HRESULT SupportsCDF(BOOL* pbCDF)
		{
			return GetBooleanPropertyValue(CComBSTR(L"CDF"), pbCDF);
		}
		HRESULT SupportsAuthenticodeUpdate(BOOL* pbAuthenticodeUpdate)
		{
			return GetBooleanPropertyValue(CComBSTR(L"AuthenticodeUpdate"), pbAuthenticodeUpdate);
		}
		HRESULT IsBeta(BOOL* pbIsBeta)
		{
			return GetBooleanPropertyValue(CComBSTR(L"beta"), pbIsBeta);
		}
		HRESULT IsCrawler(BOOL* pbIsCrawler)
		{
			return GetBooleanPropertyValue(CComBSTR(L"Crawler"), pbIsCrawler);
		}
		HRESULT IsAOL(BOOL* pbIsAOL)
		{
			return GetBooleanPropertyValue(CComBSTR(L"AOL"), pbIsAOL);
		}
		HRESULT IsWin16(BOOL* pbIsWin16)
		{
			return GetBooleanPropertyValue(CComBSTR(L"Win16"), pbIsWin16);
		}
		HRESULT IsAK(BOOL* pbIsAK)
		{
			return GetBooleanPropertyValue(CComBSTR(L"AK"), pbIsAK);
		}
		HRESULT IsSK(BOOL* pbIsSK)
		{
			return GetBooleanPropertyValue(CComBSTR(L"SK"), pbIsSK);
		}
		HRESULT IsUpdate(BOOL* pbIsUpdate) 
		{
			return GetBooleanPropertyValue(CComBSTR(L"Update"), pbIsUpdate);
		}

	private:
		CBrowserCapsSvc::BrowserCaps * m_pCaps;
	};
};

// Copies a CString into a null-terminated string.
// pdwDestLen on input is the size of the buffer in characters (including the null)
// On success, pdwDestLen contains the length of the string in characters (not including the null)
// On failure, pdwDestLen contains the length of the string including the null.
template <class StringType>
inline BOOL CopyCString(const StringType& str, typename StringType::PXSTR szDest, DWORD *pdwDestLen) throw()
{
	if (!pdwDestLen)
		return FALSE;

	DWORD dwLen = str.GetLength();
	if (!szDest || *pdwDestLen < (dwLen + 1))
	{
		*pdwDestLen = dwLen + 1;
		return FALSE;
	}

	StringType::PCXSTR szBuffer = str;
	if (szBuffer)
	{
		memcpy(szDest, szBuffer, (dwLen+1) * sizeof(StringType::XCHAR));
		*pdwDestLen = dwLen;
		return TRUE;
	}

	return FALSE;
}

// Call this function to convert from a SYSTEMTIME
// structure to an Http-date as defined in rfc2616
inline void SystemTimeToHttpDate(const SYSTEMTIME& st, CStringA &strTime)
{
	static LPCSTR szDays[] = { "Sun", "Mon", "Tue",
		"Wed", "Thu", "Fri", "Sat" };
	static LPCSTR szMonth[] = { "Jan", "Feb", "Mar", "Apr",
		"May", "Jun", "Jul", "Aug", "Sep", 
		"Oct", "Nov", "Dec" };

	strTime.Format("%s, %02d %s %d %02d:%02d:%02d GMT",
		szDays[st.wDayOfWeek], st.wDay, szMonth[st.wMonth-1], st.wYear,
		st.wHour, st.wMinute, st.wSecond);
}

// RGBToHtml - Converts a COLORREF to a color that can be used in HTML.
//             Eg. RGB(11,22,33) would be converted to #112233
// color:    The color to convert.
// pbOut:    The output buffer that will hold the the resulting color.
// nBuffer:	 Specifies the number of bytes in pbOut.
bool inline RGBToHtml(COLORREF color, LPTSTR pbOut, long nBuffer)
{
	// make sure the buffer is big enough
	if (nBuffer < (8 * sizeof(TCHAR)))
		return false;

	wsprintf(pbOut, _T("#%0.2x%0.2x%0.2x"),
		GetRValue(color), GetGValue(color), GetBValue(color));
	return true;
}

inline int AsciiStricmp (
		const char * dst,
		const char * src
		) throw()
{
		int f, l;

		do {
			if ( ((f = (unsigned char)(*(dst++))) >= 'A') &&
				 (f <= 'Z') )
				f -= 'A' - 'a';
			if ( ((l = (unsigned char)(*(src++))) >= 'A') &&
				 (l <= 'Z') )
				l -= 'A' - 'a';
		} while ( f && (f == l) );

		return(f - l);
}

inline int AsciiStrnicmp (
		const char * first,
		const char * last,
		size_t count
		) throw()
{
		int f, l;

		do {

			if ( ((f = (unsigned char)(*(first++))) >= 'A') &&
				 (f <= 'Z') )
				f -= 'A' - 'a';

			if ( ((l = (unsigned char)(*(last++))) >= 'A') &&
				 (l <= 'Z') )
				l -= 'A' - 'a';

		} while ( --count && f && (f == l) );

		return ( f - l );
}

} // namespace ATL

#pragma warning( pop )

#endif // __ATLUTIL_H__
