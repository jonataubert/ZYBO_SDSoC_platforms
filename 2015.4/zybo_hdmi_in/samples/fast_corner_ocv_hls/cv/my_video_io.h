/************************************************************************/
/*																		*/
/*	my_video_io.h														*/
/*																		*/
/************************************************************************/
/*	Author: Jonathan Aubert												*/
/************************************************************************/
/*  Module Description: 												*/
/*																		*/
/*	Hardware functions that allows to frame buffer <-> HLS Mat (Stream)	*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/* 																		*/
/*		16.12.2016(Jonathan Aubert) : First version						*/
/*																		*/
/************************************************************************/

#ifndef MY_VIDEO_IO_H_
#define MY_VIDEO_IO_H_

#include "img_specs.h"

namespace myhls {


/**
 * @brief Sequentially get pixel (coded on 16 bits) from the array and store
 * it in Mat object with 3 channels and a depth of 8 bits per channel.
 * @param fb the pointer on the array (frame buffer)
 * @param img reference to the resulting Mat object
 */
template <int FB_COLS, typename FB_T, int ROWS, int COLS, int T>
void int16Array2MatBGR(FB_T fb[ROWS*FB_COLS], hls::Mat<ROWS, COLS, T>& img)
{
    const HLS_SIZE_T rows = img.rows;
    const HLS_SIZE_T cols = img.cols;
    const int fBBitWidth = Type_BitWidth<FB_T>::Value;
    const int depth = HLS_TBITDEPTH(T);
    const int ch = HLS_MAT_CN(T);
	u_int32_t i = 0;	//to iterate over the pixels
	hls::Scalar<HLS_MAT_CN(T), HLS_TNAME(T)> pix;

    assert(COLS == FB_COLS && "Cols != stride");//verify width == stride for contiguity of pixels
	assert(fBBitWidth == BITS_PER_PIXEL && "Incorrect Bit-Width for frame buffer");
	assert(ch == OCV_NCHANNEL && "Incorrect number of channels");
	assert(depth == OCV_CHANNEL_DEPTH_BITS && "Incorrect DEPTH");

loop_pixel:
    for (HLS_SIZE_T row = 0; row < rows; row++) {
        for (HLS_SIZE_T col = 0; col < cols; col++) {
#pragma HLS pipeline II=1		//get new data every every clk

            FB_T fBPix = fb[i++];	//Get current pixel on the Frame buffer
            //store it on the channels of the pixel
            pix.val[0] = SHIFT16_B_TO_BYTE(fBPix & MASK16_B);	//B
            pix.val[1] = SHIFT16_G_TO_BYTE(fBPix & MASK16_G);	//G
            pix.val[2] = SHIFT16_R_TO_BYTE(fBPix & MASK16_R);	//R

            img << pix;		//stream pixel in Mat
        }
    }
}

/**
 * @brief Sequentially get pixel from Mat object and store it in the array
 * (coded on 16 bits).
 * @param img reference to "input" Mat object
 * @param fb the pointer on the array (frame buffer)
 */
template <int FB_COLS, typename FB_T, int ROWS, int COLS, int T>
void matBGR2Int16Array(hls::Mat<ROWS, COLS, T>& img, FB_T fb[ROWS*FB_COLS])
{
    const HLS_SIZE_T rows = img.rows;
    const HLS_SIZE_T cols = img.cols;
    const int fBBitWidth = Type_BitWidth<FB_T>::Value;
    const int depth = HLS_TBITDEPTH(T);
    const int ch = HLS_MAT_CN(T);
	u_int32_t i = 0;	//to iterate over the pixels
	hls::Scalar<HLS_MAT_CN(T), HLS_TNAME(T)> pix;

    assert(COLS == FB_COLS && "Cols != stride");	//sequential access only
	assert(fBBitWidth == 16 && "Bit-Width of frame buffer must be 16");
	assert(ch == 3 && "Must be 3 channels");
	assert(depth == 8 && "Depth must be 8");

loop_pixel:
    for (HLS_SIZE_T row = 0; row < rows; row++) {
        for (HLS_SIZE_T col = 0; col < cols; col++) {
#pragma HLS pipeline II=1	//fetch new data every clk cycle

            img >> pix;		//extract pixel from stream

            //Store it in 16 bits RBG
            FB_T fBPix;
            fBPix = (SHIFT_BYTE_TO_16_B((FB_T)pix.val[0])&MASK16_B)
            		+ (SHIFT_BYTE_TO_16_G((FB_T)pix.val[1])&MASK16_G)
            		+ (SHIFT_BYTE_TO_16_R((FB_T)pix.val[2])&MASK16_R);

            //Copy to frame buffer
            fb[i++] = fBPix;
        }
    }
}

} // namespace myhls

#endif /* MY_VIDEO_IO_H_ */
