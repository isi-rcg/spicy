all:
	cd vivado && vivado -s rebuild.tcl -mode batch
	sdspfm -cfg config.xml -o .. -force

clean:
	rm -rf sdspfm.* .Xil
	rm -rf vivado/pynq_hdmi.* vivado/vivado.* vivado/.Xil
