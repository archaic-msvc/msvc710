/***
*handler.cpp - defines C++ setHandler routine
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Defines C++ setHandler routine.
*
*******************************************************************************/

#include <stddef.h>
#include <internal.h>
#include <cruntime.h>
#include <mtdll.h>
#include <process.h>
#include <new.h>
#include <dbgint.h>

/* pointer to old-style C++ new handler */
_PNH _pnhHeap;

/***
*_PNH _set_new_handler(_PNH pnh) - set the new handler
*
*Purpose:
*       _set_new_handler is different from the ANSI C++ working standard definition
*       of set_new_handler.  Therefore it has a leading underscore in its name.
*
*Entry:
*       Pointer to the new handler to be installed.
*
*Return:
*       Previous new handler
*
*******************************************************************************/
_PNH __cdecl _set_new_handler(
        _PNH pnh
        )
{
        _PNH pnhOld;

        /* lock the heap */
        _mlock(_HEAP_LOCK);

        pnhOld = _pnhHeap;
        _pnhHeap = pnh;

        /* unlock the heap */
        _munlock(_HEAP_LOCK);

        return(pnhOld);
}


/***
*_PNH _query_new_handler(void) - query value of new handler
*
*Purpose:
*       Obtain current new handler value.
*
*Entry:
*       None
*
*       WARNING: This function is OBSOLETE. Use _query_new_ansi_handler instead.
*
*Return:
*       Currently installed new handler
*
*******************************************************************************/
_PNH __cdecl _query_new_handler (
        void
        )
{
        return _pnhHeap;
}


/***
*int _callnewh - call the appropriate new handler
*
*Purpose:
*       Call the appropriate new handler.
*
*Entry:
*       None
*
*Return:
*       1 for success
*       0 for failure
*       may throw bad_alloc
*
*******************************************************************************/
extern "C" int __cdecl _callnewh(size_t size)
{
        {
            _PNH pnh = _pnhHeap;

            if ( (pnh == NULL) || ((*pnh)(size) == 0) )
                return 0;
        }
        return 1;
}
