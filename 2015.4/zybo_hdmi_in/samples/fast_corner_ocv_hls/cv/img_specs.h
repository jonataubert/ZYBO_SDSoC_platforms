/************************************************************************/
/*																		*/
/*	img_specs.h															*/
/*																		*/
/************************************************************************/
/*	Author: Jonathan Aubert												*/
/************************************************************************/
/*  Module Description: 												*/
/*	Specifications of the frame buffers, size, color depth...			*/
/*	Contains macros to easily access a color channel from 				*/
/*  the value of a pixel 												*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/* 																		*/
/*		16.12.2016(Jonathan Aubert) : First version						*/
/*																		*/
/************************************************************************/

#ifndef IMG_SPECS_H_
#define IMG_SPECS_H_

#define RES_720p	//Comment this to use 1080p

//Image specifications
#ifdef RES_720p
#define IM_WIDTH  1280
#define IM_HEIGHT 720
#else
#define IM_WIDTH  1920
#define IM_HEIGHT 1080
#endif

#define IM_PIXELS (IM_WIDTH*IM_HEIGHT)
#define IM_STRIDE IM_WIDTH	//Because we perform a sequential access (DMA SIMPLE)

//Pixel representation in the frame buffer 16 bits RBG
//		    MSB			     LSB
//			|11111|11111|111111|
//			|  R  |  B  |  G   |
#define BITS_PER_PIXEL 16
#define BYTE_PER_PIXEL 2
#define IM_STRIDE_BYTE (IM_STRIDE * BYTE_PER_PIXEL)
#define IM_SIZE_BYTE (IM_WIDTH*IM_HEIGHT*BYTE_PER_PIXEL)


//Pixel representation with the computer vision libraries (SW + HW) is 24 bits BGR (8UC3)
#define OCV_NCHANNEL 3
#define OCV_CHANNEL_DEPTH_BITS 8

//COLOR masks and shift macro
#define MASK16_R 0xF800
#define MASK16_G 0x003F
#define MASK16_B 0x07C0
#define SHIFT16_R_TO_BYTE(r) ((r) >> 8)
#define SHIFT16_G_TO_BYTE(g) ((g) << 2)
#define SHIFT16_B_TO_BYTE(b) ((b) >> 3)
#define SHIFT_BYTE_TO_16_R(r) ((r) << 8)
#define SHIFT_BYTE_TO_16_G(g) ((g) >> 2)
#define SHIFT_BYTE_TO_16_B(b) ((b) << 3)

//Number of framebuffers
#define DISPLAY_NUM_FRAMES 3



#endif /* IMG_SPECS_H_ */
