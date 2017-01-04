/************************************************************************/
/*																		*/
/*	MoveAvg.h															*/
/*																		*/
/************************************************************************/
/*	Author: Jonathan Aubert												*/
/************************************************************************/
/*  Module Description: 												*/
/*																		*/
/*	Simple class that stores samples and computes their average.		*/
/*	(sliding Window)													*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/* 																		*/
/*		16.12.2016(Jonathan Aubert) : First version						*/
/*																		*/
/************************************************************************/

#ifndef MOVEAVG_H
#define MOVEAVG_H

#include <queue>
#include <sys/types.h>

class MoveAvg
{
	public:

		/**
		 * @brief Default constructor, window has max 2 samples
		 */
		MoveAvg();

		/**
		 * @brief Constructor which needs the maximum length of the moving window
		 * @param length the number of samples
		 */
		MoveAvg(u_int32_t length);

		/**
		 * @brief Add a sample to the moving window
		 * @param sample value to add
		 */
		void addSample(double sample);

		/**
		 * @brief Get the value of the last added sample
		 * @return last sample
		 */
		double getLastSample() const;

		/**
		 * @brief Get the current mean value of the window
		 * @return the current mean value
		 */
		double getCurrentMean() const;

		/**
		 * @brief Get the current size of the window
		 * @return the number of samples in the window
		 */
		u_int32_t getSampleNB() const;

	private:
		std::queue<double> qu;	///Contains the samples
		u_int32_t maxLen;		///Maximum number of samples
		double sum;             ///Sum of the samples
};

#endif // MOVEAVG_H
