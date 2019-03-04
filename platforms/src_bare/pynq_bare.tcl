
set name "pynq_bare"

platform -name $name -desc "Pynq Z1 Board" -hw ./vivado/pynq_bare.dsa -out .. -prebuilt

system -name linux -display-name "Linux"  -boot ./boot  -readme ./generic.readme
domain -name linux -proc ps7_cortexa9_0 -os linux -image ./sw/ubuntu/image
boot -bif ./sw/ubuntu/ubuntu.bif

platform -generate
