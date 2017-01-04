/************************************************************************/
/*																		*/
/*	my_ocv_sw.h															*/
/*																		*/
/************************************************************************/
/*	Author: Jonathan Aubert												*/
/************************************************************************/
/*  Module Description: 												*/
/*																		*/
/*	Functions used to use OpenCV Mat BGR 8UC3 object along with the 	*/
/*  frame buffer coded that has 16 bits per pixels.						*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/* 																		*/
/*		16.12.2016(Jonathan Aubert) : First version						*/
/*																		*/
/************************************************************************/

#ifndef MY_OCV_SW_H_
#define MY_OCV_SW_H_

#include <opencv2/opencv.hpp>

/**
 * @brief Detects and draws the detected corners from Mat src to Mat dst using
 * FAST corner.
 * @param src pointer of the source image
 * @param dst pointer of the destination image
 * @return the number of detected corners
 */
u_int32_t ocvSwFastCorner(cv::Mat *src, cv::Mat *dst);

/**
 * @brief Copies of src Mat to dst Mat
 * @param src pointer of the source image
 * @param dst pointer of the destination image
 */
void ocvSwPassthrough(cv::Mat *src, cv::Mat *dst);

/**
 * @brief Convert RGB color to RGB greyscale
 * @param src pointer of the source image
 * @param dst pointer of the destination image
 */
void ocvSwColorGrey(cv::Mat *src, cv::Mat *dst);

/**
 * @brief Copy frame buffer to BGR 8UC3 Mat object
 * @param frameBufferIn pointer of the source frame buffer
 * @param fBHeight Height (in pixels) of the frame buffer
 * @param fBWidth Width (in pixels) of the frame buffer
 * @param stridePxl size (in pixel) of the stride (frame buffer)
 * @param dst MAt pointer (destination image)
 * @return 0 on success
 */
int32_t ocvFbToMat(u_int16_t *frameBufferIn, int32_t fBHeight, int32_t fBWidth, u_int32_t stridePxl, cv::Mat *dst);

/**
 * @brief Copy BGR 8UC3 Mat object to frame buffer
 * @param frameBufferOut pointer of the destination frame buffer
 * @param fBHeight Height (in pixels) of the frame buffer
 * @param fBWidth Width (in pixels) of the frame buffer
 * @param stridePxl size (in pixel) of the stride (frame buffer)
 * @param src Mat pointer (source image)
 * @return 0 on success
 */
int32_t ocvMatToFb(u_int16_t *frameBufferOut, int32_t fBHeight, int32_t fBWidth, u_int32_t stridePxl, cv::Mat *src);

#endif /* MY_OCV_SW_H_ */
