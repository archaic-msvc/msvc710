/***
*strtime.c - contains the function "_strtime()"
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       contains the function _strtime()
*
*******************************************************************************/


#include <cruntime.h>
#include <time.h>
#include <tchar.h>
#include <oscalls.h>


/***
*_TSCHAR *_strtime(buffer) - return time in string form
*
*Purpose:
*       _strtime() returns a string containing the time in "HH:MM:SS" form
*
*Entry:
*       _TSCHAR *buffer = the address of a 9-byte user buffer
*
*Exit:
*       returns buffer, which contains the time in "HH:MM:SS" form
*
*Exceptions:
*
*******************************************************************************/

_TSCHAR * __cdecl _tstrtime (
        _TSCHAR *buffer
        )
{
        int hours, minutes, seconds;
        SYSTEMTIME dt;                       /* Win32 time structure */
        GetLocalTime(&dt);

        hours = dt.wHour;
        minutes = dt.wMinute;
        seconds = dt.wSecond;

        /* store the components of the time into the string */
        /* store separators */
        buffer[2] = buffer[5] = _T(':');
        /* store end of string */
        buffer[8] = _T('\0');
        /* store tens of hour */
        buffer[0] = (_TSCHAR) (hours   / 10 + _T('0'));
        /* store units of hour */
        buffer[1] = (_TSCHAR) (hours   % 10 + _T('0'));
        /* store tens of minute */
        buffer[3] = (_TSCHAR) (minutes / 10 + _T('0'));
        /* store units of minute */
        buffer[4] = (_TSCHAR) (minutes % 10 + _T('0'));
        /* store tens of second */
        buffer[6] = (_TSCHAR) (seconds / 10 + _T('0'));
        /* store units of second */
        buffer[7] = (_TSCHAR) (seconds % 10 + _T('0'));

        return buffer;
}

