/***
*mbsbtype.c - Return type of byte within a string (MBCS)
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Return type of byte within a string (MBCS)
*
*******************************************************************************/

#ifdef _MBCS

#include <mtdll.h>
#include <cruntime.h>
#include <mbdata.h>
#include <mbstring.h>
#include <mbctype.h>


#define _MBBTYPE(p,c)   _mbbtype(p,c)

/***
* _mbsbtype - Return type of byte within a string
*
*Purpose:
*       Test byte within a string for MBCS char type.
*       This function requires the start of the string because
*       context must be taken into account.
*
*Entry:
*       const unsigned char *string = pointer to string
*       size_t len = position of the char in string
*
*Exit:
*       returns one of the following values:
*
*       _MBC_LEAD      = if 1st byte of MBCS char
*       _MBC_TRAIL     = if 2nd byte of MBCS char
*       _MBC_SINGLE    = valid single byte char
*
*       _MBC_ILLEGAL   = if illegal char
*
*Exceptions:
*       returns _MBC_ILLEGAL if len is bigger than string length
*
*******************************************************************************/

int __cdecl _mbsbtype(
        const unsigned char *string,
        size_t len
        )
{
#ifdef _MT
        pthreadmbcinfo ptmbci = _getptd()->ptmbcinfo;

        if ( ptmbci != __ptmbcinfo )
            ptmbci = __updatetmbcinfo();

        return __mbsbtype_mt(ptmbci, string, len);
}

int __cdecl __mbsbtype_mt(
        pthreadmbcinfo ptmbci,
        const unsigned char *string,
        size_t len
        )
{
#endif  /* _MT */
        int chartype;

#ifdef _MT
        if ( _ISNOTMBCP_MT(ptmbci) )
#else  /* _MT */
        if ( _ISNOTMBCP )
#endif  /* _MT */
            return _MBC_SINGLE;

        chartype = _MBC_ILLEGAL;

        do {
            if (*string == '\0')
                return(_MBC_ILLEGAL);
#ifdef _MT
            chartype = __mbbtype_mt(ptmbci, *string++, chartype);
#else  /* _MT */
            chartype = _mbbtype(*string++, chartype);
#endif  /* _MT */
        }  while (len--);

        return(chartype);
}

#endif  /* _MBCS */
