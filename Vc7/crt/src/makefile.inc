PREOBJDIR = $(CPUDIR)\$(BLD_MODEL)_lib

# These are the prebuilt objects which are static link components in the
# MSVCR71[D].DLL implib

DLL_STAT_PREBLD = \
!if "$(TARGET_CPU)"=="i386"
        $(OBJDIR)\ehprolg2.obj \
        $(OBJDIR)\ehprolog.obj \
        $(OBJDIR)\ftol2.obj \
!endif
        $(OBJDIR)\ehvccctr.obj \
        $(OBJDIR)\ehvcccvb.obj \
        $(OBJDIR)\ehvecctr.obj \
        $(OBJDIR)\ehveccvb.obj \
        $(OBJDIR)\ehvecdtr.obj

# Here is the target for building LIBC{,D} / LIBCMT{,D} / MSVCRT{,D}.LIB

all: \
        $(OBJDIR)\convert.lib  \
        $(OBJDIR)\direct.lib   \
!if "$(BLD_DLL)"=="1"
        $(OBJDIR)\dllstuff.lib \
!endif
        $(OBJDIR)\dos.lib      \
        $(OBJDIR)\eh.lib       \
        $(OBJDIR)\exec.lib     \
        $(OBJDIR)\heap.lib     \
!if "$(TARGET_CPU)"=="i386"
        $(OBJDIR)\helper.lib   \
!endif
!if "$(BLD_DLL)"!="1"
        $(OBJDIR)\linkopts.lib \
!endif
        $(OBJDIR)\lowio.lib    \
        $(OBJDIR)\mbstring.lib \
        $(OBJDIR)\misc.lib     \
!if "$(RTC)"=="YES"
        $(OBJDIR)\rtc.lib      \
!endif
        $(OBJDIR)\startup.lib  \
        $(OBJDIR)\stdio.lib    \
        $(OBJDIR)\string.lib   \
        $(OBJDIR)\time.lib     \
        $(OBJDIR)\conv.lib     \
        $(OBJDIR)\tran.lib     \
!if "$(BLD_BSKU)"=="1"
        $(OBJDIR)\bsku.obj     \
!endif
        $(OBJDIR)\matherr.obj  \
!if "$(BLD_DLL)"=="1"
        $(DLL_STAT_PREBLD)
!endif


# Here is the target for building LIBCP{,D} / LIBCPMT{,D} / MSVCPRT{,D}.LIB

STDCPP_SRC = stdcpp

_stdcpp_ : \
!if "$(BLD_DLL)"=="1"
        $(OBJDIR)\newop_s.obj   \
        $(OBJDIR)\newaop_s.obj  \
!endif
!if "$(BLD_BSKU)"=="1"
        $(OBJDIR)\bsku.obj     \
!endif
        $(OBJDIR)\$(STDCPP_SRC).lib

CONVERT_OBJECTS = \
        $(OBJDIR)\_ctype.obj   \
        $(OBJDIR)\_fptostr.obj \
        $(OBJDIR)\_mbslen.obj  \
        $(OBJDIR)\_wctype.obj  \
        $(OBJDIR)\atof.obj     \
        $(OBJDIR)\atox.obj     \
        $(OBJDIR)\fcvt.obj     \
        $(OBJDIR)\gcvt.obj     \
        $(OBJDIR)\isctype.obj  \
        $(OBJDIR)\iswctype.obj \
        $(OBJDIR)\mblen.obj    \
        $(OBJDIR)\mbstowcs.obj \
        $(OBJDIR)\mbtowc.obj   \
        $(OBJDIR)\strtod.obj   \
        $(OBJDIR)\strtol.obj   \
        $(OBJDIR)\strtoq.obj   \
        $(OBJDIR)\swab.obj     \
        $(OBJDIR)\tolower.obj  \
        $(OBJDIR)\toupper.obj  \
        $(OBJDIR)\towlower.obj \
        $(OBJDIR)\towupper.obj \
        $(OBJDIR)\wchtodig.obj \
        $(OBJDIR)\wcstod.obj   \
        $(OBJDIR)\wcstol.obj   \
        $(OBJDIR)\wcstoq.obj   \
        $(OBJDIR)\wcstombs.obj \
        $(OBJDIR)\wctomb.obj   \
        $(OBJDIR)\wtof.obj     \
        $(OBJDIR)\wtox.obj     \
        $(OBJDIR)\xtoa.obj     \
        $(OBJDIR)\xtow.obj

DIRECT_OBJECTS = \
        $(OBJDIR)\drivemap.obj \
        $(OBJDIR)\drivfree.obj \
        $(OBJDIR)\findfile.obj \
        $(OBJDIR)\findf64.obj  \
        $(OBJDIR)\findfi64.obj \
        $(OBJDIR)\seterrm.obj  \
        $(OBJDIR)\slbeep.obj   \
!if "$(TARGET_CPU)"=="i386"
        $(OBJDIR)\enable.obj   \
!endif
        $(OBJDIR)\wfindfil.obj \
        $(OBJDIR)\wfndf64.obj  \
        $(OBJDIR)\wfndfi64.obj

DLLSTUFF_OBJECTS = \
        $(OBJDIR)\atonexit.obj \
        $(OBJDIR)\crtlib.obj   \
        $(OBJDIR)\crtexe.obj   \
        $(OBJDIR)\crtexew.obj  \
        $(OBJDIR)\crtdll.obj   \
        $(OBJDIR)\cinitexe.obj \
        $(OBJDIR)\dllargv.obj  \
        $(OBJDIR)\dll_argv.obj \
        $(OBJDIR)\merr.obj     \
        $(OBJDIR)\oldexcpt.obj \
        $(OBJDIR)\ti_inst.obj  \
        $(OBJDIR)\wcrtexe.obj  \
        $(OBJDIR)\wcrtexew.obj \
        $(OBJDIR)\wdllargv.obj \
        $(OBJDIR)\wdll_av.obj  \
        $(OBJDIR)\wildcard.obj \
        $(OBJDIR)\xtxtmode.obj \
        $(OBJDIR)\xncommod.obj \
!if "$(TARGET_CPU)"=="i386"
        $(OBJDIR)\dllsupp.obj  \
        $(OBJDIR)\adjustfd.obj \
!endif
!if "$(TARGET_CPU)"=="IA64"
        $(OBJDIR)\dllsupp.obj  \
!endif

DOS_OBJECTS = \
        $(OBJDIR)\access.obj   \
        $(OBJDIR)\chmod.obj    \
        $(OBJDIR)\chdir.obj    \
        $(OBJDIR)\dosmap.obj   \
        $(OBJDIR)\drive.obj    \
        $(OBJDIR)\fullpath.obj \
        $(OBJDIR)\getcwd.obj   \
        $(OBJDIR)\getpid.obj   \
        $(OBJDIR)\mkdir.obj    \
        $(OBJDIR)\mterrno.obj  \
        $(OBJDIR)\rename.obj   \
        $(OBJDIR)\rmdir.obj    \
        $(OBJDIR)\stat.obj     \
        $(OBJDIR)\stat64.obj   \
        $(OBJDIR)\stati64.obj  \
        $(OBJDIR)\unlink.obj   \
        $(OBJDIR)\waccess.obj  \
        $(OBJDIR)\wchdir.obj   \
        $(OBJDIR)\wchmod.obj   \
        $(OBJDIR)\wfullpat.obj \
        $(OBJDIR)\wgetcwd.obj  \
        $(OBJDIR)\wmkdir.obj   \
        $(OBJDIR)\wrename.obj  \
        $(OBJDIR)\wrmdir.obj   \
        $(OBJDIR)\wstat.obj    \
        $(OBJDIR)\wstat64.obj  \
        $(OBJDIR)\wstati64.obj \
        $(OBJDIR)\wunlink.obj

EXEC_OBJECTS = \
        $(OBJDIR)\cenvarg.obj  \
        $(OBJDIR)\fileinfo.obj \
        $(OBJDIR)\dospawn.obj  \
        $(OBJDIR)\execl.obj    \
        $(OBJDIR)\execle.obj   \
        $(OBJDIR)\execlp.obj   \
        $(OBJDIR)\execlpe.obj  \
        $(OBJDIR)\execv.obj    \
        $(OBJDIR)\execve.obj   \
        $(OBJDIR)\execvp.obj   \
        $(OBJDIR)\execvpe.obj  \
        $(OBJDIR)\getproc.obj  \
        $(OBJDIR)\loaddll.obj  \
        $(OBJDIR)\spawnl.obj   \
        $(OBJDIR)\spawnle.obj  \
        $(OBJDIR)\spawnlp.obj  \
        $(OBJDIR)\spawnlpe.obj \
        $(OBJDIR)\spawnv.obj   \
        $(OBJDIR)\spawnve.obj  \
        $(OBJDIR)\spawnvp.obj  \
        $(OBJDIR)\spawnvpe.obj \
        $(OBJDIR)\system.obj   \
        $(OBJDIR)\wait.obj     \
        $(OBJDIR)\wcenvarg.obj \
        $(OBJDIR)\wdospawn.obj \
        $(OBJDIR)\wexecl.obj   \
        $(OBJDIR)\wexecle.obj  \
        $(OBJDIR)\wexeclp.obj  \
        $(OBJDIR)\wexeclpe.obj \
        $(OBJDIR)\wexecv.obj   \
        $(OBJDIR)\wexecve.obj  \
        $(OBJDIR)\wexecvp.obj  \
        $(OBJDIR)\wexecvpe.obj \
        $(OBJDIR)\wspawnl.obj  \
        $(OBJDIR)\wspawnle.obj \
        $(OBJDIR)\wspawnlp.obj \
        $(OBJDIR)\wspwnlpe.obj \
        $(OBJDIR)\wspawnv.obj  \
        $(OBJDIR)\wspawnve.obj \
        $(OBJDIR)\wspawnvp.obj \
        $(OBJDIR)\wspwnvpe.obj \
        $(OBJDIR)\wsystem.obj

HEAP_OBJECTS = \
        $(OBJDIR)\align.obj    \
        $(OBJDIR)\resetstk.obj  \
        $(OBJDIR)\calloc.obj   \
!IF "$(WINHEAP)" == "YES"
        $(OBJDIR)\expand.obj   \
!ELSE
        $(OBJDIR)\findaddr.obj \
!ENDIF
        $(OBJDIR)\free.obj     \
        $(OBJDIR)\heapadd.obj  \
        $(OBJDIR)\heapchk.obj  \
!IF "$(WINHEAP)" == "NO"
        $(OBJDIR)\heapgrow.obj \
!ENDIF
        $(OBJDIR)\heapinit.obj \
        $(OBJDIR)\heapmin.obj  \
!IF "$(WINHEAP)" == "NO"
        $(OBJDIR)\heapprm.obj  \
!ENDIF
!IF "$(WINHEAP)" == "NO"
        $(OBJDIR)\heapsrch.obj \
!ENDIF
        $(OBJDIR)\hpabort.obj  \
        $(OBJDIR)\heapused.obj \
        $(OBJDIR)\heapwalk.obj \
        $(OBJDIR)\malloc.obj   \
        $(OBJDIR)\msize.obj    \
        $(OBJDIR)\realloc.obj  \
!IF "$(WINHEAP)" == "YES"
        $(OBJDIR)\sbheap.obj   \
!ENDIF
        $(OBJDIR)\handler.obj  \
        $(OBJDIR)\setnewh.obj  \
        $(OBJDIR)\new_mode.obj \
        $(OBJDIR)\nothrow0.obj \
        $(OBJDIR)\delete.obj   \
        $(OBJDIR)\delete2.obj  \
        $(OBJDIR)\_newmode.obj \
        $(OBJDIR)\new.obj      \
        $(OBJDIR)\new2.obj     \
        $(OBJDIR)\delaopnt.obj \
        $(OBJDIR)\delopnt.obj  \
        $(OBJDIR)\newaopnt.obj \
        $(OBJDIR)\newopnt.obj

HELPER_OBJECTS = \
!if "$(TARGET_CPU)"=="i386"
        $(OBJDIR)\lldiv.obj    \
        $(OBJDIR)\lldvrm.obj   \
        $(OBJDIR)\llmul.obj    \
        $(OBJDIR)\llrem.obj    \
        $(OBJDIR)\llshl.obj    \
        $(OBJDIR)\llshr.obj    \
        $(OBJDIR)\ulldiv.obj   \
        $(OBJDIR)\ulldvrm.obj  \
        $(OBJDIR)\ullrem.obj   \
        $(OBJDIR)\ullshr.obj   \
!endif

LINKOPTS_OBJECTS = \
!if "$(TARGET_CPU)"=="i386"
        $(OBJDIR)\fp10.obj     \
!endif
!if "$(TARGET_CPU)"=="IA64"
        $(OBJDIR)\fp10.obj     \
!endif
        $(OBJDIR)\binmode.obj  \
        $(OBJDIR)\commode.obj  \
        $(OBJDIR)\newmode.obj  \
        $(OBJDIR)\setargv.obj  \
        $(OBJDIR)\wsetargv.obj \
        $(OBJDIR)\noarg.obj    \
        $(OBJDIR)\nochkclr.obj \
        $(OBJDIR)\noenv.obj    \
        $(OBJDIR)\smalheap.obj \
        $(OBJDIR)\thrownew.obj

LOWIO_OBJECTS = \
        $(OBJDIR)\chsize.obj   \
        $(OBJDIR)\close.obj    \
        $(OBJDIR)\commit.obj   \
        $(OBJDIR)\creat.obj    \
        $(OBJDIR)\dup.obj      \
        $(OBJDIR)\dup2.obj     \
        $(OBJDIR)\eof.obj      \
        $(OBJDIR)\flength.obj  \
        $(OBJDIR)\fleni64.obj  \
        $(OBJDIR)\fstat.obj    \
        $(OBJDIR)\fstat64.obj  \
        $(OBJDIR)\fstati64.obj \
        $(OBJDIR)\initcon.obj  \
        $(OBJDIR)\ioinit.obj   \
        $(OBJDIR)\isatty.obj   \
        $(OBJDIR)\locking.obj  \
        $(OBJDIR)\lseek.obj    \
        $(OBJDIR)\lseeki64.obj \
        $(OBJDIR)\mktemp.obj   \
        $(OBJDIR)\open.obj     \
        $(OBJDIR)\osfinfo.obj  \
        $(OBJDIR)\pipe.obj     \
        $(OBJDIR)\read.obj     \
        $(OBJDIR)\setmode.obj  \
        $(OBJDIR)\tell.obj     \
        $(OBJDIR)\telli64.obj  \
        $(OBJDIR)\txtmode.obj  \
        $(OBJDIR)\write.obj    \
        $(OBJDIR)\cgets.obj    \
        $(OBJDIR)\cgetws.obj   \
        $(OBJDIR)\cputs.obj    \
        $(OBJDIR)\getch.obj    \
        $(OBJDIR)\getwch.obj   \
        $(OBJDIR)\putch.obj    \
        $(OBJDIR)\putwch.obj   \
        $(OBJDIR)\wcreat.obj   \
        $(OBJDIR)\wmktemp.obj  \
!if "$(TARGET_CPU)"=="i386"
        $(OBJDIR)\inp.obj      \
        $(OBJDIR)\outp.obj     \
!endif
        $(OBJDIR)\wopen.obj

MBSTRING_OBJECTS = \
        $(OBJDIR)\ismbalnm.obj \
        $(OBJDIR)\ismbalph.obj \
        $(OBJDIR)\ismbbyte.obj \
        $(OBJDIR)\ismbdgt.obj  \
        $(OBJDIR)\ismbgrph.obj \
        $(OBJDIR)\ismbknj.obj  \
        $(OBJDIR)\ismblgl.obj  \
        $(OBJDIR)\ismblwr.obj  \
        $(OBJDIR)\ismbprn.obj  \
        $(OBJDIR)\ismbpunc.obj \
        $(OBJDIR)\ismbsle.obj  \
        $(OBJDIR)\ismbspc.obj  \
        $(OBJDIR)\ismbstr.obj  \
        $(OBJDIR)\ismbupr.obj  \
        $(OBJDIR)\mbbtype.obj  \
        $(OBJDIR)\mbccpy.obj   \
        $(OBJDIR)\mbclen.obj   \
        $(OBJDIR)\mbclevel.obj \
        $(OBJDIR)\mbctype.obj  \
        $(OBJDIR)\mbsbtype.obj \
        $(OBJDIR)\mbschr.obj   \
        $(OBJDIR)\mbscmp.obj   \
        $(OBJDIR)\mbscoll.obj  \
        $(OBJDIR)\mbscspn.obj  \
        $(OBJDIR)\mbsdec.obj   \
        $(OBJDIR)\mbsicmp.obj  \
        $(OBJDIR)\mbsicoll.obj \
        $(OBJDIR)\mbsinc.obj   \
        $(OBJDIR)\mbslen.obj   \
        $(OBJDIR)\mbslwr.obj   \
        $(OBJDIR)\mbsnbcat.obj \
        $(OBJDIR)\mbsnbcmp.obj \
        $(OBJDIR)\mbsnbcnt.obj \
        $(OBJDIR)\mbsnbcol.obj \
        $(OBJDIR)\mbsnbcpy.obj \
        $(OBJDIR)\mbsnbicm.obj \
        $(OBJDIR)\mbsnbico.obj \
        $(OBJDIR)\mbsnbset.obj \
        $(OBJDIR)\mbsncat.obj  \
        $(OBJDIR)\mbsnccnt.obj \
        $(OBJDIR)\mbsncmp.obj  \
        $(OBJDIR)\mbsncoll.obj \
        $(OBJDIR)\mbsncpy.obj  \
        $(OBJDIR)\mbsnextc.obj \
        $(OBJDIR)\mbsnicmp.obj \
        $(OBJDIR)\mbsnicol.obj \
        $(OBJDIR)\mbsninc.obj  \
        $(OBJDIR)\mbsnset.obj  \
        $(OBJDIR)\mbspbrk.obj  \
        $(OBJDIR)\mbsrchr.obj  \
        $(OBJDIR)\mbsrev.obj   \
        $(OBJDIR)\mbsset.obj   \
        $(OBJDIR)\mbsspn.obj   \
        $(OBJDIR)\mbsspnp.obj  \
        $(OBJDIR)\mbsstr.obj   \
        $(OBJDIR)\mbstok.obj   \
        $(OBJDIR)\mbsupr.obj   \
        $(OBJDIR)\mbtohira.obj \
        $(OBJDIR)\mbtokata.obj \
        $(OBJDIR)\mbtolwr.obj  \
        $(OBJDIR)\mbtoupr.obj  \
        $(OBJDIR)\tojisjms.obj \
        $(OBJDIR)\tombbmbc.obj

MISC_OBJECTS = \
!if "$(TARGET_CPU)"=="i386"
        $(OBJDIR)\chkesp.obj   \
        $(OBJDIR)\eh3valid.obj \
        $(OBJDIR)\exsup.obj    \
        $(OBJDIR)\exsup2.obj   \
        $(OBJDIR)\exsup3.obj   \
        $(OBJDIR)\loadcfg.obj  \
        $(OBJDIR)\longjmp.obj  \
        $(OBJDIR)\secchk.obj   \
        $(OBJDIR)\seccinit.obj \
        $(OBJDIR)\seccook.obj  \
        $(OBJDIR)\secfail.obj  \
        $(OBJDIR)\sehprolg.obj \
        $(OBJDIR)\sehsupp.obj  \
        $(OBJDIR)\setjmp.obj   \
        $(OBJDIR)\setjmp3.obj  \
        $(OBJDIR)\setjmpex.obj \
!endif
!if "$(TARGET_CPU)"=="IA64"
        $(OBJDIR)\chandler.obj \
        $(OBJDIR)\cinitone.obj \
        $(OBJDIR)\jmpuwind.obj \
        $(OBJDIR)\longjmp.obj  \
        $(OBJDIR)\miscs.obj    \
        $(OBJDIR)\setjmp.obj   \
        $(OBJDIR)\setjmpex.obj \
!endif
        $(OBJDIR)\a_cmp.obj    \
        $(OBJDIR)\a_env.obj    \
        $(OBJDIR)\a_loc.obj    \
        $(OBJDIR)\a_map.obj    \
        $(OBJDIR)\a_str.obj    \
        $(OBJDIR)\abort.obj    \
        $(OBJDIR)\abs.obj      \
        $(OBJDIR)\assert.obj   \
        $(OBJDIR)\aw_com.obj   \
        $(OBJDIR)\bsearch.obj  \
        $(OBJDIR)\bswap.obj    \
        $(OBJDIR)\charmax.obj  \
        $(OBJDIR)\checkclr.obj \
        $(OBJDIR)\cmiscdat.obj \
        $(OBJDIR)\crtmbox.obj  \
        $(OBJDIR)\ctype.obj    \
        $(OBJDIR)\div.obj      \
        $(OBJDIR)\dbgheap.obj  \
        $(OBJDIR)\dbghook.obj  \
        $(OBJDIR)\dbgdel.obj   \
        $(OBJDIR)\dbgnew.obj   \
        $(OBJDIR)\dbgrpt.obj   \
        $(OBJDIR)\errmode.obj  \
        $(OBJDIR)\getenv.obj   \
        $(OBJDIR)\getpath.obj  \
        $(OBJDIR)\getqloc.obj  \
        $(OBJDIR)\initcoll.obj \
        $(OBJDIR)\initcrit.obj \
        $(OBJDIR)\initctyp.obj \
        $(OBJDIR)\inithelp.obj \
        $(OBJDIR)\initmon.obj  \
        $(OBJDIR)\initnum.obj  \
        $(OBJDIR)\inittime.obj \
        $(OBJDIR)\labs.obj     \
        $(OBJDIR)\lcnvinit.obj \
        $(OBJDIR)\lconv.obj    \
        $(OBJDIR)\ldiv.obj     \
        $(OBJDIR)\lfind.obj    \
        $(OBJDIR)\lsearch.obj  \
        $(OBJDIR)\makepath.obj \
        $(OBJDIR)\mbtowenv.obj \
        $(OBJDIR)\nlsdata1.obj \
        $(OBJDIR)\nlsdata2.obj \
        $(OBJDIR)\nlsdata3.obj \
        $(OBJDIR)\onexit.obj   \
        $(OBJDIR)\perror.obj   \
        $(OBJDIR)\purevirt.obj \
        $(OBJDIR)\putenv.obj   \
        $(OBJDIR)\qsort.obj    \
        $(OBJDIR)\rand.obj     \
        $(OBJDIR)\rotl.obj     \
        $(OBJDIR)\rotr.obj     \
        $(OBJDIR)\searchen.obj \
        $(OBJDIR)\setenv.obj   \
        $(OBJDIR)\setlocal.obj \
        $(OBJDIR)\splitpat.obj \
        $(OBJDIR)\_strerr.obj  \
        $(OBJDIR)\strerror.obj \
        $(OBJDIR)\syserr.obj   \
        $(OBJDIR)\convrtcp.obj \
        $(OBJDIR)\umask.obj    \
        $(OBJDIR)\w_cmp.obj    \
        $(OBJDIR)\w_env.obj    \
        $(OBJDIR)\w_loc.obj    \
        $(OBJDIR)\w_map.obj    \
        $(OBJDIR)\w_str.obj    \
        $(OBJDIR)\wcserror.obj \
        $(OBJDIR)\_wcserr.obj  \
        $(OBJDIR)\winsig.obj   \
        $(OBJDIR)\winxfltr.obj \
        $(OBJDIR)\wgetenv.obj  \
        $(OBJDIR)\wgetpath.obj \
        $(OBJDIR)\wmakepat.obj \
        $(OBJDIR)\wperror.obj  \
        $(OBJDIR)\wputenv.obj  \
        $(OBJDIR)\wsearche.obj \
        $(OBJDIR)\wsetenv.obj  \
        $(OBJDIR)\wsetloca.obj \
        $(OBJDIR)\wsplitpa.obj \
        $(OBJDIR)\wrt2err.obj  \
        $(OBJDIR)\wtombenv.obj

STARTUP_OBJECTS = \
        $(OBJDIR)\_setargv.obj \
        $(OBJDIR)\_wstargv.obj \
        $(OBJDIR)\chkstk.obj   \
        $(OBJDIR)\crt0.obj     \
        $(OBJDIR)\crt0dat.obj  \
        $(OBJDIR)\crt0fp.obj   \
        $(OBJDIR)\crt0init.obj \
        $(OBJDIR)\crt0msg.obj  \
        $(OBJDIR)\dllcrt0.obj  \
        $(OBJDIR)\dllmain.obj  \
        $(OBJDIR)\mlock.obj    \
        $(OBJDIR)\stdargv.obj  \
        $(OBJDIR)\stdenvp.obj  \
        $(OBJDIR)\thread.obj   \
        $(OBJDIR)\threadex.obj \
        $(OBJDIR)\tidtable.obj \
        $(OBJDIR)\tlssup.obj   \
        $(OBJDIR)\wcrt0.obj    \
        $(OBJDIR)\wild.obj     \
        $(OBJDIR)\wincmdln.obj \
        $(OBJDIR)\wincrt0.obj  \
        $(OBJDIR)\wstdargv.obj \
        $(OBJDIR)\wstdenvp.obj \
        $(OBJDIR)\wwncmdln.obj \
        $(OBJDIR)\wwild.obj    \
        $(OBJDIR)\wwincrt0.obj \
!if "$(TARGET_CPU)" == "i386"
        $(OBJDIR)\atlssup.obj  \
        $(OBJDIR)\fp8.obj      \
!endif
!if "$(TARGET_CPU)" == "IA64"
        $(OBJDIR)\fp8.obj      \
!endif


STDCPP_OBJECTS = \
        $(OBJDIR)\cerr.obj      \
        $(OBJDIR)\cin.obj       \
        $(OBJDIR)\clog.obj      \
        $(OBJDIR)\cout.obj      \
        $(OBJDIR)\fiopen.obj    \
!if "$(BLD_DLL)"=="1"
        $(OBJDIR)\instances.obj \
!endif
        $(OBJDIR)\iomanip.obj   \
        $(OBJDIR)\ios.obj       \
        $(OBJDIR)\iosptrs.obj   \
        $(OBJDIR)\iostream.obj  \
        $(OBJDIR)\locale.obj    \
        $(OBJDIR)\locale0.obj   \
        $(OBJDIR)\newaop.obj    \
        $(OBJDIR)\newop.obj     \
        $(OBJDIR)\nomemory.obj  \
        $(OBJDIR)\nothrow.obj   \
        $(OBJDIR)\raisehan.obj  \
        $(OBJDIR)\stdhndlr.obj  \
        $(OBJDIR)\stdthrow.obj  \
        $(OBJDIR)\string.obj    \
        $(OBJDIR)\strstrea.obj  \
        $(OBJDIR)\ushcerr.obj   \
        $(OBJDIR)\ushcout.obj   \
        $(OBJDIR)\ushcin.obj    \
        $(OBJDIR)\ushclog.obj   \
        $(OBJDIR)\ushiostr.obj  \
        $(OBJDIR)\uncaught.obj  \
        $(OBJDIR)\wcerr.obj     \
        $(OBJDIR)\wcin.obj      \
        $(OBJDIR)\wclog.obj     \
        $(OBJDIR)\wcout.obj     \
        $(OBJDIR)\wctrans.obj   \
        $(OBJDIR)\wctype.obj    \
        $(OBJDIR)\wiostrea.obj  \
        $(OBJDIR)\wlocale.obj   \
        $(OBJDIR)\xcosh.obj     \
        $(OBJDIR)\xdateord.obj  \
        $(OBJDIR)\xdebug.obj    \
        $(OBJDIR)\xdnorm.obj    \
        $(OBJDIR)\xdscale.obj   \
        $(OBJDIR)\xdtest.obj    \
        $(OBJDIR)\xexp.obj      \
        $(OBJDIR)\xfcosh.obj    \
        $(OBJDIR)\xfdnorm.obj   \
        $(OBJDIR)\xfdscale.obj  \
        $(OBJDIR)\xfdtest.obj   \
        $(OBJDIR)\xferaise.obj  \
        $(OBJDIR)\xfexp.obj     \
        $(OBJDIR)\xfsinh.obj    \
        $(OBJDIR)\xfvalues.obj  \
        $(OBJDIR)\xgetwctype.obj\
        $(OBJDIR)\xlcosh.obj    \
        $(OBJDIR)\xldnorm.obj   \
        $(OBJDIR)\xldscale.obj  \
        $(OBJDIR)\xldtest.obj   \
        $(OBJDIR)\xlexp.obj     \
        $(OBJDIR)\xlocale.obj   \
        $(OBJDIR)\xlock.obj     \
        $(OBJDIR)\xlpoly.obj    \
        $(OBJDIR)\xlsinh.obj    \
        $(OBJDIR)\xlvalues.obj  \
        $(OBJDIR)\xmbtowc.obj   \
        $(OBJDIR)\xmtx.obj      \
        $(OBJDIR)\xmutex.obj    \
        $(OBJDIR)\xpoly.obj     \
        $(OBJDIR)\xsinh.obj     \
        $(OBJDIR)\xstod.obj     \
        $(OBJDIR)\xstrcoll.obj  \
        $(OBJDIR)\xstrxfrm.obj  \
        $(OBJDIR)\xtowlower.obj \
        $(OBJDIR)\xtowupper.obj \
        $(OBJDIR)\xvalues.obj   \
        $(OBJDIR)\xwcscoll.obj  \
        $(OBJDIR)\xwcsxfrm.obj  \
        $(OBJDIR)\xwctomb.obj   \
        $(OBJDIR)\_tolower.obj  \
        $(OBJDIR)\_toupper.obj


STDIO_OBJECTS = \
        $(OBJDIR)\feoferr.obj  \
        $(OBJDIR)\fgetc.obj    \
        $(OBJDIR)\fgetchar.obj \
        $(OBJDIR)\fgets.obj    \
        $(OBJDIR)\fgetwc.obj   \
        $(OBJDIR)\fgetws.obj   \
        $(OBJDIR)\fgetwchr.obj \
        $(OBJDIR)\fileno.obj   \
        $(OBJDIR)\fputc.obj    \
        $(OBJDIR)\fputs.obj    \
        $(OBJDIR)\fputchar.obj \
        $(OBJDIR)\fputwc.obj   \
        $(OBJDIR)\fputws.obj   \
        $(OBJDIR)\fputwchr.obj \
        $(OBJDIR)\_filbuf.obj  \
        $(OBJDIR)\_filwbuf.obj \
        $(OBJDIR)\_file.obj    \
        $(OBJDIR)\_flsbuf.obj  \
        $(OBJDIR)\_flswbuf.obj \
        $(OBJDIR)\_freebuf.obj \
        $(OBJDIR)\_getbuf.obj  \
        $(OBJDIR)\_open.obj    \
        $(OBJDIR)\_sftbuf.obj  \
        $(OBJDIR)\clearerr.obj \
        $(OBJDIR)\closeall.obj \
        $(OBJDIR)\cprintf.obj  \
        $(OBJDIR)\cwprintf.obj  \
        $(OBJDIR)\cscanf.obj   \
        $(OBJDIR)\cwscanf.obj  \
        $(OBJDIR)\fclose.obj   \
        $(OBJDIR)\fdopen.obj   \
        $(OBJDIR)\fflush.obj   \
        $(OBJDIR)\fgetpos.obj  \
        $(OBJDIR)\fopen.obj    \
        $(OBJDIR)\fprintf.obj  \
        $(OBJDIR)\fread.obj    \
        $(OBJDIR)\freopen.obj  \
        $(OBJDIR)\fscanf.obj   \
        $(OBJDIR)\fseek.obj    \
        $(OBJDIR)\fseeki64.obj \
        $(OBJDIR)\fsetpos.obj  \
        $(OBJDIR)\ftell.obj    \
        $(OBJDIR)\ftelli64.obj \
        $(OBJDIR)\fwprintf.obj \
        $(OBJDIR)\fwrite.obj   \
        $(OBJDIR)\fwscanf.obj  \
        $(OBJDIR)\gets.obj     \
        $(OBJDIR)\getw.obj     \
        $(OBJDIR)\getws.obj    \
        $(OBJDIR)\input.obj    \
        $(OBJDIR)\output.obj   \
        $(OBJDIR)\popen.obj    \
        $(OBJDIR)\printf.obj   \
        $(OBJDIR)\puts.obj     \
        $(OBJDIR)\putw.obj     \
        $(OBJDIR)\putws.obj    \
        $(OBJDIR)\rewind.obj   \
        $(OBJDIR)\rmtmp.obj    \
        $(OBJDIR)\scanf.obj    \
        $(OBJDIR)\setmaxf.obj  \
        $(OBJDIR)\setbuf.obj   \
        $(OBJDIR)\setvbuf.obj  \
        $(OBJDIR)\snprintf.obj \
        $(OBJDIR)\snscanf.obj  \
        $(OBJDIR)\snwprint.obj \
        $(OBJDIR)\snwscanf.obj \
        $(OBJDIR)\sprintf.obj  \
        $(OBJDIR)\sscanf.obj   \
        $(OBJDIR)\stream.obj   \
        $(OBJDIR)\swprintf.obj \
        $(OBJDIR)\swprintn.obj \
        $(OBJDIR)\swscanf.obj  \
        $(OBJDIR)\tempnam.obj  \
        $(OBJDIR)\tmpfile.obj  \
        $(OBJDIR)\ungetc.obj   \
        $(OBJDIR)\ungetwc.obj  \
        $(OBJDIR)\vprintf.obj  \
        $(OBJDIR)\vfprintf.obj \
        $(OBJDIR)\vfwprint.obj \
        $(OBJDIR)\vsprintf.obj \
        $(OBJDIR)\vsnprint.obj \
        $(OBJDIR)\vsnwprnt.obj \
        $(OBJDIR)\vswprint.obj \
        $(OBJDIR)\vswprinn.obj \
        $(OBJDIR)\vwprintf.obj \
        $(OBJDIR)\wfdopen.obj  \
        $(OBJDIR)\wfopen.obj   \
        $(OBJDIR)\wfreopen.obj \
        $(OBJDIR)\winput.obj   \
        $(OBJDIR)\_wopen.obj   \
        $(OBJDIR)\woutput.obj  \
        $(OBJDIR)\wpopen.obj   \
        $(OBJDIR)\wprintf.obj  \
        $(OBJDIR)\wscanf.obj   \
        $(OBJDIR)\wtempnam.obj \
        $(OBJDIR)\wtmpfile.obj \
        $(OBJDIR)\ncommode.obj

STRING_OBJECTS = \
!if "$(TARGET_CPU)" == "IA64"
        $(OBJDIR)\memccpy.obj  \
        $(OBJDIR)\memchr.obj   \
        $(OBJDIR)\memcmp.obj   \
        $(OBJDIR)\memcpy.obj   \
        $(OBJDIR)\memicmp.obj  \
        $(OBJDIR)\memmove.obj  \
        $(OBJDIR)\memset.obj   \
        $(OBJDIR)\strcat.obj   \
        $(OBJDIR)\strchr.obj   \
        $(OBJDIR)\strcmp.obj   \
        $(OBJDIR)\strcpy.obj   \
        $(OBJDIR)\strcspn.obj  \
        $(OBJDIR)\strdup.obj   \
        $(OBJDIR)\stricmp.obj  \
        $(OBJDIR)\strlen.obj   \
        $(OBJDIR)\strncat.obj  \
        $(OBJDIR)\strncmp.obj  \
        $(OBJDIR)\strncpy.obj  \
        $(OBJDIR)\strnicmp.obj \
        $(OBJDIR)\strnset.obj  \
        $(OBJDIR)\strpbrk.obj  \
        $(OBJDIR)\strrchr.obj  \
        $(OBJDIR)\strrev.obj   \
        $(OBJDIR)\strset.obj   \
        $(OBJDIR)\strspn.obj   \
        $(OBJDIR)\strstr.obj   \
!endif
!if "$(TARGET_CPU)" == "i386"
        $(OBJDIR)\memcpy.obj   \
        $(OBJDIR)\memchr.obj   \
        $(OBJDIR)\memcmp.obj   \
        $(OBJDIR)\memccpy.obj  \
        $(OBJDIR)\_memicmp.obj \
        $(OBJDIR)\memicmp.obj  \
        $(OBJDIR)\memmove.obj  \
        $(OBJDIR)\memset.obj   \
        $(OBJDIR)\strcat.obj   \
        $(OBJDIR)\strchr.obj   \
        $(OBJDIR)\strcmp.obj   \
        $(OBJDIR)\strcspn.obj  \
        $(OBJDIR)\strdup.obj   \
        $(OBJDIR)\_stricmp.obj \
        $(OBJDIR)\stricmp.obj  \
        $(OBJDIR)\strlen.obj   \
        $(OBJDIR)\strncat.obj  \
        $(OBJDIR)\strncmp.obj  \
        $(OBJDIR)\strncpy.obj  \
        $(OBJDIR)\strnicmp.obj \
        $(OBJDIR)\_strnicm.obj \
        $(OBJDIR)\strnset.obj  \
        $(OBJDIR)\strpbrk.obj  \
        $(OBJDIR)\strrchr.obj  \
        $(OBJDIR)\strrev.obj   \
        $(OBJDIR)\strset.obj   \
        $(OBJDIR)\strspn.obj   \
        $(OBJDIR)\strstr.obj   \
!endif
        $(OBJDIR)\strcoll.obj  \
        $(OBJDIR)\stricoll.obj \
        $(OBJDIR)\strlwr.obj   \
        $(OBJDIR)\strncoll.obj \
        $(OBJDIR)\strnicol.obj \
        $(OBJDIR)\strtok.obj   \
        $(OBJDIR)\strupr.obj   \
        $(OBJDIR)\strxfrm.obj  \
        $(OBJDIR)\wcscat.obj   \
        $(OBJDIR)\wcschr.obj   \
        $(OBJDIR)\wcscmp.obj   \
        $(OBJDIR)\wcscoll.obj  \
        $(OBJDIR)\wcscspn.obj  \
        $(OBJDIR)\wcsdup.obj   \
        $(OBJDIR)\wcsicmp.obj  \
        $(OBJDIR)\wcsicoll.obj \
        $(OBJDIR)\wcslen.obj   \
        $(OBJDIR)\wcslwr.obj   \
        $(OBJDIR)\wcsncat.obj  \
        $(OBJDIR)\wcsncmp.obj  \
        $(OBJDIR)\wcsncoll.obj \
        $(OBJDIR)\wcsncpy.obj  \
        $(OBJDIR)\wcsnicmp.obj \
        $(OBJDIR)\wcsnicol.obj \
        $(OBJDIR)\wcsnset.obj  \
        $(OBJDIR)\wcspbrk.obj  \
        $(OBJDIR)\wcsrchr.obj  \
        $(OBJDIR)\wcsrev.obj   \
        $(OBJDIR)\wcsset.obj   \
        $(OBJDIR)\wcsspn.obj   \
        $(OBJDIR)\wcsstr.obj   \
        $(OBJDIR)\wcstok.obj   \
        $(OBJDIR)\wcsupr.obj   \
        $(OBJDIR)\wcsxfrm.obj

TIME_OBJECTS = \
        $(OBJDIR)\asctime.obj  \
        $(OBJDIR)\clock.obj    \
        $(OBJDIR)\ctime.obj    \
        $(OBJDIR)\days.obj     \
        $(OBJDIR)\difftime.obj \
        $(OBJDIR)\dtoxtime.obj \
        $(OBJDIR)\ftime.obj    \
        $(OBJDIR)\gmtime.obj   \
        $(OBJDIR)\localtim.obj \
        $(OBJDIR)\mktime.obj   \
        $(OBJDIR)\strftime.obj \
        $(OBJDIR)\strdate.obj  \
        $(OBJDIR)\strtime.obj  \
        $(OBJDIR)\time.obj     \
        $(OBJDIR)\timeset.obj  \
        $(OBJDIR)\tzset.obj    \
        $(OBJDIR)\utime.obj    \
        $(OBJDIR)\ctime64.obj  \
        $(OBJDIR)\dtoxtm64.obj \
        $(OBJDIR)\ftime64.obj  \
        $(OBJDIR)\gmtime64.obj \
        $(OBJDIR)\loctim64.obj \
        $(OBJDIR)\mktime64.obj \
        $(OBJDIR)\systime.obj  \
        $(OBJDIR)\time64.obj   \
        $(OBJDIR)\utime64.obj  \
        $(OBJDIR)\wcsftime.obj \
        $(OBJDIR)\wasctime.obj \
        $(OBJDIR)\wctime.obj   \
        $(OBJDIR)\wctime64.obj \
        $(OBJDIR)\wstrdate.obj \
        $(OBJDIR)\wstrtime.obj \
        $(OBJDIR)\wutime.obj   \
        $(OBJDIR)\wutime64.obj


$(OBJDIR)\convert.lib: $(CONVERT_OBJECTS)
    $(LIB) -out:$@ @<<$*.rsp
$(CONVERT_OBJECTS: =^
)
<<keep


$(OBJDIR)\direct.lib: $(DIRECT_OBJECTS)
    $(LIB) -out:$@ @<<$*.rsp
$(DIRECT_OBJECTS: =^
)
<<keep


$(OBJDIR)\dllstuff.lib: $(DLLSTUFF_OBJECTS)
    $(LIB) -out:$@ @<<$*.rsp
$(DLLSTUFF_OBJECTS: =^
)
<<keep


$(OBJDIR)\dos.lib: $(DOS_OBJECTS)
    $(LIB) -out:$@ @<<$*.rsp
$(DOS_OBJECTS: =^
)
<<keep


$(OBJDIR)\exec.lib: $(EXEC_OBJECTS)
    $(LIB) -out:$@ @<<$*.rsp
$(EXEC_OBJECTS: =^
)
<<keep


$(OBJDIR)\heap.lib: $(HEAP_OBJECTS)
    $(LIB) -out:$@ @<<$*.rsp
$(HEAP_OBJECTS: =^
)
<<keep


$(OBJDIR)\helper.lib: $(HELPER_OBJECTS)
    $(LIB) -out:$@ @<<$*.rsp
$(HELPER_OBJECTS: =^
)
<<keep


$(OBJDIR)\linkopts.lib: $(LINKOPTS_OBJECTS)
    $(LIB) -out:$@ @<<$*.rsp
$(LINKOPTS_OBJECTS: =^
)
<<keep


$(OBJDIR)\lowio.lib: $(LOWIO_OBJECTS)
    $(LIB) -out:$@ @<<$*.rsp
$(LOWIO_OBJECTS: =^
)
<<keep


$(OBJDIR)\mbstring.lib: $(MBSTRING_OBJECTS)
    $(LIB) -out:$@ @<<$*.rsp
$(MBSTRING_OBJECTS: =^
)
<<keep


$(OBJDIR)\misc.lib: $(MISC_OBJECTS)
    $(LIB) -out:$@ @<<$*.rsp
$(MISC_OBJECTS: =^
)
<<keep


$(OBJDIR)\startup.lib: $(STARTUP_OBJECTS)
    $(LIB) -out:$@ @<<$*.rsp
$(STARTUP_OBJECTS: =^
)
<<keep


$(OBJDIR)\$(STDCPP_SRC).lib: $(STDCPP_OBJECTS)
    $(LIB) -out:$@ @<<$*.rsp
$(STDCPP_OBJECTS: =^
)
<<keep


$(OBJDIR)\stdio.lib: $(STDIO_OBJECTS)
    $(LIB) -out:$@ @<<$*.rsp
$(STDIO_OBJECTS: =^
)
<<keep


$(OBJDIR)\string.lib: $(STRING_OBJECTS)
    $(LIB) -out:$@ @<<$*.rsp
$(STRING_OBJECTS: =^
)
<<keep


$(OBJDIR)\time.lib: $(TIME_OBJECTS)
    $(LIB) -out:$@ @<<$*.rsp
$(TIME_OBJECTS: =^
)
<<keep


$(OBJDIR)\eh.lib: $(PREOBJDIR)\eh.lib
        copy $(PREOBJDIR)\eh.lib $@

!if "$(RTC)"=="YES"
$(OBJDIR)\rtc.lib: $(PREOBJDIR)\rtc.lib
        copy $(PREOBJDIR)\rtc.lib $@
!endif

$(OBJDIR)\conv.lib: $(PREOBJDIR)\conv.lib
        copy $(PREOBJDIR)\conv.lib $@

$(OBJDIR)\tran.lib: $(PREOBJDIR)\tran.lib
        copy $(PREOBJDIR)\tran.lib $@

$(OBJDIR)\chkesp.obj: $(PREOBJDIR)\chkesp.obj
        copy $(PREOBJDIR)\chkesp.obj $@

$(OBJDIR)\ehvccctr.obj: $(PREOBJDIR)\ehvccctr.obj
        copy $(PREOBJDIR)\ehvccctr.obj $@

$(OBJDIR)\ehvcccvb.obj: $(PREOBJDIR)\ehvcccvb.obj
        copy $(PREOBJDIR)\ehvcccvb.obj $@

$(OBJDIR)\ehvecctr.obj: $(PREOBJDIR)\ehvecctr.obj
        copy $(PREOBJDIR)\ehvecctr.obj $@

$(OBJDIR)\ehveccvb.obj: $(PREOBJDIR)\ehveccvb.obj
        copy $(PREOBJDIR)\ehveccvb.obj $@

$(OBJDIR)\ehvecdtr.obj: $(PREOBJDIR)\ehvecdtr.obj
        copy $(PREOBJDIR)\ehvecdtr.obj $@

$(OBJDIR)\oldexcpt.obj: $(PREOBJDIR)\oldexcpt.obj
        copy $(PREOBJDIR)\oldexcpt.obj $@

$(OBJDIR)\sehprolg.obj: $(PREOBJDIR)\sehprolg.obj
        copy $(PREOBJDIR)\sehprolg.obj $@

$(OBJDIR)\sehsupp.obj: $(PREOBJDIR)\sehsupp.obj
        copy $(PREOBJDIR)\sehsupp.obj $@

$(OBJDIR)\setjmp.obj: $(PREOBJDIR)\setjmp.obj
        copy $(PREOBJDIR)\setjmp.obj $@


!if "$(TARGET_CPU)" == "i386"

$(OBJDIR)\atlssup.obj: $(PREOBJDIR)\atlssup.obj
        copy $(PREOBJDIR)\atlssup.obj $@

$(OBJDIR)\bsku.obj: $(PREOBJDIR)\bsku.obj
        copy $(PREOBJDIR)\bsku.obj $@

$(OBJDIR)\ehprolg2.obj: $(PREOBJDIR)\ehprolg2.obj
        copy $(PREOBJDIR)\ehprolg2.obj $@

$(OBJDIR)\ehprolog.obj: $(PREOBJDIR)\ehprolog.obj
        copy $(PREOBJDIR)\ehprolog.obj $@

$(OBJDIR)\eh3valid.obj: $(PREOBJDIR)\eh3valid.obj
        copy $(PREOBJDIR)\eh3valid.obj $@

$(OBJDIR)\exsup.obj: $(PREOBJDIR)\exsup.obj
        copy $(PREOBJDIR)\exsup.obj $@

$(OBJDIR)\exsup2.obj: $(PREOBJDIR)\exsup2.obj
        copy $(PREOBJDIR)\exsup2.obj $@

$(OBJDIR)\exsup3.obj: $(PREOBJDIR)\exsup3.obj
        copy $(PREOBJDIR)\exsup3.obj $@

$(OBJDIR)\ftol2.obj: $(PREOBJDIR)\ftol2.obj
        copy $(PREOBJDIR)\ftol2.obj $@

$(OBJDIR)\longjmp.obj: $(PREOBJDIR)\longjmp.obj
        copy $(PREOBJDIR)\longjmp.obj $@

$(OBJDIR)\matherr.obj: $(PREOBJDIR)\matherr.obj
        copy $(PREOBJDIR)\matherr.obj $@

$(OBJDIR)\setjmp3.obj: $(PREOBJDIR)\setjmp3.obj
        copy $(PREOBJDIR)\setjmp3.obj $@

$(OBJDIR)\setjmpex.obj: $(PREOBJDIR)\setjmpex.obj
        copy $(PREOBJDIR)\setjmpex.obj $@

$(OBJDIR)\tlssup.obj: $(PREOBJDIR)\tlssup.obj
        copy $(PREOBJDIR)\tlssup.obj $@

########
#
# These objects can be built from (assembler) source by editing
# makefile.sub so that the symbol BLD_ASM is set to the value 1.
# To assembler these objects, you must have MASM 6.11a or later.
#
########

!if "$(BLD_ASM)"=="0"

$(OBJDIR)\chkstk.obj: $(PREOBJDIR)\chkstk.obj
        copy $(PREOBJDIR)\chkstk.obj $@

$(OBJDIR)\dllsupp.obj: $(PREOBJDIR)\dllsupp.obj
        copy $(PREOBJDIR)\dllsupp.obj $@

$(OBJDIR)\enable.obj: $(PREOBJDIR)\enable.obj
        copy $(PREOBJDIR)\enable.obj $@

$(OBJDIR)\inp.obj: $(PREOBJDIR)\inp.obj
        copy $(PREOBJDIR)\inp.obj $@

$(OBJDIR)\lldiv.obj: $(PREOBJDIR)\lldiv.obj
        copy $(PREOBJDIR)\lldiv.obj $@

$(OBJDIR)\lldvrm.obj: $(PREOBJDIR)\lldvrm.obj
        copy $(PREOBJDIR)\lldvrm.obj $@

$(OBJDIR)\llmul.obj: $(PREOBJDIR)\llmul.obj
        copy $(PREOBJDIR)\llmul.obj $@

$(OBJDIR)\llrem.obj: $(PREOBJDIR)\llrem.obj
        copy $(PREOBJDIR)\llrem.obj $@

$(OBJDIR)\llshl.obj: $(PREOBJDIR)\llshl.obj
        copy $(PREOBJDIR)\llshl.obj $@

$(OBJDIR)\llshr.obj: $(PREOBJDIR)\llshr.obj
        copy $(PREOBJDIR)\llshr.obj $@

$(OBJDIR)\memccpy.obj: $(PREOBJDIR)\memccpy.obj
        copy $(PREOBJDIR)\memccpy.obj $@

$(OBJDIR)\memchr.obj: $(PREOBJDIR)\memchr.obj
        copy $(PREOBJDIR)\memchr.obj $@

$(OBJDIR)\memcmp.obj: $(PREOBJDIR)\memcmp.obj
        copy $(PREOBJDIR)\memcmp.obj $@

$(OBJDIR)\memcpy.obj: $(PREOBJDIR)\memcpy.obj
        copy $(PREOBJDIR)\memcpy.obj $@

$(OBJDIR)\_memicmp.obj: $(PREOBJDIR)\_memicmp.obj
        copy $(PREOBJDIR)\_memicmp.obj $@

$(OBJDIR)\memmove.obj: $(PREOBJDIR)\memmove.obj
        copy $(PREOBJDIR)\memmove.obj $@

$(OBJDIR)\memset.obj: $(PREOBJDIR)\memset.obj
        copy $(PREOBJDIR)\memset.obj $@

$(OBJDIR)\outp.obj: $(PREOBJDIR)\outp.obj
        copy $(PREOBJDIR)\outp.obj $@

$(OBJDIR)\strcat.obj: $(PREOBJDIR)\strcat.obj
        copy $(PREOBJDIR)\strcat.obj $@

$(OBJDIR)\strchr.obj: $(PREOBJDIR)\strchr.obj
        copy $(PREOBJDIR)\strchr.obj $@

$(OBJDIR)\strcmp.obj: $(PREOBJDIR)\strcmp.obj
        copy $(PREOBJDIR)\strcmp.obj $@

$(OBJDIR)\strcspn.obj: $(PREOBJDIR)\strcspn.obj
        copy $(PREOBJDIR)\strcspn.obj $@

$(OBJDIR)\strdup.obj: $(PREOBJDIR)\strdup.obj
        copy $(PREOBJDIR)\strdup.obj $@

$(OBJDIR)\_stricmp.obj: $(PREOBJDIR)\_stricmp.obj
        copy $(PREOBJDIR)\_stricmp.obj $@

$(OBJDIR)\strlen.obj: $(PREOBJDIR)\strlen.obj
        copy $(PREOBJDIR)\strlen.obj $@

$(OBJDIR)\strncat.obj: $(PREOBJDIR)\strncat.obj
        copy $(PREOBJDIR)\strncat.obj $@

$(OBJDIR)\strncmp.obj: $(PREOBJDIR)\strncmp.obj
        copy $(PREOBJDIR)\strncmp.obj $@

$(OBJDIR)\strncpy.obj: $(PREOBJDIR)\strncpy.obj
        copy $(PREOBJDIR)\strncpy.obj $@

$(OBJDIR)\_strnicm.obj: $(PREOBJDIR)\_strnicm.obj
        copy $(PREOBJDIR)\_strnicm.obj $@

$(OBJDIR)\strnset.obj: $(PREOBJDIR)\strnset.obj
        copy $(PREOBJDIR)\strnset.obj $@

$(OBJDIR)\strpbrk.obj: $(PREOBJDIR)\strpbrk.obj
        copy $(PREOBJDIR)\strpbrk.obj $@

$(OBJDIR)\strrchr.obj: $(PREOBJDIR)\strrchr.obj
        copy $(PREOBJDIR)\strrchr.obj $@

$(OBJDIR)\strrev.obj: $(PREOBJDIR)\strrev.obj
        copy $(PREOBJDIR)\strrev.obj $@

$(OBJDIR)\strset.obj: $(PREOBJDIR)\strset.obj
        copy $(PREOBJDIR)\strset.obj $@

$(OBJDIR)\strspn.obj: $(PREOBJDIR)\strspn.obj
        copy $(PREOBJDIR)\strspn.obj $@

$(OBJDIR)\strstr.obj: $(PREOBJDIR)\strstr.obj
        copy $(PREOBJDIR)\strstr.obj $@

$(OBJDIR)\ulldiv.obj: $(PREOBJDIR)\ulldiv.obj
        copy $(PREOBJDIR)\ulldiv.obj $@

$(OBJDIR)\ulldvrm.obj: $(PREOBJDIR)\ulldvrm.obj
        copy $(PREOBJDIR)\ulldvrm.obj $@

$(OBJDIR)\ullrem.obj: $(PREOBJDIR)\ullrem.obj
        copy $(PREOBJDIR)\ullrem.obj $@

$(OBJDIR)\ullshr.obj    : $(PREOBJDIR)\ullshr.obj
        copy $(PREOBJDIR)\ullshr.obj $@

!endif # BLD_ASM==0
!endif # TARGET_CPU == i386

!if "$(TARGET_CPU)" == "IA64"

$(OBJDIR)\chandler.obj: $(PREOBJDIR)\chandler.obj
        copy $(PREOBJDIR)\chandler.obj $@

$(OBJDIR)\cinitone.obj: $(PREOBJDIR)\cinitone.obj
        copy $(PREOBJDIR)\cinitone.obj $@

$(OBJDIR)\jmpuwind.obj: $(PREOBJDIR)\jmpuwind.obj
        copy $(PREOBJDIR)\jmpuwind.obj $@

$(OBJDIR)\longjmp.obj: $(PREOBJDIR)\longjmp.obj
        copy $(PREOBJDIR)\longjmp.obj $@

$(OBJDIR)\miscs.obj: $(PREOBJDIR)\miscs.obj
        copy $(PREOBJDIR)\miscs.obj $@

$(OBJDIR)\setjmpex.obj: $(PREOBJDIR)\setjmpex.obj
        copy $(PREOBJDIR)\setjmpex.obj $@

$(OBJDIR)\strcat.obj: $(PREOBJDIR)\strcat.obj
        copy $(PREOBJDIR)\strcat.obj $@

$(OBJDIR)\strcpy.obj: $(PREOBJDIR)\strcpy.obj
        copy $(PREOBJDIR)\strcpy.obj $@

$(OBJDIR)\strcmp.obj: $(PREOBJDIR)\strcmp.obj
        copy $(PREOBJDIR)\strcmp.obj $@

$(OBJDIR)\strlen.obj: $(PREOBJDIR)\strlen.obj
        copy $(PREOBJDIR)\strlen.obj $@

$(OBJDIR)\memcpy.obj: $(PREOBJDIR)\memcpy.obj
        copy $(PREOBJDIR)\memcpy.obj $@

$(OBJDIR)\memmove.obj: $(PREOBJDIR)\memmove.obj
        copy $(PREOBJDIR)\memmove.obj $@

$(OBJDIR)\memset.obj: $(PREOBJDIR)\memset.obj
        copy $(PREOBJDIR)\memset.obj $@

$(OBJDIR)\chkstk.obj: $(PREOBJDIR)\chkstk.obj
        copy $(PREOBJDIR)\chkstk.obj $@

$(OBJDIR)\tlssup.obj: $(PREOBJDIR)\tlssup.obj
        copy $(PREOBJDIR)\tlssup.obj $@

$(OBJDIR)\matherr.obj: $(PREOBJDIR)\matherr.obj
        copy $(PREOBJDIR)\matherr.obj $@

!endif
