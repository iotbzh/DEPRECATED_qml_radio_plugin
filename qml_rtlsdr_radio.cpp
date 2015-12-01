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

#include "qml_rtlsdr_radio.h"

struct dev_ctx** RtlSdrRadio::dev_ctx;
RadioOutputImplementation * RtlSdrRadio::mRadioOutput;

RtlSdrRadio::RtlSdrRadio() : RadioImplementation(),
			     init_dev_count(0)
{
	init_dev_count = dev_count();

	dev_ctx = (struct dev_ctx**) malloc(init_dev_count * sizeof(struct dev_ctx));

	for (int i = 0; i < init_dev_count; i++) {
		dev_ctx[i] = (struct dev_ctx*) malloc(sizeof(struct dev_ctx));
		dev_ctx[i]->dev = NULL;
		dev_ctx[i]->dongle = NULL;
		dev_ctx[i]->demod = NULL;
		dev_ctx[i]->output = NULL;
		dev_ctx[i]->mode = FM;
		dev_ctx[i]->freq = 100.0;
		dev_ctx[i]->mute = false;
		dev_init(i);
	}

	/*
	if (strcasecmp(getenv(QML_RADIO_OUTPUT), "Alsa") == 0)
		mRadioOutput = new RadioOutputAlsa();
	else
		mRadioOutput = new RadioOutputPulse();
	*/
	mRadioOutput = new RadioOutputAlsa();
}

RtlSdrRadio::~RtlSdrRadio()
{
	for (int i = 0; i < init_dev_count; i++) {
		dev_free(i);
		free(dev_ctx[i]);
	}
	free(dev_ctx);
}

unsigned int RtlSdrRadio::dev_count()
{
	return rtlsdr_get_device_count();
}

QString RtlSdrRadio::dev_name(unsigned int id)
{
	return rtlsdr_get_device_name(id);
}

bool RtlSdrRadio::dev_init(unsigned int id)
{
	rtlsdr_dev_t *dev = dev_ctx[id]->dev;

	if (rtlsdr_open(&dev, id) < 0)
		return false;

	rtlsdr_set_tuner_gain_mode(dev, 0);

	if (rtlsdr_reset_buffer(dev) < 0)
		return false;

	dev_ctx[id]->dev = dev;

	apply_params(id);

	return true;
}

bool RtlSdrRadio::dev_free(unsigned int id)
{
	rtlsdr_dev_t* dev = dev_ctx[id]->dev;

	if (rtlsdr_close(dev) < 0)
		return false;
	dev = NULL;

	return true;
}

void RtlSdrRadio::set_mode(unsigned int id, Mode mode)
{
	if (init_dev_count < id + 1)
		return;

	dev_ctx[id]->mode = mode;

	apply_params(id);
}

void RtlSdrRadio::set_freq(unsigned int id, float freq)
{
	if (init_dev_count < id + 1)
		return;

	dev_ctx[id]->freq = freq;

	apply_params(id);
}

void RtlSdrRadio::set_mute(unsigned int id, bool mute)
{
	if (init_dev_count < id + 1)
		return;

	dev_ctx[id]->mute = mute;

	apply_params(id);
}

void RtlSdrRadio::play(unsigned int id)
{
	start_threads(id);
}

void RtlSdrRadio::stop(unsigned int id)
{
	stop_threads(id);
}

// ----- local -----

void RtlSdrRadio::apply_params(unsigned int id)
{
	rtlsdr_dev_t* dev = dev_ctx[id]->dev;
	Mode mode = dev_ctx[id]->mode;
	float freq = dev_ctx[id]->freq;
	int rate;

	freq *= 1000000;
	rate = ((1000000 / 200000) + 1) * 200000;

	if (mode == FM)
		freq += 16000;
	freq += rate / 4;

	rtlsdr_set_center_freq(dev, freq);
	rtlsdr_set_sample_rate(dev, rate);

	dev_ctx[id]->dev = dev;
}

void RtlSdrRadio::start_threads(unsigned int id)
{
	if (init_dev_count < id + 1)
		return;

	dongle_ctx dongle;
	demod_ctx demod;
	output_ctx output;

	rtlsdr_dev_t *dev = dev_ctx[id]->dev;
	dev_ctx[id]->dongle = &dongle;
	dev_ctx[id]->demod = &demod;
	dev_ctx[id]->output = &output;
	dev_ctx[id]->should_run = true;

	pthread_rwlock_init(&demod.lck, NULL);
	pthread_cond_init(&demod.ok, NULL);
	pthread_mutex_init(&demod.ok_m, NULL);
	pthread_rwlock_init(&output.lck, NULL);
	pthread_cond_init(&output.ok, NULL);
	pthread_mutex_init(&output.ok_m, NULL);

	 // dongle thread
	dongle.dev = dev;	// from...
	dongle.demod = &demod;	// to...
	dongle.thr_finished = false;
	pthread_create(&dongle.thr, NULL, dongle_thread_fn, (void*)dev_ctx[id]);

	 // demod thread
	demod.output = &output; // to...
	demod.pre_r = demod.pre_j = 0;
	demod.now_r = demod.now_j = 0;
	demod.index = 0;
	demod.pre_index = demod.now_index = 0;
	dongle.thr_finished = false;
	pthread_create(&demod.thr, NULL, demod_thread_fn, (void*)dev_ctx[id]);

	 // output thread
	dongle.thr_finished = false;
	pthread_create(&output.thr, NULL, output_thread_fn, (void*)dev_ctx[id]);
}

void RtlSdrRadio::stop_threads(unsigned int id)
{
	if (init_dev_count < id + 1)
		return;

	rtlsdr_dev_t* dev = dev_ctx[id]->dev;
	dongle_ctx *dongle = dev_ctx[id]->dongle;
	demod_ctx *demod = dev_ctx[id]->demod;
	output_ctx *output = dev_ctx[id]->output;

	if (!dongle || !demod || !output)
		return;

	 // stop each "while" loop in the threads
	dev_ctx[id]->should_run = false;

	rtlsdr_cancel_async(dev);
	pthread_signal(&demod->ok, &demod->ok_m);
	pthread_signal(&output->ok, &output->ok_m);

	while (!dongle->thr_finished ||
	       !demod->thr_finished ||
	       !output->thr_finished)
		usleep(100000);		

	pthread_join(dongle->thr, NULL);
	pthread_join(demod->thr, NULL);
	pthread_join(output->thr, NULL);
	pthread_rwlock_destroy(&demod->lck);
	pthread_cond_destroy(&demod->ok);
	pthread_mutex_destroy(&demod->ok_m);
	pthread_rwlock_destroy(&output->lck);
	pthread_cond_destroy(&output->ok);
	pthread_mutex_destroy(&output->ok_m);
}

static void rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx)
{
	struct dev_ctx *dev_ctx_cur = (struct dev_ctx *)ctx;
	dongle_ctx *dongle = dev_ctx_cur->dongle;
	demod_ctx *demod = dev_ctx_cur->demod;
	unsigned char tmp;

	if (!dev_ctx_cur->should_run)
		return;

	// rotate 90°
	for (int i = 0; i < (int)len; i += 8) {
		tmp = 255 - buf[i+3];
		buf[i+3] = buf[i+2];
		buf[i+2] = tmp;

		buf[i+4] = 255 - buf[i+4];
		buf[i+5] = 255 - buf[i+5];

		tmp = 255 - buf[i+6];
		buf[i+6] = buf[i+7];
		buf[i+7] = tmp;
	}

	// write data
	for (int i = 0; i < (int)len; i++)
		dongle->buf[i] = (int16_t)buf[i] - 127;

	// lock demod thread, write to it, unlock
	   pthread_rwlock_wrlock(&demod->lck);
	memcpy(demod->buf, dongle->buf, 2 * len);
	demod->buf_len = len;
	   pthread_rwlock_unlock(&demod->lck);
	   pthread_signal(&demod->ok, &demod->ok_m);
}
void* RtlSdrRadio::dongle_thread_fn(void *ctx)
{
	struct dev_ctx *dev_ctx_cur = (struct dev_ctx *)ctx;
	dongle_ctx *dongle = dev_ctx_cur->dongle;

	rtlsdr_read_async(dongle->dev, rtlsdr_callback, dev_ctx_cur, 0, 0);

	dongle->thr_finished = true;
	return 0;
}

void lowpass_demod(void *ctx)
{
	demod_ctx *demod = (demod_ctx *)ctx;
	int i=0, i2=0;

	while (i < demod->buf_len) {
		demod->now_r += demod->buf[i];
		demod->now_j += demod->buf[i+1];
		i += 2;
		demod->index++;
		if (demod->index < ((1000000 / 200000) + 1))
			continue;
		demod->buf[i2] = demod->now_r;
		demod->buf[i2+1] = demod->now_j;
		demod->index = 0;
		demod->now_r = demod->now_j = 0;
		i2 += 2;
	}
	demod->buf_len = i2;
}
void lowpassreal_demod(void *ctx)
{
	demod_ctx *demod = (demod_ctx *)ctx;
	int i=0, i2=0;
	int fast = 200000;
	int slow = 48000;

	while (i < demod->res_len) {
		demod->now_index += demod->res[i];
		i++;
		demod->pre_index += slow;
		if (demod->pre_index < fast)
			continue;
		demod->res[i2] = (int16_t)(demod->now_index / (fast/slow));
		demod->pre_index -= fast;
		demod->now_index = 0;
		i2 += 1;
	}
	demod->res_len = i2;
}
void multiply(int ar, int aj, int br, int bj, int *cr, int *cj)
{
	*cr = ar*br - aj*bj;
	*cj = aj*br + ar*bj;
}
int polar_discriminant(int ar, int aj, int br, int bj)
{
	int cr, cj;
	double angle;
	multiply(ar, aj, br, -bj, &cr, &cj);
	angle = atan2((double)cj, (double)cr);
	return (int)(angle / 3.14159 * (1<<14));
}
void fm_demod(void *ctx)
{
	demod_ctx *demod = (demod_ctx *)ctx;
	int16_t *buf = demod->buf;
	int buf_len = demod->buf_len;
	int pcm;

	pcm = polar_discriminant(buf[0], buf[1], demod->pre_r, demod->pre_j);
	demod->res[0] = (int16_t)pcm;

	for (int i = 2; i < (buf_len-1); i += 2) {
		pcm = polar_discriminant(buf[i], buf[i+1], buf[i-2], buf[i-1]);
		demod->res[i/2] = (int16_t)pcm;
	}
	demod->pre_r = buf[buf_len - 2];
	demod->pre_j = buf[buf_len - 1];
	demod->res_len = buf_len/2;
}
void am_demod(void *ctx)
{
	demod_ctx *demod = (demod_ctx *)ctx;
	int16_t *buf = demod->buf;
	int buf_len = demod->buf_len;
	int pcm;

	for (int i = 0; i < buf_len; i += 2) {
		pcm = buf[i] * buf[i];
		pcm += buf[i+1] * buf[i+1];
		demod->res[i/2] = (int16_t)sqrt(pcm);
	}
	demod->res_len = buf_len/2;
}
void* RtlSdrRadio::demod_thread_fn(void *ctx)
{
	struct dev_ctx *dev_ctx_cur = (struct dev_ctx *)ctx;
	demod_ctx *demod = dev_ctx_cur->demod;
	output_ctx *output = dev_ctx_cur->output;

	while (dev_ctx_cur->should_run) {
		   pthread_wait(&demod->ok, &demod->ok_m);
		   pthread_rwlock_wrlock(&demod->lck);
		lowpass_demod(demod);
		if (dev_ctx_cur->mode == FM)
			fm_demod(demod);
		else
			am_demod(demod);
		lowpassreal_demod(demod);
		  pthread_rwlock_unlock(&demod->lck);

		    // lock demod thread, write to it, unlock
		   pthread_rwlock_wrlock(&output->lck);
		memcpy(output->buf, demod->res, 2 * demod->res_len);
		output->buf_len = demod->res_len;
		   pthread_rwlock_unlock(&output->lck);
		   pthread_signal(&output->ok, &output->ok_m);
	}

	demod->thr_finished = true;
	return 0;
}

void* RtlSdrRadio::output_thread_fn(void *ctx)
{
	struct dev_ctx *dev_ctx_cur = (struct dev_ctx *)ctx;
	output_ctx *output = dev_ctx_cur->output;

	while (dev_ctx_cur->should_run) {
		  pthread_wait(&output->ok, &output->ok_m);
		  pthread_rwlock_rdlock(&output->lck);
		if (!dev_ctx_cur->mute)
			mRadioOutput->play((void*)&output->buf, output->buf_len);
		  pthread_rwlock_unlock(&output->lck);
	}

	output->thr_finished = true;
	return 0;
}
