/***
*drivfree.c - Get the size of a disk
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file has _getdiskfree()
*
*******************************************************************************/

#include <cruntime.h>
#include <direct.h>
#include <oscalls.h>

/***
*int _getdiskfree(drivenum, diskfree)  - get size of a specified disk
*
*Purpose:
*       Gets the size of the current or specified disk drive
*
*Entry:
*       int drivenum - 0 for current drive, or drive 1-26
*
*Exit:
*       returns 0 if succeeds
*       returns system error code on error.
*
*Exceptions:
*
*******************************************************************************/

#ifndef _WIN32
#error ERROR - ONLY WIN32 TARGET SUPPORTED!
#endif  /* _WIN32 */

unsigned __cdecl _getdiskfree (
        unsigned uDrive,
        struct _diskfree_t * pdf
        )
{
        char   Root[4];
        char * pRoot;

        if ( uDrive == 0 ) {
            pRoot = NULL;
        }
        else if ( uDrive > 26 ) {
            return ( ERROR_INVALID_PARAMETER );
        }
        else {
            pRoot = &Root[0];
            Root[0] = (char)uDrive + (char)('A' - 1);
            Root[1] = ':';
            Root[2] = '\\';
            Root[3] = '\0';
        }


        if ( !GetDiskFreeSpace( pRoot,
                                (LPDWORD)&(pdf->sectors_per_cluster),
                                (LPDWORD)&(pdf->bytes_per_sector),
                                (LPDWORD)&(pdf->avail_clusters),
                                (LPDWORD)&(pdf->total_clusters)) )
        {
            return ( (unsigned)GetLastError() );
        }
        return ( 0 );
}
