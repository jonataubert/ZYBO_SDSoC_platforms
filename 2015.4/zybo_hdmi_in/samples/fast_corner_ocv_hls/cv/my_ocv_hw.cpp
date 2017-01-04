/************************************************************************/
/*																		*/
/*	my_ocv_hw.h															*/
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

#include "my_ocv_hw.h"


void cvHwPassthrough(u_int16_t video_in[IM_PIXELS],u_int16_t video_out[IM_PIXELS])
{
    IMAGE_C3 img_1(IM_HEIGHT, IM_WIDTH);

#pragma HLS DATAFLOW

    myhls::int16Array2MatBGR<IM_STRIDE>(video_in, img_1);
	myhls::matBGR2Int16Array<IM_STRIDE>(img_1, video_out);
}

void cvHwFastCorner(u_int16_t video_in[IM_PIXELS], u_int16_t video_out[IM_PIXELS])
{
	const int rows = IM_HEIGHT;
	const int cols = IM_WIDTH;

	#pragma HLS DATAFLOW	//pipeline functions

    IMAGE_C3 img_1(rows, cols);
    IMAGE_C3 img_1_(rows, cols);
    IMAGE_C3 img_3(rows, cols);
    IMAGE_C3 img_4(rows, cols);
    IMAGE_C3 img_5(rows, cols);
    IMAGE_C3 img_6(rows, cols);
    IMAGE_C1 img_1_Y(rows, cols);
    IMAGE_C1 img_1_U(rows, cols);
    IMAGE_C1 img_1_V(rows, cols);
    //BGR(0,255,0) TO YUV
    //y = 0.587*255 = 149.6
    //u = 128-0.4187*255 = 21.23
    //v = 128-0.3313*255 = 43.52
    PIXEL_C3 YCrCbcolor(149,21,43);
    IMAGE_C1 mask(rows, cols);
    IMAGE_C1 dmask(rows, cols);

    //XAPP1167 p.10 -> reconvergent path (10 lines of latency)
#ifdef RES_720p
	#pragma HLS stream depth=13000 variable=img_1_.data_stream
#else
	#pragma HLS stream depth=20000 variable=img_1_.data_stream
#endif

    //Sequential read RAM -> Mat
    myhls::int16Array2MatBGR<IM_STRIDE>(video_in, img_3);

    hls::CvtColor<HLS_BGR2YCrCb>(img_3, img_4);
    //img_1 and img_1_1 are copies of img_4
	hls::Duplicate(img_4, img_1, img_1_);
    //Split Y  U  V from img_1
	hls::Split(img_1, img_1_Y, img_1_U, img_1_V);
	//U and V are not needed anymore
    hls::Consume(img_1_U);
    hls::Consume(img_1_V);
    hls::FASTX(img_1_Y, mask, 20, true);
    hls::Dilate(mask, dmask);
    //Copy img_1_ to img_6 and apply color (Green)
    //when dmask pxl is != 0
    hls::PaintMask(img_1_, dmask, img_6, YCrCbcolor);
    hls::CvtColor<HLS_YCrCb2BGR>(img_6, img_5);
    //Mat -> sequential write to RAM
    myhls::matBGR2Int16Array<IM_STRIDE>(img_5, video_out);
}

void cvHwColorGrey(u_int16_t video_in[IM_PIXELS], u_int16_t video_out[IM_PIXELS])
{
	const int rows = IM_HEIGHT;
	const int cols = IM_WIDTH;

    IMAGE_C3 img_1(rows, cols);
    IMAGE_C1 img_g(rows, cols);
    IMAGE_C3 img_2(rows, cols);

#pragma HLS DATAFLOW	//pipeline functions
    myhls::int16Array2MatBGR<IM_STRIDE>(video_in, img_1);
    hls::CvtColor<HLS_BGR2GRAY>(img_1, img_g);
    hls::CvtColor<HLS_GRAY2BGR>(img_g, img_2);
	myhls::matBGR2Int16Array<IM_STRIDE>(img_2, video_out);
}
