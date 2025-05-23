/***
*toupper.c - convert character to uppercase
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Defines function versions of _toupper() and toupper().
*
*******************************************************************************/


#include <cruntime.h>
#include <ctype.h>
#include <stddef.h>
#include <locale.h>
#include <setlocal.h>
#include <mtdll.h>
#include <awint.h>

/* remove macro definitions of _toupper() and toupper()
 */
#undef  _toupper
#undef  toupper

/* define function-like macro equivalent to _toupper()
 */
#define mkupper(c)  ( (c)-'a'+'A' )

/***
*int _toupper(c) - convert character to uppercase
*
*Purpose:
*       _toupper() is simply a function version of the macro of the same name.
*
*Entry:
*       c - int value of character to be converted
*
*Exit:
*       returns int value of uppercase representation of c
*
*Exceptions:
*
*******************************************************************************/

int __cdecl _toupper (
        int c
        )
{
        return(mkupper(c));
}


/***
*int toupper(c) - convert character to uppercase
*
*Purpose:
*       toupper() is simply a function version of the macro of the same name.
*
*Entry:
*       c - int value of character to be converted
*
*Exit:
*       if c is a lower case letter, returns int value of uppercase
*       representation of c. otherwise, it returns c.
*
*Exceptions:
*
*******************************************************************************/


int __cdecl toupper (
    int c
    )
{

#ifdef _MT
        pthreadlocinfo ptloci = _getptd()->ptlocinfo;

        if ( ptloci != __ptlocinfo )
            ptloci = __updatetlocinfo();

        return __toupper_mt(ptloci, c);
}


/***
*int __toupper_mt(ptloci, c) - convert character to uppercase
*
*Purpose:
*       Multi-thread function! Non-locking version of toupper.
*
*Entry:
*
*Exit:
*
*Exceptions:
*
*******************************************************************************/


int __cdecl __toupper_mt (
        pthreadlocinfo ptloci,
        int c
        )
{

#endif  /* _MT */

        int size;
        unsigned char inbuffer[3];
        unsigned char outbuffer[3];

#ifndef _MT
        if ( __lc_handle[LC_CTYPE] == _CLOCALEHANDLE ||
             (__lc_clike && (unsigned)c <= 0x7f))
            return __ascii_toupper(c);
#else  /* _MT */
        if ( ptloci->lc_handle[LC_CTYPE] == _CLOCALEHANDLE ||
             (ptloci->lc_clike && (unsigned)c <= 0x7f))
            return __ascii_toupper(c);
#endif  /* _MT */

        /* if checking case of c does not require API call, do it */
        if ( (unsigned)c < 256 ) {
#ifdef _MT
            if ( !__islower_mt(ptloci, c) )
#else  /* _MT */
            if ( !islower(c) )
#endif  /* _MT */
            {
                return c;
            }
        }

        /* convert int c to multibyte string */
#ifdef _MT
        if ( __isleadbyte_mt(ptloci, c >> 8 & 0xff) ) {
#else  /* _MT */
        if ( isleadbyte(c >> 8 & 0xff) ) {
#endif  /* _MT */
            inbuffer[0] = (c >> 8 & 0xff); /* put lead-byte at start of str */
            inbuffer[1] = (unsigned char)c;
            inbuffer[2] = 0;
            size = 2;
        } else {
            inbuffer[0] = (unsigned char)c;
            inbuffer[1] = 0;
            size = 1;
        }

        /* convert wide char to lowercase */
#ifdef _MT
        if ( 0 == (size = __crtLCMapStringA( ptloci->lc_handle[LC_CTYPE],
#else  /* _MT */
        if ( 0 == (size = __crtLCMapStringA( __lc_handle[LC_CTYPE],
#endif  /* _MT */
                                             LCMAP_UPPERCASE,
                                             inbuffer,
                                             size,
                                             outbuffer,
                                             3,
#ifdef _MT
                                             ptloci->lc_codepage,
#else  /* _MT */
                                             __lc_codepage,
#endif  /* _MT */
                                             TRUE)) )
        {
            return c;
        }

        /* construct integer return value */
        if (size == 1)
            return ((int)outbuffer[0]);
        else
            return ((int)outbuffer[1] | ((int)outbuffer[0] << 8));

}
