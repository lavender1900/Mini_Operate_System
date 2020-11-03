#!/bin/bash
if [ $# == 0 ]
then
	echo "Missing Parameter"
	exit -1
fi

sudo mount os.img /mnt/floppy
sudo cp $1 /mnt/floppy
sudo umount /mnt/floppy
