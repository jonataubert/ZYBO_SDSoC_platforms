/************************************************************************/
/*																		*/
/*	MoveAvg.cpp															*/
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

#include "MoveAvg.h"

MoveAvg::MoveAvg() : maxLen(2), sum(0.0)
{
}

MoveAvg::MoveAvg(u_int32_t length) : maxLen(length), sum(0.0)
{
}

void MoveAvg::addSample(double sample)
{
	qu.push(sample);
	sum += sample;
	if(qu.size() > maxLen)
	{
		sum -= qu.front();
		qu.pop();
	}
}

double MoveAvg::getLastSample() const
{
	if (qu.empty())
		return 0.0;
	else
		return qu.back();

}

double MoveAvg::getCurrentMean() const
{
	if (qu.empty())
		return 0.0;
	else
		return sum / qu.size();
}

u_int32_t MoveAvg::getSampleNB() const
{
	return qu.size();
}
