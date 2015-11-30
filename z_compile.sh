#!/bin/sh

# Note : output folder MUST be called "rien_ne_vaut" !!!
# (module name, defined in "qmldir" file)

OUTPUT_DIR=radio

moc qml_radio.h -o $OUTPUT_DIR/qml_radio.moc.h
#moc qml_rtlsdr_radio.h -o $OUTPUT_DIR/qml_rtlsdr_radio.moc.h
g++ -O0 -fPIC -std=c++11 -DQT_NO_PLUGIN_CHECK=1 qml_radio.cpp qml_radio_output.cpp qml_rtlsdr_radio.cpp $OUTPUT_DIR/qml_radio.moc.h -shared -o $OUTPUT_DIR/libradioqtquick.so `pkg-config --cflags --libs Qt5Qml librtlsdr alsa`
