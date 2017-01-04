#!/bin/sh

# 
# Vivado(TM)
# runme.sh: a Vivado-generated Runs Script for UNIX
# Copyright 1986-2015 Xilinx, Inc. All Rights Reserved.
# 

if [ -z "$PATH" ]; then
  PATH=/opt/Xilinx/SDSoC/2015.4/SDK/2015.4/bin:/opt/Xilinx/SDSoC/2015.4/Vivado/2015.4/ids_lite/ISE/bin/lin64:/opt/Xilinx/SDSoC/2015.4/Vivado/2015.4/bin
else
  PATH=/opt/Xilinx/SDSoC/2015.4/SDK/2015.4/bin:/opt/Xilinx/SDSoC/2015.4/Vivado/2015.4/ids_lite/ISE/bin/lin64:/opt/Xilinx/SDSoC/2015.4/Vivado/2015.4/bin:$PATH
fi
export PATH

if [ -z "$LD_LIBRARY_PATH" ]; then
  LD_LIBRARY_PATH=/opt/Xilinx/SDSoC/2015.4/Vivado/2015.4/ids_lite/ISE/lib/lin64
else
  LD_LIBRARY_PATH=/opt/Xilinx/SDSoC/2015.4/Vivado/2015.4/ids_lite/ISE/lib/lin64:$LD_LIBRARY_PATH
fi
export LD_LIBRARY_PATH

HD_PWD='/home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.runs/synth_1'
cd "$HD_PWD"

HD_LOG=runme.log
/bin/touch $HD_LOG

ISEStep="./ISEWrap.sh"
EAStep()
{
     $ISEStep $HD_LOG "$@" >> $HD_LOG 2>&1
     if [ $? -ne 0 ]
     then
         exit
     fi
}

EAStep vivado -log zybo_hdmi_in_wrapper.vds -m64 -mode batch -messageDb vivado.pb -notrace -source zybo_hdmi_in_wrapper.tcl
