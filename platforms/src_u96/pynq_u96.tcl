
set name "pynq_u96"

platform -name $name -desc "Pynq Ultra96 Board" -hw ./vivado/pynq_u96.dsa -out .. -prebuilt

system -name ubuntu -display-name "Ubuntu"  -boot ./boot  -readme ./generic.readme
domain -name ubuntu -proc psu_cortexa53_0 -os linux -image ./sw/ubuntu/image
boot -bif ./sw/ubuntu/ubuntu.bif

platform -generate
