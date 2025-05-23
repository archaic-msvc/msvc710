/***
*wperror.c - print system error message (wchar_t version)
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines _wperror() - print wide system error message
*       System error message are indexed by errno.
*
*******************************************************************************/


#include <cruntime.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syserr.h>
#include <mtdll.h>
#include <io.h>
#include <dbgint.h>
#include <limits.h>

/***
*void _wperror(wmessage) - print system error message
*
*Purpose:
*       prints user's error message, then follows it with ": ", then the system
*       error message, then a newline.  All output goes to stderr.  If user's
*       message is NULL or a null string, only the system error message is
*       printer.  If errno is weird, prints "Unknown error".
*
*Entry:
*       const wchar_t *wmessage - users message to prefix system error message
*
*Exit:
*       Prints message; no return value.
*
*Exceptions:
*
*******************************************************************************/

void __cdecl _wperror (
        const wchar_t *wmessage
        )
{
        int fh = 2;
        size_t size;
        char *amessage;

        /* convert WCS string into ASCII string */

        if ( wmessage && *wmessage )
        {
            size = (size_t)(wcstombs(NULL,wmessage,INT_MAX) + 1);

            if ( size==0 || (amessage = (char *)_malloc_crt(size * sizeof(char))) == NULL )
                return;

            if ( 0 >= wcstombs(amessage, wmessage, size) )
            {
                _free_crt(amessage);
                return;
            }
        }
        else
            amessage = NULL;

#ifdef _MT
        _lock_fh( fh );         /* acquire file handle lock */
        __try {
#endif  /* _MT */

        if ( amessage )
        {
                _write_lk(fh,(char *)amessage,(unsigned)strlen(amessage));
                _write_lk(fh,": ",2);
        }

        _free_crt(amessage);    /* note: freeing NULL is legal and benign */

        amessage = _sys_err_msg( errno );
        _write_lk(fh,(char *)amessage,(unsigned)strlen(amessage));
        _write_lk(fh,"\n",1);

#ifdef _MT
        }
        __finally {
            _unlock_fh( fh );   /* release file handle lock */
        }
#endif  /* _MT */
}

