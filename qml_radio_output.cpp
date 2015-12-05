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

#include "qml_radio_output.h"

RadioOutputAlsa::RadioOutputAlsa() : RadioOutputImplementation(),
				     dev(NULL),
				     hw_params(NULL)
{
	unsigned int rate = 44100;

	if (snd_pcm_open(&dev, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
		std::cerr << "Could not open primary ALSA device" << std::endl;
		works = false;
		return;
	}

	snd_pcm_hw_params_malloc(&hw_params);
	snd_pcm_hw_params_any(dev, hw_params);

	snd_pcm_hw_params_set_access (dev, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format (dev, hw_params, SND_PCM_FORMAT_S16_LE);
	snd_pcm_hw_params_set_rate_near (dev, hw_params, &rate, 0);
	snd_pcm_hw_params_set_channels (dev, hw_params, 2);

	if (snd_pcm_hw_params (dev, hw_params) < 0) {
		std::cerr << "Could not set hardware parameters" << std::endl;
		works = false;
	}
	snd_pcm_hw_params_free (hw_params);

	snd_pcm_prepare (dev);
}

RadioOutputAlsa::~RadioOutputAlsa()
{
	snd_pcm_close (dev);
}

bool RadioOutputAlsa::play(void *buf, int len)
{
	int16_t *cbuf = (int16_t *)buf;
	int frames = (len * sizeof(int16_t)) / 4;
	int res;

	if ((res = snd_pcm_writei(dev, cbuf, frames)) != frames)
		snd_pcm_recover(dev, res, 0);
	snd_pcm_drain(dev);
	snd_pcm_prepare(dev);

	return true;
}


RadioOutputPulse::RadioOutputPulse() : RadioOutputImplementation(),
				       pa(NULL),
				       pa_spec(NULL)
{
	int error;

	pa_spec = (pa_sample_spec*) malloc(sizeof(pa_sample_spec));
	pa_spec->format = PA_SAMPLE_S16LE;
	pa_spec->rate = 44100;
	pa_spec->channels = 2;

	if (!(pa = pa_simple_new(NULL, "qml-radio-plugin", PA_STREAM_PLAYBACK, NULL,
	                         "radio-output", pa_spec, NULL, NULL, &error))) {
		std::cerr << "Error connecting to PulseAudio : " << pa_strerror(error) << std::endl;
		works = false;
	}

	free(pa_spec);	
}

RadioOutputPulse::~RadioOutputPulse()
{
	pa_simple_free(pa);
}

bool RadioOutputPulse::play(void *buf, int len)
{
	int error;

	if (pa_simple_write(pa, buf, (size_t) len, &error) < 0)
		std::cerr << "Error writing to PulseAudio : " << pa_strerror(error) << std::endl;
	pa_simple_drain(pa, &error);

	return true;
}
