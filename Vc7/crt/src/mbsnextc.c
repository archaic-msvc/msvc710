/***
*mbsnextc.c - Get the next character in an MBCS string.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       To return the value of the next character in an MBCS string.
*
*******************************************************************************/

#ifdef _MBCS

#include <cruntime.h>
#include <mbdata.h>
#include <mbctype.h>
#include <mbstring.h>


/***
*_mbsnextc:  Returns the next character in a string.
*
*Purpose:
*       To return the value of the next character in an MBCS string.
*       Does not advance pointer to the next character.
*
*Entry:
*       unsigned char *s = string
*
*Exit:
*       unsigned int next = next character.
*
*Exceptions:
*
*******************************************************************************/

unsigned int __cdecl _mbsnextc(
        const unsigned char *s
        )
{
        unsigned int  next = 0;

        /* don't skip forward 2 if the leadbyte is followed by EOS (dud string)
           also don't assert as we are too low-level
        */
        if ( _ismbblead(*s) && s[1]!='\0')
            next = ((unsigned int) *s++) << 8;

        next += (unsigned int) *s;

        return(next);
}

#endif  /* _MBCS */
