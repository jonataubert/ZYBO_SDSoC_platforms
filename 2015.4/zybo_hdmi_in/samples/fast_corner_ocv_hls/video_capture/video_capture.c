/******************************************************************************
 * @file video_capture.c
 * Digilent Video Capture Driver
 *
 * @author Sam Bobrowicz
 *
 * @date 2015-Nov-25
 *
 * @copyright
 * (c) 2015 Copyright Digilent Incorporated
 * All Rights Reserved
 *
 * This program is free software; distributed under the terms of BSD 3-clause
 * license ("Revised BSD License", "New BSD License", or "Modified BSD License")
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name(s) of the above-listed copyright holder(s) nor the names
 *    of its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 * @desciption
 * This module provides an easy to use API for streaming video into memory
 * on a Digilent system board. It currently has only been tested with HDMI
 * input streams, but other video sources such as an image sensor or displayport
 * should work with minor modifications. Current features include seamless
 * framebuffer-swapping, ability to pause/start streaming, and attaching a callback
 * function that is called on video detection and signal loss. The driver can
 * also be configured to automatically start streaming into memory when a video
 * signal is detected.
 *
 * To use this driver you must have a Xilinx Video Timing Controller core (vtc),
 * Xilinx axi_vdma core, Xilinx Video to AXI Stream core, Digilent DVI2RGB core,
 * and an axi_gpio core (for Hot-plug detect and signal lock detection) present
 * in your design. See the Video/Display reference projects for your system board
 * to see how they need to be connected. Digilent reference projects and IP cores
 * can be found at www.github.com/Digilent.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who          		Date         	Changes
 * ----- ------------ 		-----------  	----------------------------------
 * 1.00  Sam Bobrowicz 		2015-Nov-25 	First Release
 * 1.01  Jonathan Aubert 	2016-Dec-16 	Adapted to be used with Linux
 * </pre>
 *
 *****************************************************************************/
/*
 * TODO: The VDMA debugging options can be enabled in Vivado IPI to enable
 *       the frame count functionality. This provides an interrupt that is
 *       triggered at the end of every frame, and also provides the ability
 *       to grab a single frame and have the VDMA core then automatically
 *       halt. We should evaluate if adding this functionality is worth requiring
 *       users to enable the Debug mode of the VDMA core (it requires some
 *       manual TCL commands to be run).
 * TODO: Currently VDMA errors (typically SOFLate) crop up every time
 *       The stream is started after having been stopped. The error appears to
 *       only be with the first frame that is input, as it goes away when
 *       it is cleared and the stream is still running. This shouldn't
 *       be a problem with the controller now, since we don't have the
 *       ability to just grab a single frame, but if we add frame count
 *       support or a frame finished interrupt then this will likely need
 *       to be addressed.
 * TODO: Make the VTC struct get passed as a pointer so that this driver
 *       will work with designs that share the VTC with a display pipeline
 *       that uses it in generator mode. Will need to test that all accesses
 *       to the VTC don't interfere with generator functionality. Note that
 *       this requires both the Display and Video pipelines use the same
 *       pixel clock.
 * TODO: It would be nice to remove the need for users above this to access
 *       members of the VideoCapture struct manually. This can be done by
 *       implementing get/set functions for things like video timings, state,
 *       current frame, etc..
 */



/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */



#include "video_capture.h"
#include "stdio.h"
#include "unistd.h"
#include "../cv/img_specs.h"
/* ------------------------------------------------------------ */
/*				Procedure Definitions							*/
/* ------------------------------------------------------------ */

/***	VideoStop(VideoCapture *videoPtr)
**
**	Parameters:
**		videoPtr - Pointer to the initialized VideoCapture struct
**
**	Return Value: int
**		XST_SUCCESS if successful. Currently always returns success
**
**	Description:
**		Halts input stream into memory
**
*/
int VideoStop(VideoCapture *videoPtr)
{

	/*
	 * If already stopped, do nothing
	 */
	if (videoPtr->state == VIDEO_PAUSED || videoPtr->state == VIDEO_DISCONNECTED)
	{
		return XST_SUCCESS;
	}

	XAxiVdma_Reset(videoPtr->vdma, XAXIVDMA_WRITE);
	while(XAxiVdma_ResetNotDone(videoPtr->vdma, XAXIVDMA_WRITE));
	videoPtr->state = VIDEO_PAUSED;

	return XST_SUCCESS;
}
/* ------------------------------------------------------------ */

/***	VideoStart(VideoCapture *videoPtr)
**
**	Parameters:
**		videoPtr - Pointer to the initialized VideoCapture struct
**
**	Return Value: int
**		XST_SUCCESS if successful
**		XST_NO_DATA if the Video input is disconnected
**		XST_FAILURE otherwise
**
**	Errors:
**
**	Description:
**		Starts writing the Video stream into memory.
**
*/
int VideoStart(VideoCapture *videoPtr)
{
	int Status;
	int i;

	if (videoPtr->state == VIDEO_DISCONNECTED)
		return XST_NO_DATA;
	if (videoPtr->state == VIDEO_STREAMING)
		return XST_SUCCESS;

	/*
	 * Configure the VDMA to access a frame with the same dimensions as the
	 * current mode
	 */
	videoPtr->vdmaConfig.VertSizeInput = videoPtr->timing.VActiveVideo;
	videoPtr->vdmaConfig.HoriSizeInput = videoPtr->timing.HActiveVideo * 2;
	videoPtr->vdmaConfig.FixedFrameStoreAddr = videoPtr->curFrame;
	/*
	 *Also reset the stride and address values, in case the user manually changed them
	 */
	videoPtr->vdmaConfig.Stride = videoPtr->stride;
	for (i = 0; i < VIDEO_NUM_FRAMES; i++)
	{
		videoPtr->vdmaConfig.FrameStoreStartAddr[i] = (u32)  videoPtr->framePtr[i];
	}

	printf("Starting VDMA for Video capture\n\r");
	Status = XAxiVdma_DmaConfig(videoPtr->vdma, XAXIVDMA_WRITE, &(videoPtr->vdmaConfig));
	if (Status != XST_SUCCESS)
	{
		printf("Write channel config failed %d\r\n", Status);
		return XST_FAILURE;
	}
	Status = XAxiVdma_DmaSetBufferAddr(videoPtr->vdma, XAXIVDMA_WRITE, videoPtr->vdmaConfig.FrameStoreStartAddr);
	if (Status != XST_SUCCESS)
	{
		printf("Write channel set buffer address failed %d\r\n", Status);
		return XST_FAILURE;
	}
	Status = XAxiVdma_DmaStart(videoPtr->vdma, XAXIVDMA_WRITE);
	if (Status != XST_SUCCESS)
	{
		printf("Start Write transfer failed %d\r\n", Status);
		return XST_FAILURE;
	}
	Status = XAxiVdma_StartParking(videoPtr->vdma, videoPtr->curFrame, XAXIVDMA_WRITE);
	if (Status != XST_SUCCESS)
	{
		printf("Unable to park the Write channel %d\r\n", Status);
		return XST_FAILURE;
	}

	videoPtr->state = VIDEO_STREAMING;

	return XST_SUCCESS;
}

/* ------------------------------------------------------------ */




/***	VideoInitialize(VideoCapture *videoPtr, INTC *intCtrl, XAxiVdma *vdma, u16 gpioId, u16 vtcId, u32 vtcIrptId, u8 *framePtr[VIDEO_NUM_FRAMES], u32 stride, u32 startOnDet)

**
**	Parameters:
**		videoPtr - Pointer to the struct that will be initialized
**		intCtrl - pointer to the initialized interrupt controller. Prior to calling this function, the interrupt
**				  controller must be initialized and enabled using the Digilent intc library. The vector table must also
**				  include entries for the VTC controller and AXI GPIO used with this driver. You can use the videoGpioIvt and
**				  videoVtcIvt Macros for creating the vector table entries.
**		vdma - Pointer to the initialized VDMA driver struct.
**		gpioId - Device ID of the AXI GPIO core as found in xparameters.h. This is the core connect to the HPD and locked signals
**		vtcId - Device ID of the VTC core as found in xparameters.h
**		vtcIrptId - Interrupt ID of the VTC core
**		framePtr - array of pointers to the framebuffers. The framebuffers must be instantiated above this driver
**		stride - line stride of the framebuffers. This is the number of bytes between the start of one line and the start of another.
**		startOnDet - Flag indicating if you want to begin streaming as soon as a video signal is detected. Non-zero values mean yes.
**
**	Return Value: int
**		XST_SUCCESS if successful, XST_FAILURE otherwise
**
**	Errors:
**
**	Description:
**		Initializes the driver struct for use. The interrupt controller should be enabled before calling this function to ensure
**		a locked interrupt is not missed. After this function has been called, VideoStart,VideoStop, VideoSetCallBack, and VideoChangeFrame
**		can all be called at will.
**
*/
int VideoInitialize(VideoCapture *videoPtr, XAxiVdma *vdma, u16 vtcId, u16 gpio_Id, u32 addVirtVtc, u32 addVirtGpio, u8 *framePtr[VIDEO_NUM_FRAMES], u32 stride, u32 startOnDet)
{
	int Status;
	int i;
	//To get HW info and then set gpio_video to '1' (HDMI source)
	XGpio_Config *gpioVideoCfg;

	/*
	 * Initialize all the fields in the VideoCapture struct
	 */
	videoPtr->curFrame = 0;

	//get the frame addresses
	for (i = 0; i < VIDEO_NUM_FRAMES; i++)
	{
		videoPtr->framePtr[i] = framePtr[i];
	}

	videoPtr->state = VIDEO_DISCONNECTED;
	videoPtr->stride = stride;
	videoPtr->vdma = vdma;
	videoPtr->startOnDetect = startOnDet;

	/*
	 * Initialize the VDMA Read configuration struct
	 */
	videoPtr->vdmaConfig.FrameDelay = 0;
	videoPtr->vdmaConfig.EnableCircularBuf = 1;
	videoPtr->vdmaConfig.EnableSync = 0;
	videoPtr->vdmaConfig.PointNum = 0;
	videoPtr->vdmaConfig.EnableFrameCounter = 0;

	/* Initialize the GPIO driver. If an error occurs then exit */

	printf("Video Init started\n\r");
	//Get lookup configs
	gpioVideoCfg = XGpio_LookupConfig(gpio_Id);

	//init gpio instance
	Status = XGpio_CfgInitialize(&videoPtr->gpio, gpioVideoCfg, addVirtGpio);
	if (Status != XST_SUCCESS)
	{
		printf("XGPIO video Init failed\n\r");
		return XST_FAILURE;
	}

	/*
	 * Setup direction registers,
	 */
	//and ensure HPD is low
	//XGpio_DiscreteWrite(&videoPtr->gpio, 1, 0);
	XGpio_SetDataDirection(&videoPtr->gpio, 1, 0); //Set HPD channel as output
	XGpio_SetDataDirection(&videoPtr->gpio, 2, 1); //Set Locked channel as input

	/*
	 * Set HPD high, which will signal the HDMI source to begin transmitting.
	 */
	XGpio_DiscreteWrite(&videoPtr->gpio, 1, 1);

	//Wait for HDMI signal and get (hard coded) timings
	VideoDetect(videoPtr);

	printf("Video init : OK\n\r");

	return XST_SUCCESS;
}

/* ------------------------------------------------------------ */

/***	VideoChangeFrame(VideoCapture *videoPtr, u32 frameIndex)
**
**	Parameters:
**		videoPtr - Pointer to the initialized VideoCapture struct
**		frameIndex - Index of the framebuffer to change to (must
**				be between 0 and (DISPLAY_NUM_FRAMES - 1))
**
**	Return Value: int
**		XST_SUCCESS if successful, XST_FAILURE otherwise
**
**	Errors:
**
**	Description:
**		Changes the frame that video is currently streamed to.
**
*/

int VideoChangeFrame(VideoCapture *videoPtr, u32 frameIndex)
{
	int Status;

	videoPtr->curFrame = frameIndex;
	/*
	 * If currently running, then the DMA needs to be told to start reading from the desired frame
	 * at the end of the current frame
	 */
	if (videoPtr->state == VIDEO_STREAMING)
	{
		Status = XAxiVdma_StartParking(videoPtr->vdma, videoPtr->curFrame, XAXIVDMA_WRITE);
		if (Status != XST_SUCCESS)
		{
			printf("Cannot change frame, unable to start parking %d\r\n", Status);
			return XST_FAILURE;
		}
	}

	return XST_SUCCESS;
}

/* ------------------------------------------------------------ */

/***	VideoDetect(VideoCapture *videoPtr)
**
**	Parameters:
**		videoPtr - Pointer to the initialized VideoCapture struct
**
**	Return Value:
**
**	Errors:
**
**	Description:
**		Blocking function that waits for HDMI signal and sets
**		video timings (hard coded).
**
*/
void VideoDetect(VideoCapture *videoPtr)
{
	XGpio *GpioPtr = &videoPtr->gpio;
	u32 locked = 0;

	//Wait clock from HDMI
	while(!locked)
	{
		locked = XGpio_DiscreteRead(GpioPtr, 2);
	}

	sleep(3);


#ifdef RES_720p
	//manually hard coded for 1280*720 60Hz
	videoPtr->timing.HActiveVideo = 0x0500;
	videoPtr->timing.HFrontPorch = 0x006e;
	videoPtr->timing.HSyncWidth = 0x0028;
	videoPtr->timing.HBackPorch = 0x00dc;
	videoPtr->timing.HSyncPolarity = 0x0001;
	videoPtr->timing.VActiveVideo = 0x02d0;
	videoPtr->timing.V0FrontPorch = 0x0005;
	videoPtr->timing.V0SyncWidth = 0x0006;
	videoPtr->timing.V0BackPorch = 0x0015;
	videoPtr->timing.V1FrontPorch = 0xfd31;
	videoPtr->timing.V1SyncWidth = 0x0001;
	videoPtr->timing.V1BackPorch = 0x0000;
	videoPtr->timing.VSyncPolarity = 0x0001;
	videoPtr->timing.Interlaced = 0;

#else
	//manually hard coded for 1920*1080 60Hz
	videoPtr->timing.HActiveVideo = 0x0780;
	videoPtr->timing.HFrontPorch = 0x0030;
	videoPtr->timing.HSyncWidth = 0x0020;
	videoPtr->timing.HBackPorch = 0x0050;
	videoPtr->timing.HSyncPolarity = 0x0001;
	videoPtr->timing.VActiveVideo = 0x0438;
	videoPtr->timing.V0FrontPorch = 0x0003;
	videoPtr->timing.V0SyncWidth = 0x0006;
	videoPtr->timing.V0BackPorch = 0x0018;
	videoPtr->timing.V1FrontPorch = 0xfbc9;
	videoPtr->timing.V1SyncWidth = 0x0001;
	videoPtr->timing.V1BackPorch = 0x0000;
	videoPtr->timing.VSyncPolarity = 0x0000;
	videoPtr->timing.Interlaced = 0;
#endif

	videoPtr->state = VIDEO_PAUSED;
	if (videoPtr->startOnDetect)	//autostart when required
	{
		VideoStart(videoPtr);
	}

}
