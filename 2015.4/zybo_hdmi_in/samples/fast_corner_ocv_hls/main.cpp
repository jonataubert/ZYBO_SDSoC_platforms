/************************************************************************/
/*																		*/
/*	main.cpp															*/
/*																		*/
/************************************************************************/
/*	Author: Jonathan Aubert												*/
/************************************************************************/
/*  Module Description: 												*/
/*																		*/
/*		This SDSoC project detects the detected corners from the		*/
/*		video input (HDMI) and displays them on the video output (VGA) 	*/
/*		Two resolutions can be used (see #define 720p in img_specs.h) : */
/*		1280x720 or 1920x1080											*/
/*		Both OpenCV2.4.5 and HLS video libraries are used for software	*/
/*		and hardware implementations. Runs Linux kernel 4.0.0			*/
/*																		*/
/*																		*/
/* 		Warning : external power supply might be required! 				*/
/*																		*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/* 																		*/
/*		16.12.2016(Jonathan Aubert) : First version						*/
/*																		*/
/************************************************************************/

/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <sstream>
#include <iostream>
#include <pthread.h>
#include "sds_lib.h"


//Includes to configure PL
#include "xparams/xparameters.h"
#include "gpio/xgpio.h"
#include "vdma/xaxivdma.h"
#include "xillinio/xillinio.h"
#include "video_capture/video_capture.h"
#include "display_ctrl/display_ctrl.h"

//Image processing
#include <opencv2/opencv.hpp>
#include "cv/img_specs.h"
#include "cv/my_ocv_sw.h"
#include "cv/my_ocv_hw.h"

//Tools (average moving windows)
#include "tools/MoveAvg.h"

#define DBG

/*
 * XPARAMETERS redefines
 */
#define DYNCLK_BASEADDR XPAR_AXI_DYNCLK_0_BASEADDR
#define VGA_VDMA_ID XPAR_AXIVDMA_0_DEVICE_ID
#define DISP_VTC_ID XPAR_VTC_0_DEVICE_ID
#define VID_VTC_ID XPAR_VTC_1_DEVICE_ID
#define VID_GPIO_ID XPAR_AXI_GPIO_VIDEO_DEVICE_ID
#define VID_VTC_IRPT_ID XPS_FPGA3_INT_ID
#define VID_GPIO_IRPT_ID XPS_FPGA4_INT_ID
#define SCU_TIMER_ID XPAR_SCUTIMER_DEVICE_ID
#define UART_BASEADDR XPAR_PS7_UART_1_BASEADDR

//Time measurement
#define CPU_FCLK_HZ 650000000
#define SAMPLE_N_MAX 64	//get max 64 sample
#define TIME_STAMP_INIT() unsigned long long clockStart, clockEnd;\
		double timeMes; MoveAvg swAvg(SAMPLE_N_MAX),hwAvg(SAMPLE_N_MAX);\
		clockStart = sds_clock_counter();
#define TIME_STAMP() 	{ clockEnd = sds_clock_counter(); \
		timeMes = (double)(clockEnd-clockStart)/CPU_FCLK_HZ;}
#define TIME_STAMP_RESTART() clockStart = sds_clock_counter();


#define BAD_PHYS_ADDR 0xFFFFFFFF
//the allocated size of memory (see in Vivado)
#define MAP_SIZE_DRIVERS 0x00010000
//the allocated size for a framebuffer in the kernel module
#define MAP_SIZE_FB (1013*4096)


enum MenuStates
{
    MENU_PT, MENU_OCV, MENU_HW, MENU_FPS, MENU_EXIT
};

/* ------------------------------------------------------------ */
/*					Procedure Declarations						*/
/* ------------------------------------------------------------ */

/**
 * @brief Initialize the IPs that are part of the capture and display chain.
 */
void DemoInitialize();

/**
 * @brief Get the physical address of the framebuffer according to the number of the frame
 * @param fbNumber the number of the frame (0;1;2)
 */
u_int32_t getFramebufferPhysAddr(u_int32_t fbNumber);

/**
 * @brief Allows to choose what to do from menu console, returns the choice.
 * @return The number related to the choice
 */
u_int32_t chooseMenu();

/**
 * @brief Thread displaying menu and getting user choice
 */
void *threadChooseMenu(void *threadarg);

/* ------------------------------------------------------------ */
/*				Global Variables								*/
/* ------------------------------------------------------------ */

//Display and Video Driver structs
DisplayCtrl dispCtrl;
XAxiVdma vdma;
VideoCapture videoCapt;

//Pointers (physical addr) on frame buffers
u_int16_t *pPhysFrames[DISPLAY_NUM_FRAMES];

//Class to perform address mapping translation : physical -> virtual addr
xil_lin_io addrMap;

//For the menu
pthread_mutex_t menuLock;
volatile MenuStates menuSelect = MENU_PT;


/**
 * Entry point main.
 * @param argc number of arguments.
 * @param argv arguments (as an array of strings).
 * @return status code.
 */
int main(int argc, char **argv)
{

	//framebuffer access
	u_int16_t *imBuff0, *imBuff1, *imBuff2;

	//allocates Mat object
	cv::Mat cvImSrc(IM_HEIGHT, IM_WIDTH, CV_8UC3);
	cv::Mat cvImDst(IM_HEIGHT, IM_WIDTH, CV_8UC3);
	u_int32_t nbCorners = 0;

	TIME_STAMP_INIT()

	//load the kernel module that allocates 3 contiguous frame buffers
	system("/sbin/insmod /lib/modules/4.0.0-xilinx-apf/extra/myVDMA.ko");

	//get the physical address of the framebuffers
	for(int i = 0; i < DISPLAY_NUM_FRAMES ; i++)
	{
		//mapping of memory allocated by DMA
		pPhysFrames[i] = (u_int16_t *)getFramebufferPhysAddr(i);
		if (pPhysFrames[i] == (u_int16_t *)BAD_PHYS_ADDR)
		{
			return EXIT_FAILURE;
		}
	}

	//perform mapping using SDSoC mmap (DMA transfer required)
	imBuff0 = (u_int16_t *)sds_mmap(pPhysFrames[0], IM_SIZE_BYTE,NULL);
	imBuff1 = (u_int16_t *)sds_mmap(pPhysFrames[1], IM_SIZE_BYTE,NULL);
	imBuff2 = (u_int16_t *)sds_mmap(pPhysFrames[2], IM_SIZE_BYTE,NULL);


	//initialize PL (IPs)
	DemoInitialize();

	bool firstLoop = true;
	MenuStates oldMenuSelect = MENU_HW;

	if(pthread_mutex_init(&menuLock, NULL) != 0)
	{
		printf("Mutex init failed\n");
		return EXIT_FAILURE;
	}

	//create and run a thread for the menu selection
	pthread_t thread;
	int rc;
	std::cout << "main() : creating thread for menu selection, " << std::endl;
	rc = pthread_create(&thread, NULL, threadChooseMenu, NULL);
	if (rc){
		std::cout << "Error:unable to create thread," << rc << std::endl;
		return EXIT_FAILURE;
	}

	//main behavior
	while(menuSelect != MENU_EXIT)
	{
		//new loop when new choice
		if(menuSelect != oldMenuSelect)
		{
			//display current measure
			if(menuSelect == MENU_FPS)
			{
				pthread_mutex_lock(&menuLock);
				//Stay in previous mode
				menuSelect = oldMenuSelect;
				pthread_mutex_unlock(&menuLock);

				if(oldMenuSelect == MENU_OCV)
				{
					printf("OCV_PERFS with last %d samples:\nMean time : %e [s]\nMean freq : %e [Hz]\n",
							swAvg.getSampleNB(), swAvg.getCurrentMean(), (double)1.0/swAvg.getCurrentMean());
				}
				else if (oldMenuSelect == MENU_HW)
				{
					printf("HW_PERFS with last %d samples:\nMean time : %e [s]\nMean freq : %e [Hz]\n",
							hwAvg.getSampleNB(), hwAvg.getCurrentMean(),(double)1.0/hwAvg.getCurrentMean());
				}
			}
			else
			{
				firstLoop = true;
			}
			oldMenuSelect = menuSelect;
		}

		//pass through ?
		if (menuSelect == MENU_PT)
		{
			//display framebuffer0
			if(dispCtrl.curFrame != 0)
			{
				DisplayChangeFrame(&dispCtrl,0);
			}
		}
		else
		{
			//display framebuffer1
			if(dispCtrl.curFrame != 1)
			{
				DisplayChangeFrame(&dispCtrl,1);
			}

			switch(menuSelect)
			{
				case MENU_OCV:
					if(firstLoop)
					{
						printf("ocv software implementation running ! \n");
					}
					TIME_STAMP_RESTART()
					//Copy img from frambuffer0
					ocvFbToMat(imBuff0,IM_HEIGHT,IM_WIDTH,IM_STRIDE,&cvImSrc);
//					ocvSwPassthrough(&cvImSrc,&cvImDst);
//					ocvSwColorGrey(&cvImSrc,&cvImDst);
					nbCorners = ocvSwFastCorner(&cvImSrc,&cvImDst);
					//copyback to framebuffer1
					ocvMatToFb(imBuff1,IM_HEIGHT,IM_WIDTH,IM_STRIDE,&cvImDst);
					//Store timestamp to sliding mean
					TIME_STAMP()
					swAvg.addSample(timeMes);
					if(firstLoop)
					{
						printf("%d corners detected\n", nbCorners);
					}
				break;

				case MENU_HW:
					if(firstLoop)
					{
						printf("cv hardware implementation running ! \n");
					}
					TIME_STAMP_RESTART()
//					cvHwPassthrough((u_int16_t*)imBuff0, (u_int16_t*)imBuff1);
//					cvHwColorGrey((u_int16_t*)imBuff0, (u_int16_t*)imBuff1);
					cvHwFastCorner((u_int16_t*)imBuff0, (u_int16_t*)imBuff1);
					TIME_STAMP()	//Add time to sliding mean
					hwAvg.addSample(timeMes);
				break;

				default:
				break;
			}
		}
		firstLoop = false;
	}

	//wait for menu thread to finish
	pthread_join(thread,NULL);
	pthread_mutex_destroy(&menuLock);
	//Stop video
	VideoStop(&videoCapt);
	DisplayStop(&dispCtrl);
	//free mappings
	addrMap.~xil_lin_io();
	sds_munmap(imBuff0);
	sds_munmap(imBuff1);
	sds_munmap(imBuff2);

	return EXIT_SUCCESS;
}

u_int32_t getFramebufferPhysAddr(u_int32_t fbNumber)
{
	FILE *buff_phys;
	char pathToDrv[100] = "/sys/bus/platform/drivers/myVDMA/buffer_base_phys";
	char sfbNum[2];
	char sPhysAddr[20];
	u_int32_t physAddr = BAD_PHYS_ADDR;

	//number of framebuffer is 3 (0,1,2)
	if (fbNumber > DISPLAY_NUM_FRAMES-1)
	{
		printf( "ERROR: %u is a bad frame number\n",fbNumber);
		return BAD_PHYS_ADDR;
	}

	sprintf(sfbNum,"%u",fbNumber);
	strcat(pathToDrv, sfbNum);

	buff_phys = fopen(pathToDrv, "r");

	if(buff_phys == NULL)
	{
		printf( "ERROR: could not open \"/myVDMA\"...\n" );
		return BAD_PHYS_ADDR;
	}
	fread(sPhysAddr, 10, 1, buff_phys);
	//convert string
	sscanf(sPhysAddr, "%X", &physAddr);

	return physAddr;
}

void DemoInitialize()
{
	int32_t Status;
	XAxiVdma_Config *vdmaConfig;	//to get HW configs from xparameters.h
	int32_t startOnDetect = 0;

	//Map each peripheral in the virtual memory
	addrMap.mapAddAddr(XPAR_AXI_GPIO_VIDEO_BASEADDR,MAP_SIZE_DRIVERS);
	addrMap.mapAddAddr(XPAR_V_TC_0_BASEADDR,MAP_SIZE_DRIVERS);
	addrMap.mapAddAddr(XPAR_AXI_DYNCLK_0_BASEADDR,MAP_SIZE_DRIVERS);
	addrMap.mapAddAddr(XPAR_AXI_VDMA_0_BASEADDR,MAP_SIZE_DRIVERS);

#ifdef DBG
	printf("Check with printMap :\n");
	addrMap.printMap();
#endif

	/*
	 * Initialize VDMA driver from xparameters.h
	 */
	vdmaConfig = XAxiVdma_LookupConfig(VGA_VDMA_ID);
	if (!vdmaConfig)
	{
		printf("No video DMA found for ID %d\r\n", VGA_VDMA_ID);
		return;
	}

	//Here configure the VDMA instance "vdma" with the virtual address.
	Status = XAxiVdma_CfgInitialize(&vdma, vdmaConfig, addrMap.getVirtAddr(XPAR_AXI_VDMA_0_BASEADDR));
	if (Status != XST_SUCCESS)
	{
		printf("VDMA Configuration Initialization failed %d\r\n", Status);
		return;
	}


	/*
	 * Initialize the Display controller
	 */
	Status = DisplayInitialize(&dispCtrl, &vdma, DISP_VTC_ID, addrMap.getVirtAddr(XPAR_V_TC_0_BASEADDR), addrMap.getVirtAddr(XPAR_AXI_DYNCLK_0_BASEADDR), (u8**) pPhysFrames, IM_STRIDE_BYTE);
	if (Status != XST_SUCCESS)
	{
		printf("Display Ctrl initialization failed during demo initialization%d\r\n", Status);
		return;
	}
	else
	{
		printf("Display init ok\n");
	}
	//Start display
	Status = DisplayStart(&dispCtrl);
	if (Status != XST_SUCCESS)
	{
		printf("Couldn't start display during demo initialization%d\r\n", Status);
		return;
	}

#ifdef DBG
	printf("DEBUG :Time to init capture (HDMI input) and get timings\n");
#endif

	/*
	 * Initialize the Video Capture device
	 * Warning : blocks until HDMI signal detected
	 */
	Status = VideoInitialize(&videoCapt, &vdma, VID_VTC_ID, VID_GPIO_ID,addrMap.getVirtAddr(XPAR_V_TC_1_BASEADDR),addrMap.getVirtAddr(XPAR_AXI_GPIO_VIDEO_BASEADDR), (u8**) pPhysFrames , IM_STRIDE_BYTE, startOnDetect);
	if (Status != EXIT_SUCCESS)
	{
		printf("Video Ctrl initialization failed during demo initialization\r\n");
		return;
	}

	//Start Capture from HDMI
	Status = VideoStart(&videoCapt);
	if (Status != EXIT_SUCCESS)
	{
		printf("Video start failed during demo initialization\r\n");
		return;
	}
}

u_int32_t chooseMenu()
{
	std::string input = "";
	u_int32_t myNumber = 0;
	while(1)
	{
		std::cout << "Running choices :\n0 : pass through\n1 : OpenCV\n2 : HW accelerated CV\n3 : getFPS\n4 : quit\n";
		std::getline(std::cin, input);

		// Converts from string to number
		std::stringstream myStream(input);
		if (myStream >> myNumber)
		{
			if (myNumber<5)
			{
				break;
			}
		}
		std::cout << "Invalid number, please try again" << std::endl;
	}
	std::cout << "You entered: " << myNumber << std::endl;
	return myNumber;
}

void *threadChooseMenu(void *threadarg)
{
	MenuStates menuStates;
	while(menuSelect != MENU_EXIT)
	{
		menuStates = (MenuStates)chooseMenu();

		pthread_mutex_lock(&menuLock);
		menuSelect = menuStates;
		pthread_mutex_unlock(&menuLock);
	}
	return NULL;
}
