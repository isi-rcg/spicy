# Hot & Spicy
Python tools for FPGAs

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