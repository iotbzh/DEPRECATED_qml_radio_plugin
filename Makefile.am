bin_PROGRAMS =

all-local :

AM_CPPFLAGS = $(GCC_CPPFLAGS) -I$(top_srcdir)/radio
AM_CXXFLAGS = $(GCC_CXXFLAGS) -fPIC -std=c++11

CLEANFILES = radio/qml_radio.moc.h

moduledir = $(libdir)/qt5/qml/radio
module_LTLIBRARIES = libradioqtquick.la
libradioqtquick_la_LDFLAGS = -module -avoid-version
libradioqtquick_la_LIBADD = $(QT5QML_LIBS) $(RTLSDR_LIBS) $(ALSA_LIBS)
libradioqtquick_la_CXXFLAGS = $(AM_CXXFLAGS) $(QT5QML_CFLAGS) $(RTLSDR_CFLAGS) $(ALSA_CFLAGS)

libradioqtquick_la_SOURCES =		\
	qml_radio.cpp			\
	qml_radio_output.cpp		\
	qml_rtlsdr_radio.cpp

qml_radio.cpp : radio/qml_radio.moc.h

libradioqtquickdatadir = $(libdir)/qt5/qml/radio
dist_libradioqtquickdata_DATA =		\
	radio/qmldir

radio/qml_radio.moc.h : $(top_srcdir)/qml_radio.h
	$(AM_V_GEN)$(MOC) $< -o $@
