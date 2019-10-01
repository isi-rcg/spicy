# Hot & Spicy: Python tools for FPGAs

Hot & Spicy is an open-source infrastructure and tool suite for integrating FPGA accelerators in Python applications, provided entirely as Python source code. This suite of tools eases the packaging, integration, and binding of accelerators and their C/C++ based drivers callable from a Python application. The Hot & Spicy tools can: 
1. translate Python functions to HLS-suitable C functions, 
2. generate Python C wrapper bindings, 
3. automate the FPGA EDA tool flow, and 
4. retarget Python source code to use accelerated libraries
For FPGA experts, this enables increased productivity and supports research on each stage of the flow by providing a framework to integrate additional compilers and optimizations. For everyone else this enables fast, consistent, acceleration of applications on FPGAs. 

# Licensing
Currently this project is licensed using a custom MIT-like license, see 'license.txt' for specifics. We are working towards a GPL release in the future. 

If you'd like another license, please contact us at: skalicky@isi.edu. 

# Publication

If you use any of the tools in your work, we would love to hear about it and would very much appreciate a citation:

- Sam Skalicky, Joshua Monson, Andrew Schmidt, and Matthew French: Hot & Spicy: Improving Productivity with Python and HLS for FPGAs, 26th IEEE International Symposium on Field-Programmable Custom Computing Machines (FCCM 2018), April 2018.

```
@inproceedings{Skalicky:2018:FCCM:Spicy,
title={Hot \& Spicy: Improving Productivity with Python and HLS for FPGAs},
author={Skalicky, Sam and Monson, Joshua and Schmidt, Andrew and French, Matthew},
booktitle={International Symposium on Field-Programmable Custom Computing Machines},
month={Apr},
year={2018},
}
```

A copy of the paper and presentation slides are available here: https://samskalicky.wordpress.com/2018/04/30/paper-accepted-python-hls-sdsoc/

# EDA Tools & Platforms

Hot & Spicy currently targets SDSoC 2018.3. There is a previous release in the 2017.4 branch. In the paper, the results were produced using SDSoC 2017.2. There are quite a few improvements and differences between 2017.2 and 2017.4 but are not backwards compatible.

Hot & Spicy is built on top of the PYNQ platform version 2.4 (2018.3 Xilinx tools) and 2.1 (2017.4 Xilinx tools). Here are the default platforms available:

1. pynq_bare - this platform is an empty platform without any I/O
2. pynq_hdmi - this platform only includes the PYNQ HDMI in/out path
