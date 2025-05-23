/***
*setlocal.h - internal definitions used by locale-dependent functions.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Contains internal definitions/declarations for locale-dependent
*       functions, in particular those required by setlocale().
*
*       [Internal]
*
****/

#if _MSC_VER > 1000
#pragma once
#endif  /* _MSC_VER > 1000 */

#ifndef _INC_SETLOCAL
#define _INC_SETLOCAL

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
#include <oscalls.h>
#include <limits.h>

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

#define ERR_BUFFER_TOO_SMALL    1   // should be in windef.h

#define _CLOCALEHANDLE  0       /* "C" locale handle */
#define _CLOCALECP      CP_ACP  /* "C" locale Code page */


#define _COFFSET    127     /* introduced so that pctype can work with unsigned
                               char types and EOF, used only in initctype and
                               setloc.c
                               */
/* Define the max length for each string type including space for a null. */

#define _MAX_WDAY_ABBR  4
#define _MAX_WDAY   10
#define _MAX_MONTH_ABBR 4
#define _MAX_MONTH 10
#define _MAX_AMPM   3

#define _DATE_LENGTH    8       /* mm/dd/yy (null not included) */
#define _TIME_LENGTH    8       /* hh:mm:ss (null not included) */

/* LC_TIME localization structure */

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


#define MAX_LANG_LEN        64  /* max language name length */
#define MAX_CTRY_LEN        64  /* max country name length */
#define MAX_MODIFIER_LEN    0   /* max modifier name length - n/a */
#define MAX_LC_LEN          (MAX_LANG_LEN+MAX_CTRY_LEN+MAX_MODIFIER_LEN+3)
                                /* max entire locale string length */
#define MAX_CP_LEN          16  /* max code page name length */
#define CATNAMES_LEN        57  /* "LC_COLLATE=;LC_CTYPE=;..." length */

#define LC_INT_TYPE         0
#define LC_STR_TYPE         1

#ifndef _TAGLC_ID_DEFINED
typedef struct tagLC_ID {
        WORD wLanguage;
        WORD wCountry;
        WORD wCodePage;
} LC_ID, *LPLC_ID;
#define _TAGLC_ID_DEFINED
#endif  /* _TAGLC_ID_DEFINED */


typedef struct tagLC_STRINGS {
        char szLanguage[MAX_LANG_LEN];
        char szCountry[MAX_CTRY_LEN];
        char szCodePage[MAX_CP_LEN];
} LC_STRINGS, *LPLC_STRINGS;

#ifdef _MT
#ifndef _THREADLOCALEINFO
typedef struct threadlocaleinfostruct {
        int refcount;
        UINT lc_codepage;
        UINT lc_collate_cp;
        LCID lc_handle[6];
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
extern pthreadlocinfo __ptlocinfo;
pthreadlocinfo __cdecl __updatetlocinfo(void);
#endif  /* _MT */

extern LC_ID __lc_id[];                 /* complete info from GetQualifiedLocale */
_CRTIMP extern LCID __lc_handle[];      /* locale "handles" -- ignores country info */
_CRTIMP extern UINT __lc_codepage;      /* code page */
_CRTIMP extern UINT __lc_collate_cp;    /* code page for LC_COLLATE */

_CRTIMP extern int __lc_clike;          /* if first 127 characters of
                                           current locale are same as
                                           first 127 characters of C_LOCALE */
#ifndef _INTERNAL_IFSTRIP_
/* These functions are for enabling STATIC_CPPLIB functionality */
_CRTIMP LCID* __cdecl ___lc_handle_func(void);
_CRTIMP UINT __cdecl ___lc_codepage_func(void);
_CRTIMP UINT __cdecl ___lc_collate_cp_func(void);
#endif  /* _INTERNAL_IFSTRIP_ */

BOOL __cdecl __get_qualified_locale(const LPLC_STRINGS, LPLC_ID, LPLC_STRINGS);

int __cdecl __getlocaleinfo (int, LCID, LCTYPE, void *);

/* lconv structure for the "C" locale */
extern struct lconv __lconv_c;

/* pointer to current lconv structure */
extern struct lconv * __lconv;

/* Pointer to non-C locale lconv */
extern struct lconv *__lconv_intl;

/* initial values for lconv structure */
extern char __lconv_static_decimal[];
extern char __lconv_static_null[];

///* language and country string definitions */
//typedef struct tagLANGREC
//{
//        CHAR * szLanguage;
//        WORD wLanguage;
//} LANGREC;
//extern LANGREC __rg_lang_rec[];
//
///ypedef struct tagCTRYREC
//{
//        CHAR * szCountry;
//        WORD wCountry;
//} CTRYREC;
//extern CTRYREC __rg_ctry_rec[];

/* Initialization functions for locale categories */

int __cdecl __init_collate(void);
int __cdecl __init_ctype(void);
int __cdecl __init_monetary(void);
int __cdecl __init_numeric(void);
int __cdecl __init_time(void);
int __cdecl __init_dummy(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _INC_SETLOCAL */
