/***
*mbsdec.c - Move MBCS string pointer backward one charcter.
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Move MBCS string pointer backward one character.
*
*******************************************************************************/

#ifdef _MBCS

#include <mtdll.h>
#include <cruntime.h>
#include <mbdata.h>
#include <mbstring.h>
#include <mbctype.h>
#include <stddef.h>


/***
*_mbsdec - Move MBCS string pointer backward one charcter.
*
*Purpose:
*       Move the supplied string pointer backwards by one
*       character.  MBCS characters are handled correctly.
*
*Entry:
*       const unsigned char *string = pointer to beginning of string
*       const unsigned char *current = current char pointer (legal MBCS boundary)
*
*Exit:
*       Returns pointer after moving it.
*       Returns NULL if string >= current.
*
*Exceptions:
*
*******************************************************************************/

unsigned char * __cdecl _mbsdec(
        const unsigned char *string,
        const unsigned char *current
        )
{
        const unsigned char *temp;
#ifdef _MT
        pthreadmbcinfo ptmbci = _getptd()->ptmbcinfo;

        if ( ptmbci != __ptmbcinfo )
            ptmbci = __updatetmbcinfo();
#endif  /* _MT */

        if (string >= current)
                return(NULL);

#ifdef _MT
        if ( _ISNOTMBCP_MT(ptmbci) )
#else  /* _MT */
        if ( _ISNOTMBCP )
#endif  /* _MT */
            return (unsigned char *)--current;

        temp = current - 1;

/* There used to be an optimisation here:
 *
 *  If (current-1) returns true from _ismbblead, it is a trail byte, because
 *  current is a known character start point, and so current-1 would have to be a
 *  legal single byte MBCS character, which a lead byte is not.  Therefore, if so,
 *  return (current-2) because it must be the trailbyte's lead.
 *
 *      if ( _ismbblead(*temp) )
 *           return (unsigned char *)(temp - 1);
 *
 * But this is not a valid optimisation if you want to cope correctly with an
 * MBCS string which is terminated by a leadbyte and a 0 byte, when you are passed
 * an initial position pointing to the \0 at the end of the string.
 *
 * This optimisation is also invalid if you are passed a pointer to half-way
 * through an MBCS pair.
 *
 * Neither of these are truly valid input conditions, but to ensure predictably
 * correct behaviour in the presence of these conditions, we have removed
 * the optimisation.
 */

/*
 *  It is unknown whether (current - 1) is a single byte character or a
 *  trail.  Now decrement temp until
 *      a)  The beginning of the string is reached, or
 *      b)  A non-lead byte (either single or trail) is found.
 *  The difference between (current-1) and temp is the number of non-single
 *  byte characters preceding (current-1).  There are two cases for this:
 *      a)  (current - temp) is odd, and
 *      b)  (current - temp) is even.
 *  If odd, then there are an odd number of "lead bytes" preceding the
 *  single/trail byte (current - 1), indicating that it is a trail byte.
 *  If even, then there are an even number of "lead bytes" preceding the
 *  single/trail byte (current - 1), indicating a single byte character.
 */
#ifdef _MT
        while ( (string <= --temp) && (__ismbblead_mt(ptmbci, *temp)) )
#else  /* _MT */
        while ( (string <= --temp) && (_ismbblead(*temp)) )
#endif  /* _MT */
                ;

        return (unsigned char *)(current - 1 - ((current - temp) & 0x01) );
}

#endif  /* _MBCS */
