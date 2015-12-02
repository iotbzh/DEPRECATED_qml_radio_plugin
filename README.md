AM/FM Radio QML Plugin
======================

<strong>A Qt5/QML plugin for AM/FM radio devices (RTL2832U...)</strong>


## Description

 The <strong>AM/FM Radio QML Plugin</strong> provides a simple interface for detecting, initializing, and using AM/FM radio devices within QML code. It ships a sample QML application for demonstration purposes.

 It intends to support multiple radio chipsets, and the ALSA/PulseAudio audio backends.
 Currently, it only supports <strong>Realtek RTL2832U-based chipsets</strong>, and the ALSA audio backend. For this purpose, it heavily depends on the RTL-SDR library.

![QMLRadioPlugin screenshots](http://www.tarnyko.net/repo/qmlradio_plugin.png)

## Requirements

You will need the following tools & libraries :

* GNU g++ (> 4.7) ;
* Qt (>= 5.0) ;
* rtl-sdr (>= 0.5.0) <i>(clone it from "git://git.osmocom.org/rtl-sdr")</i> ;
* alsa.

## Installation

Follow these steps :

<strong>$ autoreconf --install <br />
./configure <br />
make <br />
make install </strong>

## Running

Type the following command :

<strong>$ qmlscene radio.qml</strong>

<i>(if the plugin has been installed to a non-standard location, such as "/opt" : <br />
<strong>$ qmlscene -I /opt/lib/qt5/qml radio.qml</strong> )</i>
