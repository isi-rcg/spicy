#remove any old files from previous run
exec rm -rf component.xml
exec rm -rf xgui

###################################################
#create ip
###########
#create project
create_project project_1 . -part xc7z020clg484-1
set_property board_part xilinx.com:zc702:part0:1.2 [current_project]

#add hdl files
add_files ./hdl

#package the project as an IP
ipx::package_project -root_dir ./ -vendor isi.edu -library user -taxonomy /UserIP

#configure info settings for IP
set_property library ip [ipx::current_core]
set_property name XIOBUF [ipx::current_core]
set_property version 1.0 [ipx::current_core]
set_property display_name XIOBUF [ipx::current_core]
set_property description "Xilinx I/O Buffer Primitive" [ipx::current_core]
set_property vendor_display_name {ISI} [ipx::current_core]
set_property company_url http://www.isi.edu [ipx::current_core]
set_property taxonomy /BaseIP [ipx::current_core]
set_property supported_families {} [ipx::current_core]
set_property supported_families {virtex7 Beta qvirtex7 Beta kintex7 Beta kintex7l Beta qkintex7 Beta qkintex7l Beta artix7 Beta artix7l Beta aartix7 Beta qartix7 Beta zynq Beta qzynq Beta azynq Beta spartan7 Beta virtexu Beta virtexuplus Beta kintexuplus Beta zynquplus Beta kintexu Beta} [ipx::current_core]

#other misc configuration
set_property core_revision 1 [ipx::current_core]
ipx::create_xgui_files [ipx::current_core]
ipx::update_checksums [ipx::current_core]
ipx::save_core [ipx::current_core]
