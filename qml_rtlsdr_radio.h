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

#ifndef QML_RTLSDR_RADIO_H
#define QML_RTLSDR_RADIO_H

#include <math.h>
#include <pthread.h>
#include <rtl-sdr.h>

#include "qml_radio.h"
#include "qml_radio_output.h"

#define pthread_signal(n, m) pthread_mutex_lock(m); pthread_cond_signal(n); pthread_mutex_unlock(m)
#define pthread_wait(n, m) pthread_mutex_lock(m); pthread_cond_wait(n, m); pthread_mutex_unlock(m)
#define BUF_LEN 16*16384

typedef struct dongle_ctx dongle_ctx;
typedef struct demod_ctx demod_ctx;
typedef struct output_ctx output_ctx;

struct dongle_ctx {
	pthread_t thr;
	bool thr_finished;
	uint16_t buf[BUF_LEN];
	uint32_t buf_len;
	rtlsdr_dev_t* dev; // from...
	demod_ctx *demod;  // to...
};

struct demod_ctx {
	pthread_t thr;
	bool thr_finished;
	pthread_rwlock_t lck;
	pthread_cond_t ok;
	pthread_mutex_t ok_m;
	int pre_r, pre_j, now_r, now_j, index;
	int pre_index, now_index;
	int16_t buf[BUF_LEN];
	int buf_len;
	int16_t res[BUF_LEN];
	int res_len;
	output_ctx *output; // to...
};

struct output_ctx {
	pthread_t thr;
	bool thr_finished;
	pthread_rwlock_t lck;
	pthread_cond_t ok;
	pthread_mutex_t ok_m;
	int16_t buf[BUF_LEN];
	int buf_len;
};

struct dev_ctx {
	rtlsdr_dev_t* dev;
	Mode mode;
	float freq;
	bool mute;
	bool should_run;
	bool running;
	 // thread contexts
	dongle_ctx *dongle;
	demod_ctx *demod;
	output_ctx *output;
};


class RtlSdrRadio : public RadioImplementation
{
public:
	RtlSdrRadio();
	~RtlSdrRadio() override;

private:
	unsigned int dev_count(void)               override;
	     QString dev_name(unsigned int)        override;
	        bool dev_init(unsigned int)        override;
	        bool dev_free(unsigned int)        override;
		void set_mode(unsigned int, Mode)  override;
		void set_freq(unsigned int, float) override;
	        void set_mute(unsigned int, bool)  override;
	        void     play(unsigned int)        override;
	        void     stop(unsigned int)        override;

	void apply_params(unsigned int);
	void start_threads(unsigned int);
	void stop_threads(unsigned int);
	static void* dongle_thread_fn(void*);
	static void* demod_thread_fn(void*);
	static void* output_thread_fn(void*);

	unsigned int init_dev_count;
	static struct dev_ctx **dev_ctx;
	static RadioOutputImplementation *mRadioOutput;
};

#endif  // QML_RTLSDR_RADIO_H
