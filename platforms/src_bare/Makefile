all:
	cd vivado && vivado -s rebuild.tcl -mode batch
	xsct -sdx pynq_bare.tcl

clean:
	rm -rf sdspfm.* .Xil
	rm -rf vivado/pynq_bare.* vivado/vivado.* vivado/.Xil
	rm -rf vivado/*.log vivado/*.jou
	rm -rf vivado/awsver.txt  vivado/NA
