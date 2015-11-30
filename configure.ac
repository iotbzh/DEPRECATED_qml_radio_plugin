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

# Check for g++
AC_PROG_CXX

# Check for Qt moc
AC_CHECK_PROGS(MOC, [moc-qt5 moc])
AC_SUBST(MOC)

# Initialize libtool
LT_PREREQ([2.2])
LT_INIT([disable-static])

PKG_PROG_PKG_CONFIG()

AC_CHECK_FUNCS([strcasecmp])

AC_CHECK_LIB([pthread], [pthread_create])
AC_SUBST(PTHREAD_LIBS)

PKG_CHECK_MODULES(QT5QML, [Qt5Core >= 5.0 Qt5Qml >= 5.0])
PKG_CHECK_MODULES(RTLSDR, [librtlsdr >= 0.5.0])
PKG_CHECK_MODULES(ALSA, [alsa])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

AC_MSG_RESULT([
	Type 'make' to compile the AM/FM Radio QML plugin.
])
