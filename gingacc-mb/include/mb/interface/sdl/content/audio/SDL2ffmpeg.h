/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************
Part of this file is extracted from ffplay
ffplay is part of FFmpeg library.
FFmpeg is free software licensed under the LGPL or GPL
Many thanks to ffmpeg developers and to the community that support them!
*******************************************************************************/

#ifndef SDL2FFMPEG_H
#define SDL2FFMPEG_H

/* SDL2ffmpeg cplusplus compat begin */
extern "C" {

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#endif //INT64_C

#ifndef UINT64_C
#define UINT64_C(c) (c ## ULL)
#endif //UINT64_C

#include "libavcodec/avfft.h"
#include "libavformat/avformat.h"
#include "libavutil/avstring.h"
#include "libavutil/opt.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"

#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
}

#ifndef INT64_MIN
#define INT64_MIN		(-__INT64_C(9223372036854775807)-1)
#endif

#ifndef INT64_MAX
#define INT64_MAX		(__INT64_C(9223372036854775807))
#endif

/* SDL2ffmpeg cplusplus compat end*/


#include <SDL.h>
#include <SDL_thread.h>

#define SCALEBITS 10
#define ONE_HALF  (1 << (SCALEBITS - 1))
#define FIX(x)    ((int) ((x) * (1<<SCALEBITS) + 0.5))

#define RGB_TO_Y_CCIR(r, g, b) \
((FIX(0.29900*219.0/255.0) * (r) + FIX(0.58700*219.0/255.0) * (g) + \
  FIX(0.11400*219.0/255.0) * (b) + (ONE_HALF + (16 << SCALEBITS))) >> SCALEBITS)

#define RGB_TO_U_CCIR(r1, g1, b1, shift)\
(((- FIX(0.16874*224.0/255.0) * r1 - FIX(0.33126*224.0/255.0) * g1 + \
		FIX(0.50000*224.0/255.0) * b1 + (ONE_HALF << shift) - 1) >> (SCALEBITS \
    		 + shift)) + 128)

#define RGB_TO_V_CCIR(r1, g1, b1, shift)\
(((FIX(0.50000*224.0/255.0) * r1 - FIX(0.41869*224.0/255.0) * g1 - \
   FIX(0.08131*224.0/255.0) * b1 + (ONE_HALF << shift) - 1) >> (SCALEBITS + \
		   shift)) + 128)

#define MAX_QUEUE_SIZE (15 * 1024 * 1024)
#define MIN_AUDIOQ_SIZE (20 * 16 * 1024)
#define MIN_FRAMES 5

/* SDL audio buffer size, in samples. Should be small to have precise
   A/V sync as SDL does not have hardware buffer fullness info. */
#define SDL_AUDIO_BUFFER_SIZE 1024

/* no AV sync correction is done if below the AV sync threshold */
#define AV_SYNC_THRESHOLD 0.01
/* no AV correction is done if too big error */
#define AV_NOSYNC_THRESHOLD 10.0

/* maximum audio speed change to get correct sync */
#define SAMPLE_CORRECTION_PERCENT_MAX 10

/* we use about AUDIO_DIFF_AVG_NB A-V differences to make the average */
#define AUDIO_DIFF_AVG_NB   20

/* the size must be big enough to compensate the hardware audio buffer size */
#define SAMPLE_ARRAY_SIZE (2*65536)

#define VIDEO_PICTURE_QUEUE_SIZE 2
#define SUBPICTURE_QUEUE_SIZE 4

#define ALPHA_BLEND(a, oldp, newp, s)\
		((((oldp << s) * (255 - (a))) + (newp * (a))) / (255 << s))

#define RGBA_IN(r, g, b, a, s) {\
	unsigned int v = ((const uint32_t *)(s))[0];\
	a = (v >> 24) & 0xff;\
	r = (v >> 16) & 0xff;\
	g = (v >> 8) & 0xff;\
	b = v & 0xff;\
}

#define YUVA_IN(y, u, v, a, s, pal) {\
	unsigned int val = ((const uint32_t *)(pal))[*(const uint8_t*)(s)];\
	a = (val >> 24) & 0xff;\
	y = (val >> 16) & 0xff;\
	u = (val >> 8) & 0xff;\
	v = val & 0xff;\
}

#define YUVA_OUT(d, y, u, v, a) {\
	((uint32_t *)(d))[0] = (a << 24) | (y << 16) | (u << 8) | v;\
}

#define BPP 1

#include <string>
#include <iostream>
#include <set>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	typedef struct PacketQueue {
		AVPacketList *first_pkt, *last_pkt;
		int nb_packets;
		int size;
		int abort_request;
	} PacketQueue;

	typedef struct VideoPicture {
		double pts;                //presentation time stamp for this picture
		double duration;           //expected duration of the frame
		int64_t pos;               //byte position in file
		int skip;
		SDL_Texture* tex;
		int width, height;         // source height & width
		int allocated;
		int reallocate;
		enum PixelFormat pix_fmt;

		AVFrame *src_frame;
	} VideoPicture;

	typedef struct SubPicture {
		double pts; /* presentation time stamp for this picture */
		AVSubtitle sub;
	} SubPicture;

	enum {
		AV_SYNC_AUDIO_MASTER, /* default choice */
		AV_SYNC_VIDEO_MASTER,
		AV_SYNC_EXTERNAL_CLOCK /* synchronize to an external clock */
	};

	typedef struct VideoState {
		int no_background;
		int abort_request;
		int paused;
		int last_paused;
		int seek_req;
		int seek_flags;
		int64_t seek_pos;
		int64_t seek_rel;
		int read_pause_return;
		AVFormatContext *ic;

		int audio_stream;

		int av_sync_type;
		double external_clock; /* external clock base */
		int64_t external_clock_time;

		double audio_clock;
		double audio_diff_cum; /* used for AV difference average computation */
		double audio_diff_avg_coef;
		double audio_diff_threshold;
		int audio_diff_avg_count;
		AVStream *audio_st;
		PacketQueue audioq;
		int audio_hw_buf_size;
		DECLARE_ALIGNED(16,uint8_t,audio_buf2)[AVCODEC_MAX_AUDIO_FRAME_SIZE * 4];
		uint8_t silence_buf[SDL_AUDIO_BUFFER_SIZE];
		uint8_t *audio_buf;
		uint8_t *audio_buf1;

		uint8_t *audio_main_buf[2];
		unsigned int audio_main_buf_size[2];

		int audio_write_buf_size;
		AVPacket audio_pkt_temp;
		AVPacket audio_pkt;
		enum AVSampleFormat audio_src_fmt;
		enum AVSampleFormat audio_tgt_fmt;
		int audio_src_channels;
		int audio_tgt_channels;
		int64_t audio_src_channel_layout;
		int64_t audio_tgt_channel_layout;
		int audio_src_freq;
		int audio_tgt_freq;
		struct SwrContext *swr_ctx;
		double audio_current_pts;
		double audio_current_pts_drift;
		int frame_drops_early;
		int frame_drops_late;
		AVFrame *frame;

		int16_t sample_array[SAMPLE_ARRAY_SIZE];
		int sample_array_index;
		int last_i_start;
		RDFTContext *rdft;
		int rdft_bits;
		FFTSample *rdft_data;
		int xpos;

		SDL_Thread *subtitle_tid;
		int subtitle_stream;
		int subtitle_stream_changed;
		AVStream *subtitle_st;
		PacketQueue subtitleq;
		SubPicture subpq[SUBPICTURE_QUEUE_SIZE];
		int subpq_size, subpq_rindex, subpq_windex;

		double frame_timer;
		double frame_last_pts;
		double frame_last_duration;
		double frame_last_dropped_pts;
		double frame_last_returned_time;
		double frame_last_filter_delay;
		int64_t frame_last_dropped_pos;

		//pts of last decoded frame / predicted pts of next decoded frame
		double video_clock;
		int video_stream;
		AVStream *video_st;
		PacketQueue videoq;

		/*
		 * current displayed pts
		 * (different from video_clock if frame fifos are used)
		 */
		double video_current_pts;

		/*
		 * video_current_pts - time (av_gettime) at which we updated
		 * video_current_pts - used to have running video pts
		 */
		double video_current_pts_drift;

		//current displayed file pos
		int64_t video_current_pos;

		VideoPicture pictq[VIDEO_PICTURE_QUEUE_SIZE];
		int pictq_size, pictq_rindex, pictq_windex;

		struct SwsContext *img_convert_ctx;

		char filename[1024];
		int step;

		int refresh;
	} VideoState;

  class SDL2ffmpeg {
	private:
		static const short ST_PLAYING = 0;
		static const short ST_PAUSED  = 1;
		static const short ST_STOPPED = 2;

		int wanted_stream[AVMEDIA_TYPE_NB];
		int seek_by_bytes;
		int av_sync_type;
		int64_t start_time;
		int64_t duration;
		int workaround_ffmpeg_bugs;
		int fast;
		int genpts;
		int lowres;
		int idct;
		enum AVDiscard skip_frame;
		enum AVDiscard skip_idct;
		enum AVDiscard skip_loop_filter;
		int error_concealment;
		int decoder_reorder_pts;
		int framedrop;
		int rdftspeed;

		AVPacket flush_pkt;
		VideoState* is;

		static bool init;
		static pthread_mutex_t iMutex;
		static set<SDL2ffmpeg*> instances;
		static SDL_AudioSpec wantedSpec;

		AVFrame vFrame;
		AVPacket vPkt;

		int audioFreq;
		Uint8 audioChannels;

		short state;
		SDL_AudioSpec spec;
		float soundLevel;
		SDL_Texture* texture;
		bool hasPic;

	public:
		SDL2ffmpeg(const char *filename);
		~SDL2ffmpeg();

	private:
		void release();
		bool prepare();

	public:
		void play();
		void stop();
		void pause();
		void resume();

		void getOriginalResolution(int* width, int* height);

		double getDuration();
		double getPosition();
		void seek(int64_t pos);

		void setTexture(SDL_Texture* texture);
		bool hasTexture();
		SDL_Texture* getTexture();

		bool hasPicture();
        void setSoundLevel(float level);

	private:
		int packet_queue_put(PacketQueue *q, AVPacket *pkt);
		void packet_queue_init(PacketQueue *q);
		void packet_queue_flush(PacketQueue *q);
		void packet_queue_end(PacketQueue *q);
		void packet_queue_abort(PacketQueue *q);
		int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block);

		void alloc_picture();
		void free_subpicture(SubPicture *sp);
		void video_image_display();
		void stream_close();

		void video_display();

		double get_audio_clock();
		double get_video_clock();
		double get_external_clock();
		double get_master_clock();

		void stream_seek(int64_t pos, int64_t rel, int seek_by_bytes);
		void stream_toggle_pause();
		double compute_target_delay(double delay);
		void pictq_next_picture();
		void update_video_pts(double pts, int64_t pos);

		int queue_picture(AVFrame *src_frame, double pts1, int64_t pos);
		int get_video_frame(AVFrame *frame, int64_t *pts, AVPacket* pkt);

		int synchronize_audio(short *samples, int samples_size1, double pts);
		int audio_decode_frame(double *pts_ptr);
		int stream_component_open(int stream_index);
		void stream_component_close(int stream_index);

		void stream_cycle_channel(int codec_type);
		void toggle_pause();
		void step_to_next_frame();

	public:
		void refresh();

	private:
		int audio_refresh_decoder();
		int video_refresh_decoder();
		void video_refresh_content();
		int read_content();

		bool getAudioSpec(
				SDL_AudioSpec* spec, int sample_rate, uint8_t channels);

		static void clamp(short* buf, int len);
		static void sdl_audio_callback(void *opaque, Uint8 *stream, int len);

		static int subtitle_thread(void *arg);

		static struct SwsContext* createContext(
				int inWidth,
				int inHeight,
				enum PixelFormat inFormat,
				int outWidth,
				int outHeight,
				enum PixelFormat outFormat);
  };
}
}
}
}
}
}

#endif /* SDL2FFMPEG_H */