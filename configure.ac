#
AC_INIT([OpenC37118], [1.0], [],[openc37118],[http://lsee.sel.eesc.usp.br/openc37118/])

AC_LANG(C++)

#libboost1.46
#libboost-thread1.46
#PKG_CHECK_MODULES([DEPS], [boost >= 1.38 libboost-thread >= 1.38 libc >= 6.0 libncurses >= 5.7 libcomedi >= 0.8])

#PKG_CHECK_MODULES([DEPS], [fuzzylite-1.0 >= 0.1 libxml++-2.6 >= 0.25 fann >= 0.26])
#AC_SUBST(DEPS_CFLAGS)
#AC_SUBST(DEPS_LIBS)
CPPFLAGS="$CPPFLAGS $DEPS_CFLAGS" 
LIBS="$DEPS_LIBS $LIBS" 

AC_PREREQ([2.59])
AM_INIT_AUTOMAKE([1.10 -Wall no-define])

AC_CONFIG_HEADERS([config.h])
AC_PROG_CXX

CXXFLAGS="-O0 -g3"
#CXXFLAGS="-O3"
#LT_INIT([disable-static])
LT_INIT

AC_SUBST([OPENC37118_SO_VERSION], [1:2:0])
AC_SUBST([OPENC37118_API_VERSION], [1.0])

AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi
AM_CONDITIONAL([HAVE_DOXYGEN], 
[test -n "$DOXYGEN"])AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([doc/Doxyfile])])

AC_CONFIG_FILES([Makefile
		src/Makefile
		doc/Makefile
                openc37118-${OPENC37118_API_VERSION}.pc:openc37118.pc.in])
AC_OUTPUT
