/***
*fread.c - read from a stream
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Read from the specified stream into the user's buffer.
*
*******************************************************************************/

#include <cruntime.h>
#include <stdio.h>
#include <mtdll.h>
#include <io.h>
#include <string.h>
#include <file2.h>

/***
*size_t fread(void *buffer, size_t size, size_t count, FILE *stream) -
*       read from specified stream into the specified buffer.
*
*Purpose:
*       Read 'count' items of size 'size' from the specified stream into
*       the specified buffer. Return when 'count' items have been read in
*       or no more items can be read from the stream.
*
*Entry:
*       buffer  - pointer to user's buffer
*       size    - size of the item to read in
*       count   - number of items to read
*       stream  - stream to read from
*
*Exit:
*       Returns the number of (whole) items that were read into the buffer.
*       This may be less than 'count' if an error or eof occurred. In this
*       case, ferror() or feof() should be used to distinguish between the
*       two conditions.
*
*Notes:
*       fread will attempt to buffer the stream (side effect of the _filbuf
*       call) if necessary.
*
*       No more than 0xFFFE bytes may be read in at a time by a call to
*       read(). Further, read() does not handle huge buffers. Therefore,
*       in large data models, the read request is broken down into chunks
*       that do not violate these considerations. Each of these chunks is
*       processed much like an fread() call in a small data model (by a
*       call to _nfread()).
*
*       MTHREAD/DLL - Handled in three layers. fread() handles the locking
*       and DS saving/loading/restoring (if required) and calls _fread_lk()
*       to do the work. _fread_lk() is the same as the single-thread,
*       large data model version of fread(). It breaks up the read request
*       into digestible chunks and calls _nfread() to do the actual work.
*
*       386/MTHREAD/DLL - Handled in just the two layers since it is small
*       data model. The outer layer, fread(), takes care of the stream locking
*       and calls _fread_lk() to do the actual work. _fread_lk() is the same
*       as the single-thread version of fread().
*
*******************************************************************************/


#ifdef _MT
/* define locking/unlocking version */
size_t __cdecl fread (
        void *buffer,
        size_t size,
        size_t count,
        FILE *stream
        )
{
        size_t retval;

        _lock_str(stream);              /* lock stream */
        __try {
                /* do the read */
                retval = _fread_lk(buffer, size, count, stream);
        }
        __finally {
                _unlock_str(stream);    /* unlock stream */
        }

        return retval;
}
#endif  /* _MT */

/* define the normal version */
#ifdef _MT
size_t __cdecl _fread_lk (
#else  /* _MT */
size_t __cdecl fread (
#endif  /* _MT */
        void *buffer,
        size_t size,
        size_t num,
        FILE *stream
        )
{
        char *data;                     /* point to where should be read next */
        size_t total;                   /* total bytes to read */
        size_t count;                   /* num bytes left to read */
        unsigned bufsize;               /* size of stream buffer */
        unsigned nbytes;                /* how much to read now */
        unsigned nread;                 /* how much we did read */
        int c;                          /* a temp char */

        /* initialize local vars */
        data = buffer;

        if ( (count = total = size * num) == 0 )
                return 0;

        if (anybuf(stream))
                /* already has buffer, use its size */
                bufsize = stream->_bufsiz;
        else
                /* assume will get _INTERNAL_BUFSIZ buffer */
                bufsize = _INTERNAL_BUFSIZ;

        /* here is the main loop -- we go through here until we're done */
        while (count != 0) {
                /* if the buffer exists and has characters, copy them to user
                   buffer */
                if (anybuf(stream) && stream->_cnt != 0) {
                        /* how much do we want? */
                        nbytes = (count < (size_t)stream->_cnt) ? (unsigned)count : stream->_cnt;
                        memcpy(data, stream->_ptr, nbytes);

                        /* update stream and amt of data read */
                        count -= nbytes;
                        stream->_cnt -= nbytes;
                        stream->_ptr += nbytes;
                        data += nbytes;
                }
                else if (count >= bufsize) {
                        /* If we have more than bufsize chars to read, get data
                           by calling read with an integral number of bufsiz
                           blocks.  Note that if the stream is text mode, read
                           will return less chars than we ordered. */

                        /* calc chars to read -- (count/bufsize) * bufsize */
                        nbytes = ( bufsize ? (unsigned)(count - count % bufsize) :
                                   (unsigned)count );

                        nread = _read(_fileno(stream), data, nbytes);
                        if (nread == 0) {
                                /* end of file -- out of here */
                                stream->_flag |= _IOEOF;
                                return (total - count) / size;
                        }
                        else if (nread == (unsigned)-1) {
                                /* error -- out of here */
                                stream->_flag |= _IOERR;
                                return (total - count) / size;
                        }

                        /* update count and data to reflect read */
                        count -= nread;
                        data += nread;
                }
                else {
                        /* less than bufsize chars to read, so call _filbuf to
                           fill buffer */
                        if ((c = _filbuf(stream)) == EOF) {
                                /* error or eof, stream flags set by _filbuf */
                                return (total - count) / size;
                        }

                        /* _filbuf returned a char -- store it */
                        *data++ = (char) c;
                        --count;

                        /* update buffer size */
                        bufsize = stream->_bufsiz;
                }
        }

        /* we finished successfully, so just return num */
        return num;
}
