AC_INIT([suisha], [201809])
AM_INIT_AUTOMAKE([foreign nostdinc dist-bzip2 no-dist-gzip])
AC_CONFIG_SRCDIR([src/loop.cc])

PKG_CHECK_MODULES([DBUS], [dbus-1 >= 1.4])
XEMMAI_USE=xemmai-201809
PKG_CHECK_MODULES([XEMMAI], [${XEMMAI_USE}], [
	XEMMAI_BIN_PATH=`pkg-config --variable=bin_path ${XEMMAI_USE}`
	XEMMAI_MODULE_PATH=`pkg-config --variable=module_path ${XEMMAI_USE}`
])
AC_SUBST(XEMMAI_BIN_PATH)
AC_SUBST(XEMMAI_MODULE_PATH)

if test "${CXXFLAGS+set}" != set; then
	CXXFLAGS=
fi
AC_PROG_CXX
AM_DISABLE_STATIC
AM_PROG_LIBTOOL

AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T

AC_ARG_ENABLE(
	[debug],
	AS_HELP_STRING([--enable-debug], [turn on debugging]),
	[case "${enableval}" in
	  yes) debug=true ;;
	  no) debug=false ;;
	  *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
	 esac],
	[debug=false]
)
AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])
AC_ARG_ENABLE(
	[profile],
	AS_HELP_STRING([--enable-profile], [turn on profiling]),
	[case "${enableval}" in
	  yes) profile=true ;;
	  no) profile=false ;;
	  *) AC_MSG_ERROR([bad value ${enableval} for --enable-profile]) ;;
	 esac],
	[profile=false]
)
AM_CONDITIONAL([PROFILE], [test x$profile = xtrue])

AC_CONFIG_HEADERS([include/suisha/configure.h])
AC_CONFIG_FILES([
	Makefile
	include/Makefile
	src/Makefile
	xemmai/Makefile
	test/Makefile
	examples/Makefile
])
AC_OUTPUT
