AC_INIT([libpolys], [4.0.3])

AC_CONFIG_MACRO_DIR([../m4])
AC_CONFIG_AUX_DIR([../build-aux])
AC_CONFIG_SRCDIR([reporter/reporter.h])
AC_CONFIG_HEADER([_config.h])

AM_MAINTAINER_MODE([enable])

AM_INIT_AUTOMAKE([-Wall foreign subdir-objects]) # -Wno-extra-portability -Werror silent-rules
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

dnl Check if build env is sane
AM_SANITY_CHECK

# Add pre'prefixed config
AX_PREFIX_CONFIG_H([libpolysconfig.h],[],[_config.h])

# - Check for CC and CXX but be careful about CFLAGS.
SING_RESET_FLAGS()
SING_CHECK_SET_ARGS()

AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LN_S
AC_PROG_INSTALL

# Turn off shared libraries during beta-testing, since they
# make the build process take too long.
LT_INIT
# ([shared])
# LT_INIT(dlopen disable-static) # doesn't work on PowerPC!

# Checks for libraries.
LB_CHECK_GMP(3.1.1,,AC_MSG_ERROR([Unable to find GMP on your machine: please use --with-gmp=PATH_TO_DIR_CONTAINING_LIB_AND_INCLUDE (see also ./configure --help if you do not understand what we are talking about)]))
LB_CHECK_NTL(5.0,,AC_MSG_WARN([Unable to find NTL (which is strongly recommended) on your machine: please use --with-ntl=PATH_TO_DIR_CONTAINING_LIB_AND_INCLUDE (see also ./configure --help if you do not understand what we are talking about)]))
LB_CHECK_FLINT(2.4,,AC_MSG_WARN([Unable to find FLINT (which is strongly recommended) on your machine: please use --with-flint=PATH_TO_DIR_CONTAINING_LIB_AND_INCLUDE (see also ./configure --help if you do not understand what we are talking about)]))


# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([memmove memset pow sqrt strchr setenv putenv])
AC_FUNC_VPRINTF
AC_FUNC_MALLOC
AC_CHECK_FUNC([vsnprintf], [AC_DEFINE([HAVE_VSNPRINTF], [1], [Define if vsnprintf exists.])])

# Checks for header files.
AC_HEADER_STDC
AC_STDC_HEADERS
AC_CHECK_HEADERS([stdlib.h string.h unistd.h pwd.h sys/param.h limits.h float.h execinfo.h])

# If the compiler supports GCC C++ ABI name demangling (has header
#   cxxabi.h and abi::__cxa_demangle() function), define
#   HAVE_GCC_ABI_DEMANGLE
AX_CXX_GCC_ABI_DEMANGLE

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# SING_CHECK_PIPE

# check for cpu properties
AC_CHECK_SIZEOF(long,4)
SING_CHECK_CPU

#check for host:
AC_CANONICAL_HOST

case $host_os in
  *cygwin* ) AX_APPEND_LINK_FLAGS([-Wl,-Bdynamic]);;
esac

dnl INCLUDES=""
dnl dnl OS specific flags and options (does work without the following:)
dnl case "$host" in
dnl     *-*-freebsd*)
dnl         LIBS="$LIBS -L/usr/local/lib -lc"
dnl 	CFLAGS="$CFLAGS -I/usr/local/include"
dnl 	INCLUDES="$INCLUDES -I/usr/local/include"
dnl 	;;
dnl esac
dnl AC_SUBST(INCLUDES)

SING_CHECK_OMALLOC()

ENABLE_RESOURCES="yes"

AC_ARG_VAR( [RESOURCES_INCLUDES], [INCLUDES for libresources] )
AC_ARG_VAR( [RESOURCES_LIBS], [LIBS for libresources] )

AC_MSG_CHECKING(whether libresources should be used)

if test "x$ENABLE_RESOURCES" = xyes;
then
  AC_MSG_RESULT(yes)

  AC_MSG_CHECKING([  RESOURCES_INCLUDES?..])
  AC_MSG_RESULT(${RESOURCES_INCLUDES:-unset})

  AC_MSG_CHECKING([  RESOURCES_LIBS?..])
  AC_MSG_RESULT(${RESOURCES_LIBS:-unset})

  PKG_REQUIRE="$PKG_REQUIRE resources"
  AC_SUBST(PKG_REQUIRE)
else
  AC_MSG_RESULT(no)
fi

AM_CONDITIONAL([ENABLE_RESOURCES],[test "x$ENABLE_RESOURCES" = xyes])

SING_CHECK_FACTORY()

SING_CHECK_P_PROCS
SING_CHECK_ARITH_RINGS
SING_CHECK_PLURAL

# AC_SUBST(PREFIX)
# AC_DEFINE_UNQUOTED(INSTALL_PREFIX,"$PREFIX",Prefix)

AC_DEFINE([SINGULAR],[1],[SINGULAR])
AC_DEFINE([DISABLE_GMP_CPP],[1],[DISABLE_GMP_CPP])
AC_DEFINE([NOSTREAMIO],[1],[DISABLE_GMP_CPP])

# AX_PYTHON_DEFAULT()
# AX_PYTHON_WITH_VERSION([2.4])

AC_DEFINE_UNQUOTED([NTL_CFLAGS],"$NTL_CFLAGS",[NTL_CFLAGS])
AC_DEFINE_UNQUOTED([NTL_LIBS],"$NTL_LIBS",[NTL_LIBS])

AC_DEFINE_UNQUOTED([GMP_CFLAGS],"$GMP_CFLAGS",[GMP_CFLAGS])
AC_DEFINE_UNQUOTED([GMP_LIBS],"$GMP_LIBS",[GMP_LIBS])

# AC_DEFINE_UNQUOTED([FLINT_CFLAGS],"$FLINT_CFLAGS",[FLINT_CFLAGS])
# AC_DEFINE_UNQUOTED([FLINT_LIBS],"$FLINT_LIBS",[FLINT_LIBS])

## AC_DEFINE([HAVE_POLYEXTENSIONS], [1], [Enable the algebraic & transcendental extensions])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([misc/Makefile])
AC_CONFIG_FILES([reporter/Makefile])
AC_CONFIG_FILES([coeffs/Makefile])
AC_CONFIG_FILES([polys/Makefile])
AC_CONFIG_FILES([tests/Makefile])
AC_CONFIG_FILES([libpolys-config libpolys.pc  misc/auxiliary.h])
AC_OUTPUT
