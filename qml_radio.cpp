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

#include <QtQml/qqml.h>

#include "qml_radio.moc.h"
#include "qml_rtlsdr_radio.h"
 //#include "qml_maxim_radio.h"

Radio::Radio(QObject *parent=0) : QObject(parent),
				  mRadio(NULL),
				  mDevCount(0),
				  mCurDev(0),
				  mCurMode(FM),
				  mCurFreq(100.0),
				  mCurMute(false)
{
	/*
	char *impl_env = getenv("QML_RADIO_HARDWARE");
	mRadio = NULL;

	if (impl_env) {
		if (strcasecmp(impl_env), "Maxim") == 0)
			mRadio = new MaximRadio();
		if (strcasecmp(impl_env), "RtlSdr") == 0)
			mRadio = new RtlSdrRadio();
	}
	if (!mRadio) {
		mRadio = new MaximRadio();
		if (!this->works())
			mRadio = new RtlSdrRadio();
	}
	*/
	mRadio = new RtlSdrRadio();
	mDevCount = mRadio->dev_count();
}

QString Radio::status()
{
	QString status;

	if (mDevCount > 0) {
		status += mDevCount + "radio devices found :\n";
		for (int i = 0; i < mDevCount; i++) {
			status += i + ". " + mRadio->dev_name(i) + " : ";
			if (mRadio->dev_init(i))
				status += "OK";
			else
				status += "FAIL";
			status += "\n";
		}
	} else {
		status = "No radio devices found !";
	}

	return status;
}

unsigned int Radio::num()
{
	return mCurDev;
}
void Radio::setNum(unsigned int num)
{
	mCurDev = num;

	emit numChanged();
}

Radio::Mode Radio::mode()
{
	return mCurMode;
}
void Radio::setMode(Radio::Mode mode)
{
	mCurMode = mode;

	mRadio->set_mode(mCurDev, mCurMode);

	emit modeChanged();
}

float Radio::freq()
{
	return mCurFreq;
}
void Radio::setFreq(float freq)
{
	mCurFreq = freq;

	mRadio->set_freq(mCurDev, mCurFreq);

	emit freqChanged();
}

bool Radio::mute()
{
	return mCurMute;
}
void Radio::setMute(bool mute)
{
	mCurMute = mute;

	mRadio->set_mute(mCurDev, mCurMute);

	emit muteChanged();
}

void Radio::play()
{
	mRadio->play(mCurDev);

	emit playing();
}

void Radio::stop()
{
	mRadio->stop(mCurDev);

	emit stopped();
}

// ----- private -----

bool Radio::works()
{
	for (int i = 0; i < mRadio->dev_count(); i++) {
		if (mRadio->dev_init(i)) {
			mRadio->dev_free(i);
			return true;
		}
	}
	return false;
}

// -----------------

void QmlRadioPlugin::registerTypes(const char *uri)
{
	Q_ASSERT(uri == QLatin1String("radio"));

	int ret = qmlRegisterType<Radio>(uri, 1, 0, "Radio");
}
