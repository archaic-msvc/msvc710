/***
*setlocal.c - Contains the setlocale function
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Contains the setlocale() function.
*
*******************************************************************************/

#include <locale.h>

#if !defined (_WIN32)

static char _clocalestr[] = "C";

#else  /* !defined (_WIN32) */

#include <cruntime.h>
#include <mtdll.h>
#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h> /* for strtol */
#include <setlocal.h>
#include <dbgint.h>
#include <ctype.h>
#include <awint.h>

/* C locale */
static char _clocalestr[] = "C";


#define _LOC_CCACHE 5   // Cache of last 5 locale changed and if they are clike.

__declspec(selectany) struct {
        const char * catname;
        char * locale;
        int (* init)(void);
} __lc_category[LC_MAX-LC_MIN+1] = {
        /* code assumes locale initialization is "_clocalestr" */
        { "LC_ALL",     NULL,           __init_dummy /* never called */ },
        { "LC_COLLATE", _clocalestr,    __init_collate  },
        { "LC_CTYPE",   _clocalestr,    __init_ctype    },
        { "LC_MONETARY",_clocalestr,    __init_monetary },
        { "LC_NUMERIC", _clocalestr,    __init_numeric  },
        { "LC_TIME",    _clocalestr,    __init_time }
};

struct _is_ctype_compatible {
        unsigned long id;
        int is_clike;
};
        /* First 127 character type for CLOCALE */
static const short _ctype_loc_style[] = {
        _CONTROL,
        _CONTROL,
        _CONTROL,
        _CONTROL,
        _CONTROL,
        _CONTROL,
        _CONTROL,
        _CONTROL,
        _SPACE | _CONTROL | _BLANK,
        _SPACE | _CONTROL,
        _SPACE | _CONTROL,
        _SPACE | _CONTROL,
        _SPACE | _CONTROL,
        _CONTROL,
        _CONTROL,
        _CONTROL,
        _CONTROL,
        _CONTROL,
        _CONTROL,
        _CONTROL,
        _CONTROL,
        _CONTROL,
        _CONTROL,
        _CONTROL,
        _CONTROL,
        _CONTROL,
        _CONTROL,
        _CONTROL,
        _CONTROL,
        _CONTROL,
        _CONTROL,
        _SPACE | _BLANK,
        _PUNCT,
        _PUNCT,
        _PUNCT,
        _PUNCT,
        _PUNCT,
        _PUNCT,
        _PUNCT,
        _PUNCT,
        _PUNCT,
        _PUNCT,
        _PUNCT,
        _PUNCT,
        _PUNCT,
        _PUNCT,
        _PUNCT,
        _DIGIT | _HEX,
        _DIGIT | _HEX,
        _DIGIT | _HEX,
        _DIGIT | _HEX,
        _DIGIT | _HEX,
        _DIGIT | _HEX,
        _DIGIT | _HEX,
        _DIGIT | _HEX,
        _DIGIT | _HEX,
        _DIGIT | _HEX,
        _PUNCT,
        _PUNCT,
        _PUNCT,
        _PUNCT,
        _PUNCT,
        _PUNCT,
        _PUNCT,
        _UPPER | _HEX | C1_ALPHA,
        _UPPER | _HEX | C1_ALPHA,
        _UPPER | _HEX | C1_ALPHA,
        _UPPER | _HEX | C1_ALPHA,
        _UPPER | _HEX | C1_ALPHA,
        _UPPER | _HEX | C1_ALPHA,
        _UPPER | C1_ALPHA,
        _UPPER | C1_ALPHA,
        _UPPER | C1_ALPHA,
        _UPPER | C1_ALPHA,
        _UPPER | C1_ALPHA,
        _UPPER | C1_ALPHA,
        _UPPER | C1_ALPHA,
        _UPPER | C1_ALPHA,
        _UPPER | C1_ALPHA,
        _UPPER | C1_ALPHA,
        _UPPER | C1_ALPHA,
        _UPPER | C1_ALPHA,
        _UPPER | C1_ALPHA,
        _UPPER | C1_ALPHA,
        _UPPER | C1_ALPHA,
        _UPPER | C1_ALPHA,
        _UPPER | C1_ALPHA,
        _UPPER | C1_ALPHA,
        _UPPER | C1_ALPHA,
        _UPPER | C1_ALPHA,
        _PUNCT,
        _PUNCT,
        _PUNCT,
        _PUNCT,
        _PUNCT,
        _PUNCT,
        _LOWER | _HEX | C1_ALPHA,
        _LOWER | _HEX | C1_ALPHA,
        _LOWER | _HEX | C1_ALPHA,
        _LOWER | _HEX | C1_ALPHA,
        _LOWER | _HEX | C1_ALPHA,
        _LOWER | _HEX | C1_ALPHA,
        _LOWER | C1_ALPHA,
        _LOWER | C1_ALPHA,
        _LOWER | C1_ALPHA,
        _LOWER | C1_ALPHA,
        _LOWER | C1_ALPHA,
        _LOWER | C1_ALPHA,
        _LOWER | C1_ALPHA,
        _LOWER | C1_ALPHA,
        _LOWER | C1_ALPHA,
        _LOWER | C1_ALPHA,
        _LOWER | C1_ALPHA,
        _LOWER | C1_ALPHA,
        _LOWER | C1_ALPHA,
        _LOWER | C1_ALPHA,
        _LOWER | C1_ALPHA,
        _LOWER | C1_ALPHA,
        _LOWER | C1_ALPHA,
        _LOWER | C1_ALPHA,
        _LOWER | C1_ALPHA,
        _LOWER | C1_ALPHA,
        _PUNCT,
        _PUNCT,
        _PUNCT,
        _PUNCT,
        _CONTROL
};

static const char _first_127char[] = {
        1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17,
        18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,
        35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68,
        69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85,
        86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100,101,102,
        103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,
        120,121,122,123,124,125,126,127
};

#ifdef _MT

extern unsigned short *__ctype1;                /* defined in initctyp.c */
extern struct __lc_time_data __lc_time_c;       /* defined in strftime.c */
extern struct __lc_time_data *__lc_time_curr;   /* defined in strftime.c */
extern struct __lc_time_data *__lc_time_intl;   /* defined in inittime.c */

/*
 * initial locale information struct, set to the C locale. Used only until the
 * first call to setlocale()
 */
threadlocinfo __initiallocinfo = {
        1,                  /* refcount            */
        _CLOCALECP,         /* lc_codepage         */
        _CLOCALECP,         /* lc_collate_cp       */
        { _CLOCALEHANDLE,   /* lc_handle[6]        */
          _CLOCALEHANDLE,
          _CLOCALEHANDLE,
          _CLOCALEHANDLE,
          _CLOCALEHANDLE,
          _CLOCALEHANDLE },
        1,                  /* lc_clike            */
        1,                  /* mb_cur_max          */
        NULL,               /* lconv_intl_refcount */
        NULL,               /* lconv_num_refcount  */
        NULL,               /* lconv_mon_refcount  */
        &__lconv_c,         /* lconv               */
        NULL,               /* lconv_intl          */
        NULL,               /* ctype1_refcount     */
        NULL,               /* ctype1              */
        __newctype + 128,   /* pctype                   */
        &__lc_time_c,       /* lc_time_curr        */
        NULL                /* lc_time_intl        */
};

/*
 * global pointer to the current per-thread locale information structure.
 */
pthreadlocinfo __ptlocinfo = &__initiallocinfo;

/*
 * Flag indicating whether or not setlocale() is active. Its value is the
 * number of setlocale() calls currently active.
 */
_CRTIMP int __setlc_active;
/* These functions are for enabling STATIC_CPPLIB functionality */
_CRTIMP int __cdecl ___setlc_active_func(void)
{
    return __setlc_active;
}

/*
 * Flag indicating whether or not a function which references the locale
 * without having locked it is active. Its value is the number of such
 * functions.
 */
_CRTIMP int __unguarded_readlc_active;
/* These functions are for enabling STATIC_CPPLIB functionality */
_CRTIMP int * __cdecl ___unguarded_readlc_active_add_func(void)
{
    return &__unguarded_readlc_active;
}

#endif  /* _MT */

/* helper function prototypes */
char * _expandlocale(char *, char *, LC_ID *, UINT *, int);
void _strcats(char *, int, ...);
void __lc_lctostr(char *, const LC_STRINGS *);
int __lc_strtolc(LC_STRINGS *, const char *);
static char * __cdecl _setlocale_set_cat(int, const char *);
static char * __cdecl _setlocale_get_all(void);

#ifdef _MT
extern int * __lconv_intl_refcount;
extern int * __lconv_num_refcount;
extern int * __lconv_mon_refcount;
extern int * __ctype1_refcount;
static pthreadlocinfo __cdecl __updatetlocinfo_lk(void);
static char * __cdecl _setlocale_lk(int, const char *);
void __cdecl __free_lconv_mon(struct lconv *);
void __cdecl __free_lconv_num(struct lconv *);
void __cdecl __free_lc_time(struct __lc_time_data *);
#endif  /* _MT */

#endif  /* !defined (_WIN32) */


#ifdef _MT

/***
*__freetlocinfo() - free threadlocinfo
*
*Purpose:
*       Free up the per-thread locale info structure specified by the passed
*       pointer.
*
*Entry:
*       pthreadlocinfo ptloci
*
*Exit:
*
*Exceptions:
*
*******************************************************************************/

void __cdecl __freetlocinfo (
        pthreadlocinfo ptloci
        )
{
        /*
         * Free up lconv struct
         */
        if ( (ptloci->lconv_intl != __lconv_intl) &&
             (ptloci->lconv_intl != NULL) &&
             (*(ptloci->lconv_intl_refcount) == 0))
        {
            if ( (ptloci->lconv_mon_refcount != NULL) &&
                 (*(ptloci->lconv_mon_refcount) == 0) &&
                 (ptloci->lconv_mon_refcount != __lconv_mon_refcount) )
            {
                _free_crt(ptloci->lconv_mon_refcount);
                __free_lconv_mon(ptloci->lconv_intl);
            }

            if ( (ptloci->lconv_num_refcount != NULL) &&
                 (*(ptloci->lconv_num_refcount) == 0) &&
                 (ptloci->lconv_num_refcount != __lconv_num_refcount) )
            {
                _free_crt(ptloci->lconv_num_refcount);
                __free_lconv_num(ptloci->lconv_intl);
            }

            _free_crt(ptloci->lconv_intl_refcount);
            _free_crt(ptloci->lconv_intl);
        }

        /*
         * Free up ctype tables
         */
        if ( (ptloci->ctype1_refcount != __ctype1_refcount) &&
             (ptloci->ctype1_refcount != NULL) &&
             (*(ptloci->ctype1_refcount) == 0) )
        {
            _free_crt(ptloci->ctype1_refcount);
            _free_crt(ptloci->ctype1-_COFFSET);
        }

        /*
         * Free up the __lc_time_data struct
         */
        if ( (ptloci->lc_time_intl != __lc_time_intl) &&
             (ptloci->lc_time_intl != NULL) &&
             ((ptloci->lc_time_intl->refcount) == 0) )
        {
            __free_lc_time(ptloci->lc_time_intl);
            _free_crt(ptloci->lc_time_intl);
        }

        /*
         * Free up the threadlocinfo struct
         */
        _free_crt(ptloci);
}


/***
*__updatetlocinfo() - refresh the thread's locale info
*
*Purpose:
*       Update the current thread's reference to the locale information to
*       match the current global locale info. Decrement the reference on the
*       old locale information struct and if this count is now zero (so that no
*       threads are using it), free it.
*
*Entry:
*
*Exit:
*       _getptd()->ptlocinfo == __ptlocinfo
*
*Exceptions:
*
*******************************************************************************/

pthreadlocinfo __cdecl __updatetlocinfo(void)
{
        pthreadlocinfo ptloci;

        _mlock(_SETLOCALE_LOCK);
        __try
        {
            ptloci = __updatetlocinfo_lk();
        }
        __finally
        {
            _munlock(_SETLOCALE_LOCK);
        }

        return ptloci;
}

static pthreadlocinfo __cdecl __updatetlocinfo_lk(void)
{
        pthreadlocinfo ptloci;
        _ptiddata ptd = _getptd();

        if ( (ptloci = ptd->ptlocinfo) != __ptlocinfo )
        {
            /*
             * Decrement the reference counts in the old locale info
             * structure.
             */
            if ( ptloci != NULL )
            {
                (ptloci->refcount)--;

                if ( ptloci->lconv_intl_refcount != NULL )
                    (*(ptloci->lconv_intl_refcount))--;

                if ( ptloci->lconv_mon_refcount != NULL )
                    (*(ptloci->lconv_mon_refcount))--;

                if ( ptloci->lconv_num_refcount != NULL )
                    (*(ptloci->lconv_num_refcount))--;

                if ( ptloci->ctype1_refcount != NULL )
                    (*(ptloci->ctype1_refcount))--;

                (ptloci->lc_time_curr->refcount)--;
            }

            /*
             * Update to the current locale info structure and increment the
             * reference counts.
             */
            ptd->ptlocinfo = __ptlocinfo;
            (__ptlocinfo->refcount)++;

            if ( __ptlocinfo->lconv_intl_refcount != NULL )
                (*(__ptlocinfo->lconv_intl_refcount))++;

            if ( __ptlocinfo->lconv_mon_refcount != NULL )
                (*(__ptlocinfo->lconv_mon_refcount))++;

            if ( __ptlocinfo->lconv_num_refcount != NULL )
                (*(__ptlocinfo->lconv_num_refcount))++;

            if ( __ptlocinfo->ctype1_refcount != NULL )
                (*(__ptlocinfo->ctype1_refcount))++;

            (__ptlocinfo->lc_time_curr->refcount)++;

            /*
             * Free the old locale info structure, if necessary.  Must be done
             * after incrementing reference counts in current locale in case
             * any refcounts are shared with the old locale.
             */
            if ( (ptloci != NULL) &&
                 (ptloci->refcount == 0) &&
                 (ptloci != &__initiallocinfo) )
                __freetlocinfo(ptloci);
        }

        return ptd->ptlocinfo;
}

#endif  /* _MT */


/***
*char * setlocale(int category, char *locale) - Set one or all locale categories
*
*Purpose:
*       The setlocale() routine allows the user to set one or more of
*       the locale categories to the specific locale selected by the
*       user.  [ANSI]
*
*       NOTE: Under !_INTL, the C libraries only support the "C" locale.
*       Attempts to change the locale will fail.
*
*Entry:
*       int category = One of the locale categories defined in locale.h
*       char *locale = String identifying a specific locale or NULL to
*                  query the current locale.
*
*Exit:
*       If supplied locale pointer == NULL:
*
*           Return pointer to current locale string and do NOT change
*           the current locale.
*
*       If supplied locale pointer != NULL:
*
*           If locale string is '\0', set locale to default.
*
*           If desired setting can be honored, return a pointer to the
*           locale string for the appropriate category.
*
*           If desired setting can NOT be honored, return NULL.
*
*Exceptions:
*       Compound locale strings of the form "LC_COLLATE=xxx;LC_CTYPE=xxx;..."
*       are allowed for the LC_ALL category.  This is to support the ability
*       to restore locales with the returned string, as specified by ANSI.
*       Setting the locale with a compound locale string will succeed unless
*       *all* categories failed.  The returned string will reflect the current
*       locale.  For example, if LC_CTYPE fails in the above string, setlocale
*       will return "LC_COLLATE=xxx;LC_CTYPE=yyy;..." where yyy is the
*       previous locale (or the C locale if restoring the previous locale
*       also failed).  Unrecognized LC_* categories are ignored.
*
*******************************************************************************/

#if !defined (_WIN32)

char * __cdecl setlocale (
        int _category,
        const char *_locale
        )
{
        if ( (_locale == NULL) ||
             (_locale[0] == '\0') ||
             ( (_locale[0]=='C') && (_locale[1]=='\0'))  )
            return(_clocalestr);
        else
            return(NULL);
}

#else  /* !defined (_WIN32) */

char * __cdecl setlocale (
        int _category,
        const char *_locale
        )
{
        char * retval;
#ifdef _MT
        pthreadlocinfo ptloci;
        int i;

        /* Validate category */
        if ( (_category < LC_MIN) || (_category > LC_MAX) )
            return NULL;

        _mlock(_SETLOCALE_LOCK);

        __try {

            if ( _locale == NULL  )
                return _setlocale_lk(_category, NULL);

            if ( (ptloci = _malloc_crt( sizeof(threadlocinfo) )) == NULL )
                retval = NULL;

            if ( (ptloci != NULL) && (retval = _setlocale_lk(_category, _locale)) )
            {
                ptloci->refcount = 0;
                ptloci->lc_codepage = __lc_codepage;
                ptloci->lc_collate_cp = __lc_collate_cp;

                for ( i = 0 ; i <= LC_MAX - LC_MIN ; i++ )
                    ptloci->lc_handle[i] = __lc_handle[i];

                ptloci->lc_clike = __lc_clike;
                ptloci->mb_cur_max = __mb_cur_max;

                ptloci->lconv_intl_refcount = __lconv_intl_refcount;
                ptloci->lconv_num_refcount = __lconv_num_refcount;
                ptloci->lconv_mon_refcount = __lconv_mon_refcount;
                ptloci->lconv = __lconv;
                ptloci->lconv_intl = __lconv_intl;

                ptloci->ctype1_refcount = __ctype1_refcount;
                ptloci->ctype1 = __ctype1;
                ptloci->pctype = _pctype;

                ptloci->lc_time_curr = __lc_time_curr;
                ptloci->lc_time_intl = __lc_time_intl;

                if ( (__ptlocinfo->refcount == 0) &&
                     (__ptlocinfo != &__initiallocinfo) )
                     __freetlocinfo(__ptlocinfo);

                __ptlocinfo = ptloci;

                (void)__updatetlocinfo_lk();
            }

            if ( (retval == NULL) && (ptloci != NULL) )
                _free_crt(ptloci);

        }
        __finally {
            _munlock(_SETLOCALE_LOCK);
        }

        return retval;
}

static char * __cdecl _setlocale_lk(
        int _category,
        const char *_locale
        )
{
        char * retval;
#else  /* _MT */
        /* Validate category */
        if ((_category < LC_MIN) || (_category > LC_MAX))
            return NULL;
#endif  /* _MT */
        /* Interpret locale */

        if (_category != LC_ALL)
        {
            retval = (_locale) ? _setlocale_set_cat(_category,_locale) :
                __lc_category[_category].locale;

        } else { /* LC_ALL */
            char lctemp[MAX_LC_LEN];
            int i;
            int same = 1;
            int fLocaleSet = 0; /* flag to indicate if anything successfully set */

            if (_locale != NULL)
            {
                if ( (_locale[0]=='L') && (_locale[1]=='C') && (_locale[2]=='_') )
                {
                    /* parse compound locale string */
                    size_t len;
                    const char * p = _locale;  /* start of string to parse */
                    const char * s;

                    do {
                        s = strpbrk(p,"=;");

                        if ((s==(char *)NULL) || (!(len=(size_t)(s-p))) || (*s==';'))
                            return NULL;  /* syntax error */

                        /* match with known LC_ strings, if possible, else ignore */
                        for (i=LC_ALL+1; i<=LC_MAX; i++)
                        {
                            if ((!strncmp(__lc_category[i].catname,p,len))
                                && (len==strlen(__lc_category[i].catname)))
                            {
                                break;  /* matched i */
                            }
                        } /* no match if (i>LC_MAX) -- just ignore */

                        if ((!(len = strcspn(++s,";"))) && (*s!=';'))
                            return NULL;  /* syntax error */

                        if (i<=LC_MAX)
                        {
                            strncpy(lctemp, s, len);
                            lctemp[len]='\0';   /* null terminate string */

                            /* don't fail unless all categories fail */
                            if (_setlocale_set_cat(i,lctemp))
                                fLocaleSet++;       /* record a success */
                        }
                        if (*(p = s+len)!='\0')
                            p++;  /* skip ';', if present */

                    } while (*p);

                    retval = (fLocaleSet) ? _setlocale_get_all() : NULL;

                } else { /* simple LC_ALL locale string */

                    /* confirm locale is supported, get expanded locale */
                    if (retval = _expandlocale((char *)_locale, lctemp, NULL, NULL, _category))
                    {
                        for (i=LC_MIN; i<=LC_MAX; i++)
                        {
                            if (i!=LC_ALL)
                            {
                                if (strcmp(lctemp, __lc_category[i].locale))
                                {
                                    if (_setlocale_set_cat(i, lctemp))
                                    {
                                        fLocaleSet++;   /* record a success */
                                    }
                                    else
                                    {
                                        same = 0;       /* record a failure */
                                    }
                                }
                                else
                                    fLocaleSet++;   /* trivial succcess */
                            }
                        }
                        if (same) /* needn't call setlocale_get_all() if all the same */
                        {
                            retval = _setlocale_get_all();
                            /* retval set above */
                            _free_crt(__lc_category[LC_ALL].locale);
                            __lc_category[LC_ALL].locale = NULL;
                        }
                        else
                            retval = (fLocaleSet) ? _setlocale_get_all() : NULL;
                    }
                }
            } else { /* LC_ALL & NULL */
                retval = _setlocale_get_all ();
            }
        }

        /* common exit point */
        return retval;

} /* setlocale */


static char * __cdecl _setlocale_set_cat (
        int category,
        const char * locale
        )
{
        char * oldlocale;
        LCID oldhandle;
        UINT oldcodepage;
        LC_ID oldid;

        LC_ID idtemp;
        UINT cptemp;
        char lctemp[MAX_LC_LEN];
        char * pch;

        static struct _is_ctype_compatible _Lcid_c[_LOC_CCACHE] = {{0,1}};
        struct _is_ctype_compatible buf1, buf2;
        int i,j;
        short out[sizeof(_first_127char)];

        if (!_expandlocale((char *)locale, lctemp, &idtemp, &cptemp, category))
        {
            return NULL;            /* unrecognized locale */
        }
        if (!strcmp(lctemp, __lc_category[category].locale))
        {
            return __lc_category[category].locale;
        }

        if (!(pch = (char *)_malloc_crt(strlen(lctemp)+1)))
        {
            return NULL;  /* error if malloc fails */
        }

        oldlocale = __lc_category[category].locale; /* save for possible restore*/
        oldhandle = __lc_handle[category];
        memcpy((void *)&oldid, (void *)&__lc_id[category], sizeof(oldid));
        oldcodepage = __lc_codepage;

        /* update locale string */
        __lc_category[category].locale = strcpy(pch,lctemp);
        __lc_handle[category] = MAKELCID(idtemp.wLanguage, SORT_DEFAULT);
        memcpy((void *)&__lc_id[category], (void *)&idtemp, sizeof(idtemp));

        /* To speedup locale based comparisions, we identify if the current
         * local has first 127 character set same as CLOCALE. If yes then
         * __lc_clike = TRUE. Also we keep this info. in a cache of cache
         * size = _LOC_CCACHE, so that every time the locale is switched, we
         * don't have to call time consuming GetStringTypeA.
         */

        if (category==LC_CTYPE)
        {
            __lc_codepage = cptemp;
            buf1 = _Lcid_c[_LOC_CCACHE -1];
            /* brings the recently used codepage to the top. or else shifts
             * every thing down by one so that new _Lcid_c can be placed at
             * the top.
             */
            for ( i = 0; i < _LOC_CCACHE; i++)
            {
                if (__lc_codepage == _Lcid_c[i].id)
                {
                    /* We don't really want to swap cache around in case what we are looking
                     *  for is the first element of the cache
                     */
                    if (i!=0)
                    {
                        _Lcid_c[0] = _Lcid_c[i];
                        _Lcid_c[i] = buf1;
                    }
                    break;
                }
                else
                {
                    buf2 = _Lcid_c[i];
                    _Lcid_c[i] = buf1;
                    buf1 = buf2;
                }
            }
            if ( i == _LOC_CCACHE)
            {
                if ( __crtGetStringTypeA(CT_CTYPE1,
                                          _first_127char,
                                          sizeof(_first_127char),
                                          out,
                                          __lc_codepage,
                                          __lc_handle[LC_CTYPE],
                                          TRUE ))
                {
                    /* These are all the ctype flags that we support
                     */
                    for ( j = 0; j < sizeof(_first_127char)/sizeof(char); j++)
                    {
                        out[j] = out[j]&
                            (_UPPER|_LOWER|_DIGIT|_SPACE|_PUNCT|_CONTROL|_BLANK|_HEX|_ALPHA);
                    }
                    if ( !memcmp(out, _ctype_loc_style, sizeof(_ctype_loc_style)))
                    {
                        _Lcid_c[0].is_clike = TRUE;
                    }
                    else
                    {
                        _Lcid_c[0].is_clike = FALSE;
                    }
                }
                else
                {
                    _Lcid_c[0].is_clike = FALSE;
                }
                _Lcid_c[0].id = __lc_codepage;
            }
            __lc_clike = _Lcid_c[0].is_clike;
        }

        if ( category == LC_COLLATE )
        {
            __lc_collate_cp = cptemp;
        }

        if (__lc_category[category].init())
        {
            /* restore previous state! */
            __lc_category[category].locale = oldlocale;
            _free_crt(pch);
            __lc_handle[category] = oldhandle;
            __lc_codepage = oldcodepage;

            return NULL; /* error if non-zero return */
        }

        /* locale set up successfully */
        /* Cleanup */
        if ((oldlocale != _clocalestr)
            )
            _free_crt(oldlocale);

        return __lc_category[category].locale;

} /* _setlocale_set_cat */



static char * __cdecl _setlocale_get_all (
        void
        )
{
        int i;
        int same = 1;
        /* allocate memory if necessary */
        if ( (__lc_category[LC_ALL].locale == NULL) &&
             ((__lc_category[LC_ALL].locale =
               _malloc_crt((MAX_LC_LEN+1) * (LC_MAX-LC_MIN+1) + CATNAMES_LEN))
               == NULL) )
            return NULL;

        __lc_category[LC_ALL].locale[0] = '\0';
        for (i=LC_MIN+1; ; i++)
        {
            _strcats(__lc_category[LC_ALL].locale, 3, __lc_category[i].catname,"=",__lc_category[i].locale);
            if (i<LC_MAX)
            {
                strcat(__lc_category[LC_ALL].locale,";");
                if (strcmp(__lc_category[i].locale, __lc_category[i+1].locale))
                    same=0;
            }
            else
            {
                if (!same)
                    return __lc_category[LC_ALL].locale;
                else
                {
                    _free_crt(__lc_category[LC_ALL].locale);
                    __lc_category[LC_ALL].locale = (char *)NULL;
                    return __lc_category[LC_CTYPE].locale;
                }
            }
        }
} /* _setlocale_get_all */


char * _expandlocale (
        char *expr,
        char * output,
        LC_ID * id,
        UINT * cp,
        int category
        )
{
        static  LC_ID   cacheid = {0, 0, 0};
        static  UINT    cachecp = 0;
        static  char cachein[MAX_LC_LEN] = "C";
        static  char cacheout[MAX_LC_LEN] = "C";

        if (!expr)
            return NULL; /* error if no input */


        if (((*expr=='C') && (!expr[1]))
            )  /* for "C" locale, just return */
        {
            *output = 'C';
            output[1] = '\0';
            if (id)
            {
                id->wLanguage = 0;
                id->wCountry  = 0;
                id->wCodePage = 0;
            }
            if (cp)
            {
                *cp = CP_ACP; /* return to ANSI code page */
            }
            return output; /* "C" */
        }

        /* first, make sure we didn't just do this one */
        if (strlen(expr)>=MAX_LC_LEN-1 ||       /* we would never have cached this */
            (strcmp(cacheout,expr) && strcmp(cachein,expr)))
        {
            /* do some real work */
            LC_STRINGS names;
            const char *source=NULL;

            if (__lc_strtolc((LC_STRINGS *)&names, (const char *)expr))
                return NULL;  /* syntax error */

            if (!__get_qualified_locale((LPLC_STRINGS)&names,
                (LPLC_ID)&cacheid, (LPLC_STRINGS)&names))
                return NULL;    /* locale not recognized/supported */

            /* begin: cache atomic section */

            cachecp = cacheid.wCodePage;

            __lc_lctostr((char *)cacheout, &names);

            if (*expr && strlen(expr)<MAX_LC_LEN-1)
                        {
                source=expr;
                        }
            else
                        {
                /* Don't cache "" empty string or over-long string */
                source="";
                        }

                        /* Ensure that whatever we are about to copy in will be null terminated */
                        cachein[sizeof(cachein)-1]='\0';

            strncpy(cachein, source, sizeof(cachein)-1);

            /* end: cache atomic section */
        }
        if (id)
            memcpy((void *)id, (void *)&cacheid, sizeof(cacheid));   /* possibly return LC_ID */
        if (cp)
            memcpy((void *)cp, (void *)&cachecp, sizeof(cachecp));   /* possibly return cp */

        strcpy(output,cacheout);
        return cacheout; /* return fully expanded locale string */
}

/* helpers */

int __cdecl __init_dummy(void)  /* default routine for locale initializer */
{
        return 0;
}

void _strcats
        (
        char *outstr,
        int n,
        ...
        )
{
        int i;
        va_list substr;

        va_start (substr, n);

        for (i =0; i<n; i++)
        {
            strcat(outstr, va_arg(substr, char *));
        }
        va_end(substr);
}

int __lc_strtolc
   (
   LC_STRINGS *names,
   const char *locale
   )
{
        int i;
        size_t len;
        char ch;

        memset((void *)names, '\0', sizeof(LC_STRINGS));  /* clear out result */

        if (*locale=='\0')
            return 0; /* trivial case */

        /* only code page is given */
        if (locale[0] == '.' && locale[1] != '\0')
        {
            strncpy((char *)names->szCodePage, &locale[1], MAX_CP_LEN-1);
            /* Make sure to null terminate the string in case locale is > MAX_CP_LEN */
            names->szCodePage[ MAX_CP_LEN -1] = 0;
            return 0;
        }

        for (i=0; ; i++)
        {
            if (!(len=strcspn(locale,"_.,")))
                return -1;  /* syntax error */

            ch = locale[len];

            if ((i==0) && (len<MAX_LANG_LEN) && (ch!='.'))
                strncpy((char *)names->szLanguage, locale, len);

            else if ((i==1) && (len<MAX_CTRY_LEN) && (ch!='_'))
                strncpy((char *)names->szCountry, locale, len);

            else if ((i==2) && (len<MAX_CP_LEN) && (ch=='\0' || ch==','))
                strncpy((char *)names->szCodePage, locale, len);

            else
                return -1;  /* error parsing locale string */

            if (ch==',')
            {
                /* modifier not used in current implementation, but it
                   must be parsed to for POSIX/XOpen conformance */
            /*  strncpy(names->szModifier, locale, MAX_MODIFIER_LEN-1); */
                break;
            }

            if (!ch)
                break;
            locale+=(len+1);
        }
        return 0;
}

void __lc_lctostr
(
        char *locale,
        const LC_STRINGS *names
        )
{
        strcpy(locale, (char *)names->szLanguage);
        if (*(names->szCountry))
            _strcats(locale, 2, "_", names->szCountry);
        if (*(names->szCodePage))
            _strcats(locale, 2, ".", names->szCodePage);
/*      if (names->szModifier)
        _strcats(locale, 2, ",", names->szModifier); */
}


#endif  /* !defined (_WIN32) */
