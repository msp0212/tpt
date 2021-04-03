#!/bin/sh
#Create cscope db for the project in the current directory
echo "Current working dir : $PWD";
echo "cscope db will be created recursively for this dir. continue? (y/n [n]) : ";
read ch;
if [ $ch = "y" ]; then
	find $PWD -name '*.c' -o -name  '*.h' > $PWD/cscope.files;
	cscope -b -q -k;
	rm -f ./cscope.files;
fi
echo "\nPlease make sure that CSCOPE_DB environment variable is set to $PWD/cscope.out !!!";
