/************************************************************************/
/*																		*/
/*	my_ocv_hw.cpp														*/
/*																		*/
/************************************************************************/
/*	Author: Jonathan Aubert												*/
/************************************************************************/
/*  Module Description: 												*/
/*	Computer vision examples that uses the HLS video library			*/
/*																		*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/* 																		*/
/*		16.12.2016(Jonathan Aubert) : First version						*/
/*																		*/
/************************************************************************/

#ifndef _MY_OCV_H_
#define _MY_OCV_H_

//Don't use FPO LIB (hls_fpo.h) that causes link error.
#define HLS_NO_XIL_FPO_LIB

#include <hls_video.h>
#include "img_specs.h"
#include "my_video_io.h"

// typedef video library core structures
typedef hls::Scalar<1, u_int8_t>                 		PIXEL_C1;
typedef hls::Mat<IM_HEIGHT, IM_WIDTH, HLS_8UC1>     	IMAGE_C1;
typedef hls::Scalar<3, u_int8_t>                 		PIXEL_C3;
typedef hls::Mat<IM_HEIGHT, IM_WIDTH, HLS_8UC3>     	IMAGE_C3;




#pragma SDS data access_pattern(video_in:SEQUENTIAL, video_out:SEQUENTIAL)
#pragma SDS data copy(video_in[0:IM_PIXELS], video_out[0:IM_PIXELS])
//EXPLICIT CONTIGUOUS and disable automatic cache flush/invalidate
#pragma SDS data mem_attribute(video_in:PHYSICAL_CONTIGUOUS | NON_CACHEABLE, video_out:PHYSICAL_CONTIGUOUS | NON_CACHEABLE)
void cvHwPassthrough(u_int16_t video_in[IM_PIXELS], u_int16_t video_out[IM_PIXELS]);

#pragma SDS data access_pattern(video_in:SEQUENTIAL, video_out:SEQUENTIAL)
#pragma SDS data copy(video_in[0:IM_PIXELS], video_out[0:IM_PIXELS])
//EXPLICIT CONTIGUOUS and disable automatic cache flush/invalidate
#pragma SDS data mem_attribute(video_in:PHYSICAL_CONTIGUOUS | NON_CACHEABLE, video_out:PHYSICAL_CONTIGUOUS | NON_CACHEABLE)
void cvHwFastCorner(u_int16_t video_in[IM_PIXELS], u_int16_t video_out[IM_PIXELS]);

#pragma SDS data access_pattern(video_in:SEQUENTIAL, video_out:SEQUENTIAL)
#pragma SDS data copy(video_in[0:IM_PIXELS], video_out[0:IM_PIXELS])
//EXPLICIT CONTIGUOUS and disable automatic cache flush/invalidate
#pragma SDS data mem_attribute(video_in:PHYSICAL_CONTIGUOUS | NON_CACHEABLE, video_out:PHYSICAL_CONTIGUOUS | NON_CACHEABLE)
void cvHwColorGrey(u_int16_t video_in[IM_PIXELS], u_int16_t video_out[IM_PIXELS]);

#endif

