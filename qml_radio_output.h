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

#ifndef QML_RADIO_OUTPUT_H
#define QML_RADIO_OUTPUT_H

#include <alsa/asoundlib.h>

#include <pulse/simple.h>
#include <pulse/error.h>

#include "qml_radio.h"

 /* RadioOutputImplementation is a virtual class, with 2 implementations :
  * - RadioOutputAlsa ;
  * - RadioOutputPulse  (**TODO !**) ;
  */
class RadioOutputImplementation
{
public:
	RadioOutputImplementation() {};
	virtual ~RadioOutputImplementation() {};

	virtual bool play(void*, int) = 0;

	bool works;
};


class RadioOutputAlsa : public RadioOutputImplementation
{
public:
	RadioOutputAlsa();
	~RadioOutputAlsa() override;

private:
	bool play(void *, int) override;

	snd_pcm_t *dev;
	snd_pcm_hw_params_t *hw_params;
};

class RadioOutputPulse : public RadioOutputImplementation
{
public:
	RadioOutputPulse();
	~RadioOutputPulse() override;

private:
	bool play(void *, int) override;

	pa_simple *pa;
	pa_sample_spec *pa_spec;
};

#endif  // QML_RTLSDR_RADIO_H
