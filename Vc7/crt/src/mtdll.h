/***
*mtdll.h - DLL/Multi-thread include
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*
*       [Internal]
*
****/

#if _MSC_VER > 1000
#pragma once
#endif  /* _MSC_VER > 1000 */

#ifndef _INC_MTDLL
#define _INC_MTDLL

#ifndef _CRTBLD
/*
 * This is an internal C runtime header file. It is used when building
 * the C runtimes only. It is not to be used as a public header file.
 */
#error ERROR: Use of C runtime library internal header file.
#endif  /* _CRTBLD */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include <cruntime.h>
#include <windows.h>

#if !defined (_W64)
#if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300
#define _W64 __w64
#else  /* !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300 */
#define _W64
#endif  /* !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300 */
#endif  /* !defined (_W64) */

/* Define _CRTIMP */

#ifndef _CRTIMP
#ifdef CRTDLL
#define _CRTIMP __declspec(dllexport)
#else  /* CRTDLL */
#ifdef _DLL
#define _CRTIMP __declspec(dllimport)
#else  /* _DLL */
#define _CRTIMP
#endif  /* _DLL */
#endif  /* CRTDLL */
#endif  /* _CRTIMP */


/* Define __cdecl for non-Microsoft compilers */

#if (!defined (_MSC_VER) && !defined (__cdecl))
#define __cdecl
#endif  /* (!defined (_MSC_VER) && !defined (__cdecl)) */


#ifndef _UINTPTR_T_DEFINED
#ifdef _WIN64
typedef unsigned __int64    uintptr_t;
#else  /* _WIN64 */
typedef _W64 unsigned int   uintptr_t;
#endif  /* _WIN64 */
#define _UINTPTR_T_DEFINED
#endif  /* _UINTPTR_T_DEFINED */


/*
 * Define the number of supported handles and streams. The definitions
 * here must exactly match those in internal.h (for _NHANDLE_) and stdio.h
 * (for _NSTREAM_).
 */

#define _IOB_ENTRIES    20

/* Lock symbols */

#define _SIGNAL_LOCK    0       /* lock for signal()                */
#define _IOB_SCAN_LOCK  1       /* _iob[] table lock                */
#define _TMPNAM_LOCK    2       /* lock global tempnam variables    */
#define _CONIO_LOCK     3       /* lock for conio routines          */
#define _HEAP_LOCK      4       /* lock for heap allocator routines */
#define _UNDNAME_LOCK   5       /* lock for unDName() routine       */
#define _TIME_LOCK      6       /* lock for time functions          */
#define _ENV_LOCK       7       /* lock for environment variables   */
#define _EXIT_LOCK1     8       /* lock #1 for exit code            */
#define _POPEN_LOCK     9       /* lock for _popen/_pclose database */
#define _LOCKTAB_LOCK   10      /* lock to protect semaphore lock table */
#define _OSFHND_LOCK    11      /* lock to protect _osfhnd array    */
#define _SETLOCALE_LOCK 12      /* lock for locale handles, etc.    */
#define _MB_CP_LOCK     13      /* lock for multibyte code page     */
#define _TYPEINFO_LOCK  14      /* lock for type_info access        */
#define _DEBUG_LOCK     15      /* lock for debug global structs    */

#define _STREAM_LOCKS   16      /* Table of stream locks            */

#define _LAST_STREAM_LOCK  (_STREAM_LOCKS+_IOB_ENTRIES-1) /* Last stream lock */

#define _TOTAL_LOCKS        (_LAST_STREAM_LOCK+1)

#define _LOCK_BIT_INTS     (_TOTAL_LOCKS/(sizeof(unsigned)*8))+1   /* # of ints to hold lock bits */

#ifndef __assembler

/* Multi-thread macros and prototypes */

#if defined (_MT)

#define __TRY __try{
#define __FINALLY   }__finally{
#define __END_TRY_FINALLY }
/* need wchar_t for _wtoken field in _tiddata */
#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif  /* _WCHAR_T_DEFINED */


#ifdef _MT
#ifndef _THREADMBCINFO
typedef struct threadmbcinfostruct {
        int refcount;
        int mbcodepage;
        int ismbcodepage;
        int mblcid;
        unsigned short mbulinfo[6];
        char mbctype[257];
        char mbcasemap[256];
} threadmbcinfo;
typedef threadmbcinfo * pthreadmbcinfo;
#define _THREADMBCINFO
#endif  /* _THREADMBCINFO */
#endif  /* _MT */

#ifndef __LC_TIME_DATA
struct __lc_time_data {
        char *wday_abbr[7];
        char *wday[7];
        char *month_abbr[12];
        char *month[12];
        char *ampm[2];
        char *ww_sdatefmt;
        char *ww_ldatefmt;
        char *ww_timefmt;
        LCID ww_lcid;
        int  ww_caltype;
#ifdef _MT
        int  refcount;
#endif  /* _MT */
};
#define __LC_TIME_DATA
#endif  /* __LC_TIME_DATA */

#ifdef _MT
#ifndef _THREADLOCALEINFO
typedef struct threadlocaleinfostruct {
        int refcount;
        UINT lc_codepage;
        UINT lc_collate_cp;
        LCID lc_handle[6];      /* 6 == LC_MAX - LC_MIN + 1 */
        int lc_clike;
        int mb_cur_max;
        int * lconv_intl_refcount;
        int * lconv_num_refcount;
        int * lconv_mon_refcount;
        struct lconv * lconv;
        struct lconv * lconv_intl;
        int * ctype1_refcount;
        unsigned short * ctype1;
        const unsigned short * pctype;
        struct __lc_time_data * lc_time_curr;
        struct __lc_time_data * lc_time_intl;
} threadlocinfo;
typedef threadlocinfo * pthreadlocinfo;
#define _THREADLOCALEINFO
#endif  /* _THREADLOCALEINFO */
#endif  /* _MT */

_CRTIMP extern unsigned long __cdecl __threadid(void);
#define _threadid   (__threadid())
_CRTIMP extern uintptr_t __cdecl __threadhandle(void);
#define _threadhandle   (__threadhandle())



/* Structure for each thread's data */

struct _tiddata {
        unsigned long   _tid;       /* thread ID */


        uintptr_t _thandle;         /* thread handle */

        int     _terrno;            /* errno value */
        unsigned long   _tdoserrno; /* _doserrno value */
        unsigned int    _fpds;      /* Floating Point data segment */
        unsigned long   _holdrand;  /* rand() seed value */
        char *      _token;         /* ptr to strtok() token */
        wchar_t *   _wtoken;        /* ptr to wcstok() token */
        unsigned char * _mtoken;    /* ptr to _mbstok() token */

        /* following pointers get malloc'd at runtime */
        char *      _errmsg;        /* ptr to strerror()/_strerror() buff */
        wchar_t *   _werrmsg;       /* ptr to _wcserror()/__wcserror() buff */
        char *      _namebuf0;      /* ptr to tmpnam() buffer */
        wchar_t *   _wnamebuf0;     /* ptr to _wtmpnam() buffer */
        char *      _namebuf1;      /* ptr to tmpfile() buffer */
        wchar_t *   _wnamebuf1;     /* ptr to _wtmpfile() buffer */
        char *      _asctimebuf;    /* ptr to asctime() buffer */
        wchar_t *   _wasctimebuf;   /* ptr to _wasctime() buffer */
        void *      _gmtimebuf;     /* ptr to gmtime() structure */
        char *      _cvtbuf;        /* ptr to ecvt()/fcvt buffer */

        /* following fields are needed by _beginthread code */
        void *      _initaddr;      /* initial user thread address */
        void *      _initarg;       /* initial user thread argument */

        /* following three fields are needed to support signal handling and
         * runtime errors */
        void *      _pxcptacttab;   /* ptr to exception-action table */
        void *      _tpxcptinfoptrs; /* ptr to exception info pointers */
        int         _tfpecode;      /* float point exception code */

        /* pointer to the copy of the multibyte character information used by
         * the thread */
        pthreadmbcinfo  ptmbcinfo;

        /* pointer to the copy of the locale informaton used by the thead */
        pthreadlocinfo  ptlocinfo;


        /* following field is needed by NLG routines */
        unsigned long   _NLG_dwCode;

        /*
         * Per-Thread data needed by C++ Exception Handling
         */
        void *      _terminate;     /* terminate() routine */
        void *      _unexpected;    /* unexpected() routine */
        void *      _translator;    /* S.E. translator */
        void *      _purecall;      /* called when pure virtual happens */
        void *      _curexception;  /* current exception */
        void *      _curcontext;    /* current exception context */
        int         _ProcessingThrow; /* for uncaught_exception */
#if defined (_M_MRX000)
        void *      _pFrameInfoChain;
        void *      _pUnwindContext;
        void *      _pExitContext;
        int         _MipsPtdDelta;
        int         _MipsPtdEpsilon;
#elif defined (_M_PPC)
        void *      _pExitContext;
        void *      _pUnwindContext;
        void *      _pFrameInfoChain;
        int         _FrameInfo[6];
#elif defined (_M_IA64)
        void *      _pExitContext;
        void *      _pUnwindContext;
        void *      _pFrameInfoChain;
        unsigned __int64     _ImageBase;
        unsigned __int64     _TargetGp;
        unsigned __int64     _ThrowImageBase;
#elif defined (_M_IX86)
        void *      _pFrameInfoChain;
#endif  /* defined (_M_IX86) */
        };

typedef struct _tiddata * _ptiddata;

/*
 * Declaration of TLS index used in storing pointers to per-thread data
 * structures.
 */
extern unsigned long __tlsindex;

#ifdef _MT

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/*
 * Flag indicating whether or not setlocale() is active. Its value is the
 * number of setlocale() calls currently active.
 */
_CRTIMP extern int __setlc_active;

/*
 * Flag indicating whether or not a function which references the locale
 * without having locked it is active. Its value is the number of such
 * functions.
 */
_CRTIMP extern int __unguarded_readlc_active;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _MT */


/* macros */


#define _lock_fh(fh)            _lock_fhandle(fh)
#define _lock_str(s)            _lock_file(s)
#define _lock_str2(i,s)         _lock_file2(i,s)
#define _lock_fh_check(fh,flag)     if (flag) _lock_fhandle(fh)
#define _mlock(l)               _lock(l)
#define _munlock(l)             _unlock(l)
#define _unlock_fh(fh)          _unlock_fhandle(fh)
#define _unlock_str(s)          _unlock_file(s)
#define _unlock_str2(i,s)       _unlock_file2(i,s)
#define _unlock_fh_check(fh,flag)   if (flag) _unlock_fhandle(fh)

// This is only used with STDCPP stuff only
#define _lock_locale(llf)       \
        InterlockedIncrement( ___unguarded_readlc_active_add_func() );     \
        if ( ___setlc_active_func() ) {         \
            InterlockedDecrement( ___unguarded_readlc_active_add_func() ); \
            _lock( _SETLOCALE_LOCK );   \
            llf = 1;                    \
        }                               \
        else                            \
            llf = 0;

#define _unlock_locale(llf)     \
        if ( llf )                          \
            _unlock( _SETLOCALE_LOCK );     \
        else                                \
            InterlockedDecrement( ___unguarded_readlc_active_add_func() );



/* multi-thread routines */

void __cdecl _lock(int);
void __cdecl _lock_file(void *);
void __cdecl _lock_file2(int, void *);
int  __cdecl _lock_fhandle(int);
void __cdecl _lockexit(void);
void __cdecl _unlock(int);
void __cdecl _unlock_file(void *);
void __cdecl _unlock_file2(int, void *);
void __cdecl _unlock_fhandle(int);
void __cdecl _unlockexit(void);
int  __cdecl _mtinitlocknum(int);

_ptiddata __cdecl _getptd(void);  /* return address of per-thread CRT data */
void WINAPI _freefls(void *);         /* free up per-fiber CRT data block */
void __cdecl _freeptd(_ptiddata); /* free up a per-thread CRT data block */
void __cdecl _initptd(_ptiddata); /* initialize a per-thread CRT data block */
/* These functions are for enabling STATIC_CPPLIB functionality */
_CRTIMP int __cdecl ___setlc_active_func(void);
_CRTIMP int * __cdecl ___unguarded_readlc_active_add_func(void);

//
// Define Fiber Local Storage function prototypes and access macros.
//

typedef
VOID
(WINAPI *PFLS_CALLBACK_FUNCTION) (
    IN PVOID lpFlsData
    );

typedef
DWORD
(WINAPI *PFLS_ALLOC_FUNCTION) (
    IN PFLS_CALLBACK_FUNCTION lpCallback OPTIONAL
    );

typedef
PVOID
(WINAPI *PFLS_GETVALUE_FUNCTION) (
    IN DWORD dwFlsIndex
    );

typedef
BOOL
(WINAPI *PFLS_SETVALUE_FUNCTION) (
    IN DWORD dwFlsIndex,
    IN PVOID lpFlsData
    );

typedef
BOOL
(WINAPI *PFLS_FREE_FUNCTION) (
    IN DWORD dwFlsIndex
    );

extern PFLS_ALLOC_FUNCTION gpFlsAlloc;
extern PFLS_GETVALUE_FUNCTION gpFlsGetValue;
extern PFLS_SETVALUE_FUNCTION gpFlsSetValue;
extern PFLS_FREE_FUNCTION gpFlsFree;

#define FLS_ALLOC(callback)  ((gpFlsAlloc)(callback))
#define FLS_GETVALUE(index) ((gpFlsGetValue)(index))
#define FLS_SETVALUE(index, value) ((gpFlsSetValue)(index, value))
#define FLS_FREE(index) ((gpFlsFree)(index))

#else  /* defined (_MT) */


/* macros */
#define _lock_fh(fh)
#define _lock_str(s)
#define _lock_str2(i,s)
#define _lock_fh_check(fh,flag)
#define _mlock(l)
#define _munlock(l)
#define _unlock_fh(fh)
#define _unlock_str(s)
#define _unlock_str2(i,s)
#define _unlock_fh_check(fh,flag)

#define _lock_locale(llf)
#define _unlock_locale(llf)
#define __TRY
#define __FINALLY
#define __END_TRY_FINALLY

#endif  /* defined (_MT) */

#endif  /* __assembler */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _INC_MTDLL */
