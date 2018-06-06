all:
	cd vivado && vivado -s rebuild.tcl -mode batch
	sdspfm -cfg config.xml -o .. -force

clean:
	rm -rf sdspfm.* .Xil
	rm -rf vivado/pynq_bare.* vivado/vivado.* vivado/.Xil
