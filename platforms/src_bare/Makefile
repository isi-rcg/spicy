all:
	cd vivado && vivado -s rebuild.tcl -mode batch
	xsct -sdx pynq_bare.tcl
	mv ../pynq_bare/export/pynq_bare/* ../pynq_bare/
	rm -rf ../pynq_bare/logs  ../pynq_bare/_platform  ../pynq_bare/platform.spr ../pynq_bare/export ../pynq_bare/.project

clean:
	rm -rf sdspfm.* .Xil
	rm -rf vivado/pynq_bare.* vivado/vivado.* vivado/.Xil
	rm -rf vivado/*.log vivado/*.jou
	rm -rf vivado/awsver.txt  vivado/NA

ultraclean: clean
	rm -rf ../pynq_bare
