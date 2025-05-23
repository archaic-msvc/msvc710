/***
*ismbslead.c - True _ismbslead function
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Contains the function _ismbslead, which is a true context-sensitive
*       MBCS lead-byte function.  While much less efficient than _ismbblead,
*       it is also much more sophisticated, in that it determines whether a
*       given sub-string pointer points to a lead byte or not, taking into
*       account the context in the string.
*
*******************************************************************************/

#ifdef _MBCS

#include <mtdll.h>
#include <cruntime.h>
#include <stddef.h>
#include <mbdata.h>
#include <mbctype.h>
#include <mbstring.h>


/***
* int _ismbslead(const unsigned char *string, const unsigned char *current);
*
*Purpose:
*
*       _ismbslead - Check, in context, for MBCS lead byte
*
*Entry:
*       unsigned char *string   - ptr to start of string or previous known lead byte
*       unsigned char *current  - ptr to position in string to be tested
*
*Exit:
*       TRUE    : -1
*       FALSE   : 0
*
*Exceptions:
*
*******************************************************************************/

int __cdecl _ismbslead(
        const unsigned char *string,
        const unsigned char *current
        )
{
#ifdef _MT
        pthreadmbcinfo ptmbci = _getptd()->ptmbcinfo;

        if ( ptmbci != __ptmbcinfo )
            ptmbci = __updatetmbcinfo();

        return __ismbslead_mt(ptmbci, string, current);
}

int __cdecl __ismbslead_mt(
        pthreadmbcinfo ptmbci,
        const unsigned char *string,
        const unsigned char *current
        )
{
#endif  /* _MT */

#ifdef _MT
        if ( _ISNOTMBCP_MT(ptmbci) )
#else  /* _MT */
        if ( _ISNOTMBCP )
#endif  /* _MT */
            return 0;

        while (string <= current && *string) {
#ifdef _MT
            if ( __ismbblead_mt(ptmbci, (*string)) ) {
#else  /* _MT */
            if ( _ismbblead((*string)) ) {
#endif  /* _MT */
                if (string++ == current)        /* check lead byte */
                    return -1;
                if (!(*string))
                    return 0;
            }
            ++string;
        }

        return 0;
}

#endif  /* _MBCS */
