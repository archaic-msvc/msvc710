// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"

#ifdef AFX_OLE5_SEG
#pragma code_seg(AFX_OLE5_SEG)
#endif

// Note: because of the nature of these functions, it is not possible
//  to create a 'C' or 'C++' version of them.  These functions are used
//  for the lowest level of the OLE IDispatch implementation, and need
//  to be ported to each supported platform.

extern "C" {

/////////////////////////////////////////////////////////////////////////////
// Intel 386 version

#ifdef _X86_

__declspec(naked) void AFXAPI
_AfxDispatchCall(AFX_PMSG /*pfn*/, void* /*pArgs*/, UINT /*nSizeArgs*/)
{
	_asm
	{
		pop     edx         // edx = return address
		pop     eax         // eax = pfn
		pop     ecx         // ecx = pArgs
		add     ecx,[esp]   // ecx += nSizeArgs (=scratch area)
		mov     [ecx],edx   // scratch[0] = return address
		sub     ecx,[esp]   // ecx = pArgs (again)
		mov     esp,ecx     // esp = pArgs (usually already correct)
		pop     ecx         // ecx = this pointer (the CCmdTarget*)
		call    eax         // call member function
		ret                 // esp[0] should = scratch[0] = return address
	}
}
#endif // _X86_

/////////////////////////////////////////////////////////////////////////////
// MIPS R4000 version

#ifdef _MIPS_

extern "C" void _asm(char *, ...);
void AFXAPI
_AfxDispatchCall(AFX_PMSG /*pfn*/, void* /*pArgs*/, UINT /*nSizeArgs*/)
{
	_asm("addiu     %sp,%a1,0x0");      // sp = pArgs
	_asm("addiu     %t6,%a0,0x0");      // t6 = pfn (save it)
	_asm("lw        %a0,0x0(%sp)");     // a0 = param0
	_asm("lw        %a1,0x4(%sp)");     // a1 = param1
	_asm("lw        %a2,0x8(%sp)");     // a2 = param2
	_asm("lw        %a3,0xc(%sp)");     // a3 = param3
	_asm("j         %t6");              // ip = pfn (jump to target function)
}

#endif // _MIPS_

/////////////////////////////////////////////////////////////////////////////
// DEC Alpha AXP version

#ifdef _ALPHA_

// Note: ALPHA version is in src\alpha\olecall_.s

// The ALPHA compiler does not support inline assembly, so it
//  must be build separately with the ASAXP assembler.

#endif // _ALPHA_

#ifdef _IA64_  //REVIEW
void AFXAPI
_AfxDispatchCall(AFX_PMSG, void*, UINT)
{
}
#endif

} // end extern "C" block

/////////////////////////////////////////////////////////////////////////////
