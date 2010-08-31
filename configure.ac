AC_PREREQ(2.61)

AC_INIT([system-core],1.0.0)

AM_INIT_AUTOMAKE([-Wall gnu foreign])

AM_MAINTAINER_MODE

AC_CONFIG_SRCDIR([liblog/logd_write.c])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AM_PROG_AS
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

AC_CHECK_HEADERS([sys/uio.h])

#
# Build all components by default
#
# To add new components: add AC_ARG_ENABLE, AM_CONDITIONAL and update AC_OUTPUT
#

AC_ARG_ENABLE(liblog,
        AC_HELP_STRING([--disable-liblog], [Disable liblog]),
        [LIBLOG=no],
        [LIBLOG=yes])
AM_CONDITIONAL(USELIBLOG, [test x$LIBLOG = xyes])

AC_OUTPUT([ \
	Makefile \
	liblog/Makefile
	])
