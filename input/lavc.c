/*****************************************************************************
 * lavc.c: libavcodec common functions
 *****************************************************************************
 * Copyright (C) 2010 Open Broadcast Systems Ltd.
 *
 * Authors: Kieran Kunhya <kieran@kunhya.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *
 *****************************************************************************/

#include "common/common.h"
#include <libavcodec/avcodec.h>

int obe_get_buffer( AVCodecContext *codec, AVFrame *pic )
{
    int w = codec->width;
    int h = codec->height;
    int stride[4];

    avcodec_align_dimensions2( codec, &w, &h, stride );

    /* Only EDGE_EMU codecs are used
     * Allocate an extra line so that SIMD can modify the entire stride for every active line */
    if( av_image_alloc( pic->data, pic->linesize, w, h + 1, codec->pix_fmt, 16 ) < 0 )
        return -1;

    pic->age    = 256*256*256*64; /* FIXME is there a correct value for this? */
    pic->type   = FF_BUFFER_TYPE_USER;
    pic->reordered_opaque = codec->reordered_opaque;
    pic->pkt_pts = codec->pkt ? codec->pkt->pts : AV_NOPTS_VALUE;

    return 0;
}

void obe_release_buffer( AVCodecContext *codec, AVFrame *pic )
{
     /* FIXME: need to release frame when both decoder and encoder are done */
     //av_freep( &pic->data[0] );
     memset( pic->data, 0, sizeof(pic->data) );
}

/* FFmpeg shouldn't call this */
int obe_reget_buffer( AVCodecContext *codec, AVFrame *pic )
{
    if( pic->data[0] == NULL )
    {
        pic->buffer_hints |= FF_BUFFER_HINTS_READABLE;
        return codec->get_buffer( codec, pic );
    }

    pic->reordered_opaque = codec->reordered_opaque;
    pic->pkt_pts = codec->pkt ? codec->pkt->pts : AV_NOPTS_VALUE;

    return 0;
}


