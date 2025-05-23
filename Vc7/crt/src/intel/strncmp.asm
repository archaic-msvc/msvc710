        page    ,132
        title   strncmp - compare first n chars of two strings
;***
;strncmp.asm - compare first n characters of two strings
;
;       Copyright (c) Microsoft Corporation. All rights reserved.
;
;Purpose:
;       defines strncmp() - compare first n characters of two strings
;       for lexical order.
;
;*******************************************************************************

        .xlist
        include cruntime.inc
        .list

page
;***
;int strncmp(first, last, count) - compare first count chars of strings
;
;Purpose:
;       Compares two strings for lexical order.  The comparison stops
;       after: (1) a difference between the strings is found, (2) the end
;       of the strings is reached, or (3) count characters have been
;       compared.
;
;       Algorithm:
;       int
;       strncmp (first, last, count)
;             char *first, *last;
;             unsigned count;
;             {
;             if (!count)
;                     return(0);
;             while (--count && *first && *first == *last)
;                     {
;                     first++;
;                     last++;
;                     }
;             return(*first - *last);
;             }
;
;Entry:
;       char *first, *last - strings to compare
;       unsigned count - maximum number of characters to compare
;
;Exit:
;       returns <0 if first < last
;       returns 0 if first == last
;       returns >0 if first > last
;
;Uses:
;
;Exceptions:
;
;*******************************************************************************

        CODESEG

        public  strncmp
strncmp proc \
        uses edi esi ebx, \
        first:ptr byte, \
        last:ptr byte, \
        count:IWORD


        mov     ecx,[count]     ; cx=max number of bytes to compare
        jecxz   short toend     ; it's as if strings are equal

        mov     ebx,ecx         ; bx saves count

        mov     edi,[first]     ; di=first pointer (es=segment part)

        mov     esi,edi         ; si saves first pointer
        xor     eax,eax         ; ax=0
repne   scasb                   ; count bytes
        neg     ecx             ; cx=count - strlen
        add     ecx,ebx         ; strlen + count - strlen

okay:
        mov     edi,esi         ; restore first pointer
        mov     esi,[last]      ; si = last pointer
repe    cmpsb                   ; compare strings
        mov     al,[esi-1]
        xor     ecx,ecx         ; set return value = 0

        cmp     al,[edi-1]      ; last-first
        ja      short lastbig   ; <last is bigger>
        je      short toend     ; <equal>
        ;jb     short firstbig  ; <first is bigger>

firstbig:                       ; first string is bigger
        sub     ecx,2           ; make FFFE so 'not' will give 0001

lastbig:                        ; last string is bigger
        not     ecx             ; return -1

toend:
        mov     eax,ecx         ; return value

        ret                     ; _cdecl return

strncmp endp
        end
