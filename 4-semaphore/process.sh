#########################################################################
# File Name:    process.sh
# Author:       rebelOverWaist
# Created Time: 2022年11月14日 星期一 12时29分39秒
#########################################################################
#!/bin/bash

cd ..
sudo ./mount-hdc
cd linux-0.11
# sudo cp ../process.c ../hdc/usr/root/share/
sudo cp ../hdc/var/process.log ../process.log 
sudo ../files/stat_log.py ../process.log
sudo ../files/stat_log.py ../process.log 0 1 2 3 4

# a indexed_array A assoc_array
# declare -a indexed_array
# $i = 0

for line in $(sudo cat ../hdc/usr/root/process.txt)
do
   
    echo $line
  #  a = $line
 #   array[$c] = $a
#    c = $[$c+1]
    #  indexed`_array[$i] = $line 
   # i = $[$i+1]
done
# file = ../hdc/usr/root/process.txt 
# array = ($(cat $file))
# read -ra array <<< "$element"
echo "input process id"
read  array
echo "get ${#array[@]} values in array"
# sudo ../files/stat_log.py ../process.log ${array[@]}
sudo umount ../hdc 
