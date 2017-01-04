# Configure project for OpenCV

## In project's properties:

Go to "C/C++ Build" -> "Settings" and under "Tools Settings" 
"SDS++ Linker"  -> "Libraries"

### Add the following libraries (-l) :

	opencv_core
	opencv_features2d
	opencv_highgui
	opencv_imgproc

### Now add -rpath link :

Go to "C/C++ Build" -> "Settings" and under "Tools Settings" 
"SDS++ Linker"  -> "Miscellaneous"

Add the following string in "Linker Flags" :

	-Wl,-rpath-link=/PathToPlatforms/zybo_hdmi_in/arm-xilinx-linux-gnueabi/lib

--------------------------------------------------------------------------
# Choose resolution and enable HW acceleration


## To use a resolution of 1920*1080 :

In src/cv/img_specs.h, comment #define 720p 

	To enable 1080p HW acceleration, mark cvHwFastCorner() as HW,
	set its clock to 131.25 [MHz] and 
	set data motion clock frequency to 116.67 [MHz] 

--------------------------------------------------------------------------

## To use a resolution of 1280*720 :

In src/cv/img_specs.h, uncomment #define 720p 

	To enable 720p HW acceleration, mark cvHwFastCorner() as HW,	
	set its clock to 116.67 [MHz] and 								
	set data motion clock frequency to 116.67 [MHz]  				



# Note :
According to your HDMI source, you might get some artifacts when using 
a resolution of 1920*1080, this is normal as the board is not supposed to 
support this resolution. You also might need an external power supply!