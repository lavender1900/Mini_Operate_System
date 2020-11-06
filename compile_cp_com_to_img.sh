#!/bin/bash
if [ $# == 0 ]
then
	echo "Missing Parameter"
	exit -1
fi

nasm $1 -o $2
sudo mount os.img /mnt/floppy
sudo cp $2 /mnt/floppy
sudo umount /mnt/floppy
