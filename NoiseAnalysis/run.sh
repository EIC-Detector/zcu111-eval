#!/bin/sh
filename=""

if [ "$1" != "" ]; then
	filename=$1
	echo 'Opening file: '$filename'.lvm'
	#`./read_data $filename b wp`
	if ./read_data $filename b wp; then
		./read_pedestals $filename b
	else
		echo First script failed!
	fi
else
	echo 'Specify the input LVM file without extension.'
	echo 'For example (data.lvm): '$0 data
fi
