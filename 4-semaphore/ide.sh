#########################################################################
# File Name:    ide.sh
# Author:       rebelOverWaist
# Created Time: 2022年11月17日 星期四 16时24分57秒
#########################################################################
#!/bin/bash
echo "Please enter the file name you want to compile and run'"
read fileName
echo "Please check "
echo ${fileName%.*} 
echo "is file name you want. enter y"
read check 
if  [ "$check" =  "y" ]; then 
   gcc $fileName -o ${fileName%.*} -lpthread
   echo "now you get " ${fileName%.*}
   ./${fileName%.*}
fi 
