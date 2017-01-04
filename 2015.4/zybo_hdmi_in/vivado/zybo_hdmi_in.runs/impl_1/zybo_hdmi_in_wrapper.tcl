proc start_step { step } {
  set stopFile ".stop.rst"
  if {[file isfile .stop.rst]} {
    puts ""
    puts "*** Halting run - EA reset detected ***"
    puts ""
    puts ""
    return -code error
  }
  set beginFile ".$step.begin.rst"
  set platform "$::tcl_platform(platform)"
  set user "$::tcl_platform(user)"
  set pid [pid]
  set host ""
  if { [string equal $platform unix] } {
    if { [info exist ::env(HOSTNAME)] } {
      set host $::env(HOSTNAME)
    }
  } else {
    if { [info exist ::env(COMPUTERNAME)] } {
      set host $::env(COMPUTERNAME)
    }
  }
  set ch [open $beginFile w]
  puts $ch "<?xml version=\"1.0\"?>"
  puts $ch "<ProcessHandle Version=\"1\" Minor=\"0\">"
  puts $ch "    <Process Command=\".planAhead.\" Owner=\"$user\" Host=\"$host\" Pid=\"$pid\">"
  puts $ch "    </Process>"
  puts $ch "</ProcessHandle>"
  close $ch
}

proc end_step { step } {
  set endFile ".$step.end.rst"
  set ch [open $endFile w]
  close $ch
}

proc step_failed { step } {
  set endFile ".$step.error.rst"
  set ch [open $endFile w]
  close $ch
}

set_msg_config -id {HDL 9-1061} -limit 100000
set_msg_config -id {HDL 9-1654} -limit 100000

start_step init_design
set rc [catch {
  create_msg_db init_design.pb
  set_property design_mode GateLvl [current_fileset]
  set_property webtalk.parent_dir /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.cache/wt [current_project]
  set_property parent.project_path /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.xpr [current_project]
  set_property ip_repo_paths {
  /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.cache/ip
  /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.ipdefs/repo_0
} [current_project]
  set_property ip_output_repo /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.cache/ip [current_project]
  add_files -quiet /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.runs/synth_1/zybo_hdmi_in_wrapper.dcp
  read_xdc -prop_thru_buffers -ref zybo_hdmi_in_axi_gpio_video_0 -cells U0 /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_axi_gpio_video_0/zybo_hdmi_in_axi_gpio_video_0_board.xdc
  set_property processing_order EARLY [get_files /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_axi_gpio_video_0/zybo_hdmi_in_axi_gpio_video_0_board.xdc]
  read_xdc -ref zybo_hdmi_in_axi_gpio_video_0 -cells U0 /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_axi_gpio_video_0/zybo_hdmi_in_axi_gpio_video_0.xdc
  set_property processing_order EARLY [get_files /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_axi_gpio_video_0/zybo_hdmi_in_axi_gpio_video_0.xdc]
  read_xdc -ref zybo_hdmi_in_axi_vdma_0_0 -cells U0 /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_axi_vdma_0_0/zybo_hdmi_in_axi_vdma_0_0.xdc
  set_property processing_order EARLY [get_files /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_axi_vdma_0_0/zybo_hdmi_in_axi_vdma_0_0.xdc]
  read_xdc -ref zybo_hdmi_in_dvi2rgb_0_0 -cells U0 /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_dvi2rgb_0_0/src/dvi2rgb.xdc
  set_property processing_order EARLY [get_files /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_dvi2rgb_0_0/src/dvi2rgb.xdc]
  read_xdc -prop_thru_buffers -ref zybo_hdmi_in_proc_sys_reset_0_0 /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_proc_sys_reset_0_0/zybo_hdmi_in_proc_sys_reset_0_0_board.xdc
  set_property processing_order EARLY [get_files /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_proc_sys_reset_0_0/zybo_hdmi_in_proc_sys_reset_0_0_board.xdc]
  read_xdc -ref zybo_hdmi_in_proc_sys_reset_0_0 /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_proc_sys_reset_0_0/zybo_hdmi_in_proc_sys_reset_0_0.xdc
  set_property processing_order EARLY [get_files /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_proc_sys_reset_0_0/zybo_hdmi_in_proc_sys_reset_0_0.xdc]
  read_xdc -prop_thru_buffers -ref zybo_hdmi_in_proc_sys_reset_1_0 /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_proc_sys_reset_1_0/zybo_hdmi_in_proc_sys_reset_1_0_board.xdc
  set_property processing_order EARLY [get_files /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_proc_sys_reset_1_0/zybo_hdmi_in_proc_sys_reset_1_0_board.xdc]
  read_xdc -ref zybo_hdmi_in_proc_sys_reset_1_0 /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_proc_sys_reset_1_0/zybo_hdmi_in_proc_sys_reset_1_0.xdc
  set_property processing_order EARLY [get_files /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_proc_sys_reset_1_0/zybo_hdmi_in_proc_sys_reset_1_0.xdc]
  read_xdc -prop_thru_buffers -ref zybo_hdmi_in_proc_sys_reset_3_0 /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_proc_sys_reset_3_0/zybo_hdmi_in_proc_sys_reset_3_0_board.xdc
  set_property processing_order EARLY [get_files /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_proc_sys_reset_3_0/zybo_hdmi_in_proc_sys_reset_3_0_board.xdc]
  read_xdc -ref zybo_hdmi_in_proc_sys_reset_3_0 /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_proc_sys_reset_3_0/zybo_hdmi_in_proc_sys_reset_3_0.xdc
  set_property processing_order EARLY [get_files /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_proc_sys_reset_3_0/zybo_hdmi_in_proc_sys_reset_3_0.xdc]
  read_xdc -prop_thru_buffers -ref zybo_hdmi_in_proc_sys_reset_hdmi_in_0 /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_proc_sys_reset_hdmi_in_0/zybo_hdmi_in_proc_sys_reset_hdmi_in_0_board.xdc
  set_property processing_order EARLY [get_files /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_proc_sys_reset_hdmi_in_0/zybo_hdmi_in_proc_sys_reset_hdmi_in_0_board.xdc]
  read_xdc -ref zybo_hdmi_in_proc_sys_reset_hdmi_in_0 /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_proc_sys_reset_hdmi_in_0/zybo_hdmi_in_proc_sys_reset_hdmi_in_0.xdc
  set_property processing_order EARLY [get_files /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_proc_sys_reset_hdmi_in_0/zybo_hdmi_in_proc_sys_reset_hdmi_in_0.xdc]
  read_xdc -ref zybo_hdmi_in_processing_system7_0_0 -cells inst /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_processing_system7_0_0/zybo_hdmi_in_processing_system7_0_0.xdc
  set_property processing_order EARLY [get_files /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_processing_system7_0_0/zybo_hdmi_in_processing_system7_0_0.xdc]
  read_xdc /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/constrs_1/imports/constraints/ZYBO_Master.xdc
  read_xdc -ref zybo_hdmi_in_axi_vdma_0_0 -cells U0 /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_axi_vdma_0_0/zybo_hdmi_in_axi_vdma_0_0_clocks.xdc
  set_property processing_order LATE [get_files /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_axi_vdma_0_0/zybo_hdmi_in_axi_vdma_0_0_clocks.xdc]
  read_xdc -ref zybo_hdmi_in_v_axi4s_vid_out_0_0 -cells inst /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_v_axi4s_vid_out_0_0/zybo_hdmi_in_v_axi4s_vid_out_0_0_clocks.xdc
  set_property processing_order LATE [get_files /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_v_axi4s_vid_out_0_0/zybo_hdmi_in_v_axi4s_vid_out_0_0_clocks.xdc]
  read_xdc -ref zybo_hdmi_in_v_tc_0_0 -cells U0 /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_v_tc_0_0/zybo_hdmi_in_v_tc_0_0_clocks.xdc
  set_property processing_order LATE [get_files /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_v_tc_0_0/zybo_hdmi_in_v_tc_0_0_clocks.xdc]
  read_xdc -ref zybo_hdmi_in_v_vid_in_axi4s_0_0 -cells inst /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_v_vid_in_axi4s_0_0/zybo_hdmi_in_v_vid_in_axi4s_0_0_clocks.xdc
  set_property processing_order LATE [get_files /home/enjon/Dropbox/01_DIPLOME/09_platforms/zybo_hdmi_light_linux/zybo_hdmi_in/vivado/zybo_hdmi_in.srcs/sources_1/bd/zybo_hdmi_in/ip/zybo_hdmi_in_v_vid_in_axi4s_0_0/zybo_hdmi_in_v_vid_in_axi4s_0_0_clocks.xdc]
  link_design -top zybo_hdmi_in_wrapper -part xc7z010clg400-1
  close_msg_db -file init_design.pb
} RESULT]
if {$rc} {
  step_failed init_design
  return -code error $RESULT
} else {
  end_step init_design
}

start_step opt_design
set rc [catch {
  create_msg_db opt_design.pb
  catch {write_debug_probes -quiet -force debug_nets}
  opt_design 
  write_checkpoint -force zybo_hdmi_in_wrapper_opt.dcp
  report_drc -file zybo_hdmi_in_wrapper_drc_opted.rpt
  close_msg_db -file opt_design.pb
} RESULT]
if {$rc} {
  step_failed opt_design
  return -code error $RESULT
} else {
  end_step opt_design
}

start_step place_design
set rc [catch {
  create_msg_db place_design.pb
  catch {write_hwdef -file zybo_hdmi_in_wrapper.hwdef}
  place_design 
  write_checkpoint -force zybo_hdmi_in_wrapper_placed.dcp
  report_io -file zybo_hdmi_in_wrapper_io_placed.rpt
  report_utilization -file zybo_hdmi_in_wrapper_utilization_placed.rpt -pb zybo_hdmi_in_wrapper_utilization_placed.pb
  report_control_sets -verbose -file zybo_hdmi_in_wrapper_control_sets_placed.rpt
  close_msg_db -file place_design.pb
} RESULT]
if {$rc} {
  step_failed place_design
  return -code error $RESULT
} else {
  end_step place_design
}

start_step route_design
set rc [catch {
  create_msg_db route_design.pb
  route_design 
  write_checkpoint -force zybo_hdmi_in_wrapper_routed.dcp
  report_drc -file zybo_hdmi_in_wrapper_drc_routed.rpt -pb zybo_hdmi_in_wrapper_drc_routed.pb
  report_timing_summary -warn_on_violation -max_paths 10 -file zybo_hdmi_in_wrapper_timing_summary_routed.rpt -rpx zybo_hdmi_in_wrapper_timing_summary_routed.rpx
  report_power -file zybo_hdmi_in_wrapper_power_routed.rpt -pb zybo_hdmi_in_wrapper_power_summary_routed.pb
  report_route_status -file zybo_hdmi_in_wrapper_route_status.rpt -pb zybo_hdmi_in_wrapper_route_status.pb
  report_clock_utilization -file zybo_hdmi_in_wrapper_clock_utilization_routed.rpt
  close_msg_db -file route_design.pb
} RESULT]
if {$rc} {
  step_failed route_design
  return -code error $RESULT
} else {
  end_step route_design
}

start_step write_bitstream
set rc [catch {
  create_msg_db write_bitstream.pb
  catch { write_mem_info -force zybo_hdmi_in_wrapper.mmi }
  write_bitstream -force zybo_hdmi_in_wrapper.bit 
  catch { write_sysdef -hwdef zybo_hdmi_in_wrapper.hwdef -bitfile zybo_hdmi_in_wrapper.bit -meminfo zybo_hdmi_in_wrapper.mmi -file zybo_hdmi_in_wrapper.sysdef }
  close_msg_db -file write_bitstream.pb
} RESULT]
if {$rc} {
  step_failed write_bitstream
  return -code error $RESULT
} else {
  end_step write_bitstream
}

