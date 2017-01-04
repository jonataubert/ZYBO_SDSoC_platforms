/************************************************************************/
/*																		*/
/*	my_ocv_sw.cpp														*/
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

#include "my_ocv_sw.h"
#include "img_specs.h"


void ocvSwPassthrough(cv::Mat *src, cv::Mat *dst)
{
	src->copyTo(*dst);	//deep copy
}

void ocvSwColorGrey(cv::Mat *src, cv::Mat *dst)
{
	cv::Mat tmpgrey;
	cv::cvtColor(*src, tmpgrey,CV_BGR2GRAY);
	cv::cvtColor(tmpgrey, *dst,CV_GRAY2BGR);
}

u_int32_t ocvSwFastCorner(cv::Mat *src, cv::Mat *dst)
{
	//deep copy src image on the dst
	src->copyTo(*dst);
	std::vector<cv::Mat> layers;
	std::vector<cv::KeyPoint> keypoints;
	//convert the src to YUV
	cv::cvtColor(*src, *src, CV_BGR2YCrCb);
	//divides a multi-channel array into several single-channel arrays
	cv::split(*src, layers);
	//FAST corner alg on greyscale src (Y)
	cv::FAST(layers[0], keypoints, 20, true);

	//draw rectangles on destination image
	for (unsigned int i = 0; i < keypoints.size(); i++)
	{
		cv::rectangle(*dst,
				  cv::Point(keypoints[i].pt.x-1, keypoints[i].pt.y-1),
		  cv::Point(keypoints[i].pt.x+1, keypoints[i].pt.y+1),
				  cv::Scalar(0,255,0),
				  CV_FILLED);	//green filled rectangle on keypoints
	}

	return keypoints.size();
}

int32_t ocvFbToMat(u_int16_t *frameBufferIn, int32_t fBHeight, int32_t fBWidth,u_int32_t stridePxl, cv::Mat *dst)
{
	//Mat objet needs to be <= Fb size
	if((dst->cols > fBWidth) || (dst->rows > fBWidth) || dst->channels() != OCV_NCHANNEL)
	{
		return -1;
	}
	u_int32_t cn = OCV_NCHANNEL;
	for(int i = 0; i < dst->rows ; i++)
	{
		for(int j = 0; j < dst->cols ; j++)
		{
			dst->data[i*dst->cols*cn + j*cn + 0] =
					SHIFT16_B_TO_BYTE((frameBufferIn[i*stridePxl+j] & MASK16_B));
			dst->data[i*dst->cols*cn + j*cn + 1] =
					SHIFT16_G_TO_BYTE((frameBufferIn[i*stridePxl+j] & MASK16_G));
			dst->data[i*dst->cols*cn + j*cn + 2] =
					SHIFT16_R_TO_BYTE((frameBufferIn[i*stridePxl+j] & MASK16_R));
		}
	}
	return 0;
}

int32_t ocvMatToFb(u_int16_t *frameBufferOut, int32_t fBHeight, int32_t fBWidth, u_int32_t stridePxl, cv::Mat *src)
{
	//Mat objet has to be <= Fb size
	if((src->cols > fBWidth) || (src->rows > fBWidth) || src->channels() != OCV_NCHANNEL)
	{
		return -1;
	}
	u_int32_t cn = OCV_NCHANNEL;
	for(int i = 0; i < src->rows ; i++)
	{
		for(int j = 0; j < src->cols ; j++)
		{
			frameBufferOut[i*stridePxl+j] =
				(SHIFT_BYTE_TO_16_B((u_int16_t)src->data[i*src->cols*cn + j*cn + 0])&MASK16_B)
				+ (SHIFT_BYTE_TO_16_G((u_int16_t)src->data[i*src->cols*cn + j*cn + 1])&MASK16_G)
				+ (SHIFT_BYTE_TO_16_R((u_int16_t)src->data[i*src->cols*cn + j*cn + 2])&MASK16_R);
		}
	}
	return 0;
}

