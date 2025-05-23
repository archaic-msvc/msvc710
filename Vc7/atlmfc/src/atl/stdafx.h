// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the	
// Active Template Library product.


#define _WIN32_WINNT 0x0400
#define _ATL_FREE_THREADED
#define _ATL_DLL_IMPL

#include <atlbase.h>

extern CComModule _Module;

#ifdef _DEBUG
#include <atlbase.inl>
#endif

#include <atlcom.h>
#include <statreg.h>
#include <atlwin.h>
#include <atlctl.h>
#include <atlconv.h>
#include <atlhost.h>
