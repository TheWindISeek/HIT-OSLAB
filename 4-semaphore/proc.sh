#########################################################################
# File Name:    proc.sh
# Author:       程序员Carl
# mail:         programmercarl@163.com
# Created Time: 2022年11月15日 星期二 14时50分47秒
#########################################################################
#!/bin/bash
# copy of hdc/usr/root/proc.sh
echo "ls -l /proc/*info"
ls -l /proc/*info

echo "cat /proc/psinfo"
cat /proc/psinfo

echo "cat /proc/hdinfo"
cat /proc/hdinfo

echo "cat /proc/meminfo"
cat /proc/meminfo 

