m4_define([qmlradio_version], 0.1)

AC_PREREQ([2.64])
AC_INIT([qmlradio],
        [qmlradio_version],
        [https://gerrit.automotivelinux.org/],
        [qmlradio],
        [http://www.automotivelinux.org/])

AC_SUBST([QMLRADIO_VERSION], [qmlradio_version])

AC_CONFIG_AUX_DIR([build-aux])

AC_USE_SYSTEM_EXTENSIONS
AC_SYS_LARGEFILE

AM_INIT_AUTOMAKE([1.11 parallel-tests foreign no-dist-gzip dist-xz color-tests subdir-objects])

AM_SILENT_RULES([yes])

# Optional custom path for Qt moc
AC_ARG_WITH(moc-dir,
            AS_HELP_STRING([--with-moc-dir=DIR],[path to the Qt5 "moc" binary]),
            [],[with_moc_dir="$PATH"])
MOC_DIR=$with_moc_dir

# Check for g++
AC_PROG_CXX

# Check for Qt moc
AC_CHECK_PROGS(MOC, [moc-qt5 moc], [no], [$MOC_DIR])
if test "x$MOC" = "xno"; then
	AC_MSG_ERROR([Qt5 "moc" not found])
fi
# Final variable should be empty or end with "/"
if test "x$MOC_DIR" = "x$PATH"; then
	MOC_DIR=""
else
	MOC_DIR="$MOC_DIR/"
fi
AC_SUBST(MOC)
AC_SUBST(MOC_DIR)

# Initialize libtool
LT_PREREQ([2.2])
LT_INIT([disable-static])

# Check functions, libs, packages
PKG_PROG_PKG_CONFIG()

AC_CHECK_FUNCS([strcasecmp])

AC_CHECK_LIB([pthread], [pthread_create])
AC_SUBST(PTHREAD_LIBS)

PKG_CHECK_MODULES(QT5QML, [Qt5Core >= 5.0 Qt5Qml >= 5.0])
PKG_CHECK_MODULES(RTLSDR, [librtlsdr >= 0.5.0])
PKG_CHECK_MODULES(ALSA, [alsa])
PKG_CHECK_MODULES(PULSE, [libpulse-simple])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

AC_MSG_RESULT([
	Type 'make' to compile the AM/FM Radio QML plugin.
])
