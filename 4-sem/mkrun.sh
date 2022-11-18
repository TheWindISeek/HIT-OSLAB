#########################################################################
# File Name:    mkrun.sh
# Author:       rebelOverWaist
# Created Time: 2022年11月05日 星期六 18时20分25秒
#########################################################################
#!/bin/bash

cd ~/oslab/linux-0.11 
make clean && make all
cd ..
./run
