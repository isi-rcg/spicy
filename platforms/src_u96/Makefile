all:
	cd vivado && vivado -s rebuild.tcl -mode batch
	xsct -sdx pynq_u96.tcl
	mv ../pynq_u96/export/pynq_u96/* ../pynq_u96/
	rm -rf ../pynq_u96/logs  ../pynq_u96/_platform  ../pynq_u96/platform.spr ../pynq_u96/export ../pynq_u96/.project

clean:
	rm -rf sdspfm.* .Xil
	rm -rf vivado/pynq_u96.* vivado/vivado.* vivado/.Xil
	rm -rf vivado/*.log vivado/*.jou
	rm -rf vivado/awsver.txt  vivado/NA

ultraclean: clean
	rm -rf ../pynq_u96
