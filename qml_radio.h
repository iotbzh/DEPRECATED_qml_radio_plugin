/*
 * A standalone AM/FM Radio QML plugin (for RTL2832U and Maxim hardware)
 * Copyright © 2015-2016 Manuel Bachmann <manuel.bachmann@iot.bzh>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QML_RADIO_H
#define QML_RADIO_H

#include <iostream>

#include <QObject>
#include <QString>
#include <QQmlExtensionPlugin>

class RadioImplementation;


 /* main class
  */
class Radio : public QObject
{
	Q_OBJECT
	Q_ENUMS(Mode)
	Q_PROPERTY(QString status READ status NOTIFY statusChanged)
	Q_PROPERTY(unsigned int num READ num WRITE setNum NOTIFY numChanged)
	Q_PROPERTY(Radio::Mode mode READ mode WRITE setMode NOTIFY modeChanged)
	Q_PROPERTY(float freq READ freq WRITE setFreq NOTIFY freqChanged)
	Q_PROPERTY(bool mute READ mute WRITE setMute NOTIFY muteChanged)

public:
	Radio(QObject*);
	enum Mode { FM, AM };

	QString status();
	unsigned int num(); void setNum(unsigned int);
	Radio::Mode mode(); void setMode(Radio::Mode);
	float freq();       void setFreq(float);
	bool mute();        void setMute(bool);
	Q_INVOKABLE void play();
	Q_INVOKABLE void stop();

signals:
	void statusChanged();
	void numChanged();
	void modeChanged();
	void freqChanged();
	void muteChanged();
	void playing();
	void stopped();

private:
	bool works();

	RadioImplementation *mRadio;
	unsigned int mDevCount;
	unsigned int mCurDev;
	Radio::Mode mCurMode;
	float mCurFreq;
	bool mCurMute;
};


 /* RadioImplementation is a virtual class, with 2 implementations :
  * - RtlSdrRadio (RTL2832U chipsets) ;
  * - MaximRadio  (**TODO !**) ;
  */
class RadioImplementation
{
public:
	RadioImplementation() {};
	virtual ~RadioImplementation() {};

	virtual unsigned int dev_count(void)                     = 0;
	virtual      QString dev_name(unsigned int)              = 0;
	virtual         bool dev_init(unsigned int)              = 0;
	virtual         bool dev_free(unsigned int)              = 0;
	virtual         void set_mode(unsigned int, Radio::Mode) = 0;
	virtual         void set_freq(unsigned int, float)       = 0;
	virtual         void set_mute(unsigned int, bool)        = 0;
	virtual         void     play(unsigned int)              = 0;
	virtual         void     stop(unsigned int)              = 0;
};


// -----------------

 /* QML plugin class
  */
class QmlRadioPlugin : public QQmlExtensionPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.automotive.qmlplugin")

public:
	void registerTypes(const char *uri);
};


#endif  // QML_RADIO_H
