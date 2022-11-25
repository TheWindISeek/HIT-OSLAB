#########################################################################
# File Name:    syscall.sh
# Author:       rebelOverWaist
#########################################################################
#!/bin/bash
make clean && make all
cd ..
sudo ./mount-hdc
cd linux-0.11
cp iam.c whoami.c ../hdc/usr/root
cp testlab2.c testlab2.sh ../hdc/usr/root
cd ..
sudo umount hdc
./run
