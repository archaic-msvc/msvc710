/* yvals.h values header for Microsoft C/C++ */
#pragma once
#ifndef _YVALS
#define _YVALS

#define _CPPLIB_VER	313


		/* NAMING PROPERTIES */
#define _WIN32_C_LIB	1

		/* THREAD AND LOCALE CONTROL */
#define _MULTI_THREAD	_MT	/* nontrivial locks if multithreaded */
#define _GLOBAL_LOCALE	0	/* 0 for per-thread locales, 1 for shared */

		/* THREAD-LOCAL STORAGE */
#define _COMPILER_TLS	1	/* 1 if compiler supports TLS directly */
 #if _MULTI_THREAD
  #define _TLS_QUAL	__declspec(thread)	/* TLS qualifier, if any */

 #else /* _MULTI_THREAD */
  #define _TLS_QUAL
 #endif /* _MULTI_THREAD */

 #ifndef _HAS_EXCEPTIONS
  #define  _HAS_EXCEPTIONS  1	/* predefine as 0 to disable exceptions */
 #endif /* _HAS_EXCEPTIONS */

  #define _GLOBAL_USING	1

/*
Predefine this to be 0 to suppress us putting deprecated versions of the hash_xxx
family of classes into _STD
*/
#ifndef _DEFINE_DEPRECATED_HASH_CLASSES
#define _DEFINE_DEPRECATED_HASH_CLASSES 1
#endif 

#include <use_ansi.h>

#ifndef _VC6SP2
 #define _VC6SP2	0 /* define as 1 to fix linker errors with V6.0 SP2 */
#endif /* _VC6SP2 */

/* Define _CRTIMP2 */
 #ifndef _CRTIMP2
  #ifdef  CRTDLL2
   #define _CRTIMP2	__declspec(dllexport)
  #else   /* ndef CRTDLL2 */

   #if defined(_DLL) && !defined(_STATIC_CPPLIB)
    #define _CRTIMP2	__declspec(dllimport)

   #else   /* ndef _DLL && !STATIC_CPPLIB */
    #define _CRTIMP2
   #endif  /* _DLL && !STATIC_CPPLIB */

  #endif  /* CRTDLL2 */
 #endif  /* _CRTIMP2 */

 #if defined(_DLL) && !defined(_STATIC_CPPLIB)
  #define _DLL_CPPLIB
 #endif

  #ifndef _INTERNAL_IFSTRIP_
/* Define _CRTBLD_NATIVE_WCHAR_T */

   #if defined(_CRTBLD) && defined(__cplusplus)
    #ifndef _NATIVE_WCHAR_T_DEFINED
     #error Native wchar_t must be defined

    #else /* _NATIVE_WCHAR_T_DEFINED */
     #define _CRTBLD_NATIVE_WCHAR_T
    #endif /* _NATIVE_WCHAR_T_DEFINED */

   #endif /* defined(_CRTBLD) && defined(__cplusplus) */
  #endif /* _INTERNAL_IFSTRIP_ */

  #define _DEPRECATED	__declspec(deprecated)


  #ifndef _INTERNAL_IFSTRIP_
/* These functions are for enabling STATIC_CPPLIB functionality */
   #define _cpp_stdin  (&(__iob_func())[0])
   #define _cpp_stdout (&(__iob_func())[1])
   #define _cpp_stderr (&(__iob_func())[2])
   #define _cpp_isleadbyte(c) (__pctype_func()[(unsigned char)(c)] & _LEADBYTE)
  #endif

		/* NAMESPACE */

 #if defined(__cplusplus)
  #define _STD_BEGIN	namespace std {
  #define _STD_END		}
  #define _STD	::std::

/*
We use the stdext (standard extension) namespace to contain extensions that are not part of the current standard
*/
  #define _STDEXT_BEGIN	    namespace stdext {
  #define _STDEXT_END		}
  #define _STDEXT	        ::stdext::

  #ifdef _STD_USING
   #define _C_STD_BEGIN	namespace std {	/* only if *.c compiled as C++ */
   #define _C_STD_END	}
   #define _CSTD	::std::

  #else /* _STD_USING */
/* #define _GLOBAL_USING	*.h in global namespace, c* imports to std */

   #define _C_STD_BEGIN
   #define _C_STD_END
   #define _CSTD	::
  #endif /* _STD_USING */

  #define _C_LIB_DECL		extern "C" {	/* C has extern "C" linkage */
  #define _END_C_LIB_DECL	}
  #define _EXTERN_C			extern "C" {
  #define _END_EXTERN_C		}

 #else /* __cplusplus */
  #define _STD_BEGIN
  #define _STD_END
  #define _STD

  #define _C_STD_BEGIN
  #define _C_STD_END
  #define _CSTD

  #define _C_LIB_DECL
  #define _END_C_LIB_DECL
  #define _EXTERN_C
  #define _END_EXTERN_C
 #endif /* __cplusplus */

 #define _Restrict	restrict

 #ifdef __cplusplus
_STD_BEGIN
typedef bool _Bool;
_STD_END
 #endif /* __cplusplus */

		/* VC++ COMPILER PARAMETERS */
#define _LONGLONG	__int64
#define _ULONGLONG	unsigned __int64
#define _LLONG_MAX	0x7fffffffffffffff
#define _ULLONG_MAX	0xffffffffffffffff

		/* INTEGER PROPERTIES */
#define _C2			1	/* 0 if not 2's complement */

#define _MAX_EXP_DIG	8	/* for parsing numerics */
#define _MAX_INT_DIG	32
#define _MAX_SIG_DIG	36

typedef _LONGLONG _Longlong;
typedef _ULONGLONG _ULonglong;

		/* STDIO PROPERTIES */
#define _Filet _iobuf

 #ifndef _FPOS_T_DEFINED
  #define _FPOSOFF(fp)	((long)(fp))
 #endif /* _FPOS_T_DEFINED */

#define _IOBASE	_base
#define _IOPTR	_ptr
#define _IOCNT	_cnt

		/* MULTITHREAD PROPERTIES */
		/* LOCK MACROS */
#define _LOCK_LOCALE	0
#define _LOCK_MALLOC	1
#define _LOCK_STREAM	2
#define _MAX_LOCK		3	/* one more than highest lock number */

 #ifdef __cplusplus
_STD_BEGIN
		// CLASS _Lockit
class _CRTIMP2 _Lockit
	{	// lock while object in existence -- MUST NEST
public:
  #if _MULTI_THREAD

	explicit _Lockit();	// set default lock
	explicit _Lockit(int);	// set the lock
	~_Lockit();	// clear the lock

private:
	_Lockit(const _Lockit&);				// not defined
	_Lockit& operator=(const _Lockit&);	// not defined

	int _Locktype;

  #else /* _MULTI_THREAD */

	explicit _Lockit()
		{	// do nothing
		}

	explicit _Lockit(int)
		{	// do nothing
		}

	~_Lockit()
		{	// do nothing
		}
  #endif /* _MULTI_THREAD */

	};

class _CRTIMP2 _Mutex
	{	// lock under program control
public:

  #if _MULTI_THREAD
	_Mutex();
	~_Mutex();
	void _Lock();
	void _Unlock();

private:
	_Mutex(const _Mutex&);				// not defined
	_Mutex& operator=(const _Mutex&);	// not defined
	void *_Mtx;

  #else /* _MULTI_THREAD */
    void _Lock()
		{	// do nothing
		}

	void _Unlock()
		{	// do nothing
		}
  #endif /* _MULTI_THREAD */

	};

class _Init_locks
	{	// initialize mutexes
public:

 #if _MULTI_THREAD
	_Init_locks();
	~_Init_locks();

 #else /* _MULTI_THREAD */
	_Init_locks()
		{	// do nothing
		}

	~_Init_locks()
		{	// do nothing
		}
 #endif /* _MULTI_THREAD */

	};
_STD_END
 #endif /* __cplusplus */



		/* MISCELLANEOUS MACROS AND TYPES */
_C_STD_BEGIN
_EXTERN_C
_CRTIMP2 void __cdecl _Atexit(void (__cdecl *)(void));
_END_EXTERN_C

typedef int _Mbstatet;

#define _ATEXIT_T	void
#define _Mbstinit(x)	mbstate_t x = {0}
_C_STD_END

#endif /* _YVALS */

/*
 * Copyright (c) 1992-2002 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
 V3.13:0009 */
