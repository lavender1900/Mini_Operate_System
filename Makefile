# Makefile for Mini OS

# Kernel entry point
ENTRYPOINT	= 0x30400
ENTRYOFFSET	= 0x400 

# Compile paramters
ASM		 = nasm
DASM		= ndisasm
CC		= gcc
LD		= ld
# Find *.inc files in include dir
ASMBOOTFLAGS	= -I boot/include/
ASMKERNELFLAGS	= -I include/ -f elf
CFLAGS		= -I include/ -c -m32
LDFLAGS		= -s -m elf_i386 -Ttext $(ENTRYPOINT)
DASMFLAGS	= -u -o $(ENTRYPOINT) -e $(ENTRYOFFSET)

BOOT		= boot/boot.bin boot/loader.bin
KERNEL		= kernel.bin
OBJS		= kernel/kernel.o kernel/start.o kernel/global.o kernel/protect.o kernel/i8259A.o kernel/main.o kernel/process.o kernel/syscall.o kernel/clock.o lib/kliba.o lib/klib.o lib/mem.o
DASMOUTPUT	= kernel.bin.debug.asm

# actions
.PHONY : everything final image clean realclean disasm all buildimg 

everything : $(BOOT) $(KERNEL)

all : realclean everything

final : all clean

image : final buildimg

clean :
	rm -f $(OBJS)

realclean :
	rm -f $(OBJS) $(BOOT) $(KERNEL)

disasm :
	$(DASM) $(DASMFLAGS) $(KERNEL) > $(DASMOUTPUT)

buildimg :
	dd if=boot/boot.bin of=os.img bs=512 count=1 conv=notrunc
	sudo mount -t vfat os.img /mnt/floppy
	sudo cp -fv boot/loader.bin /mnt/floppy
	sudo cp -fv kernel.bin /mnt/floppy
	sudo umount /mnt/floppy

boot/boot.bin : boot/boot.asm boot/include/fat12header.inc boot/include/fat12read.inc
	$(ASM) $(ASMBOOTFLAGS) -o $@ $<

boot/loader.bin : boot/loader.asm  boot/include/macros.inc boot/include/display.inc boot/include/memory.inc \
			 boot/include/fat12header.inc boot/include/fat12read.inc
	$(ASM) $(ASMBOOTFLAGS) -o $@ $<

$(KERNEL) : $(OBJS)
	$(LD) $(LDFLAGS) -o $(KERNEL) $(OBJS)

kernel/kernel.o : kernel/kernel.asm
	$(ASM) $(ASMKERNELFLAGS) -o $@ $<

kernel/start.o : kernel/start.c include/type.h include/const.h include/protect.h include/proto.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/global.o : kernel/global.c include/type.h include/const.h include/protect.h include/proto.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/protect.o : kernel/protect.c include/type.h include/const.h include/protect.h include/proto.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/i8259A.o : kernel/i8259A.c include/type.h include/const.h include/protect.h include/proto.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/main.o : kernel/main.c include/type.h include/const.h include/protect.h include/proto.h include/global.h include/process.h include/i386macros.h	
	$(CC) $(CFLAGS) -o $@ $<

kernel/process.o : kernel/process.c include/type.h include/const.h include/global.h include/process.h include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/clock.o : kernel/clock.c include/type.h include/const.h include/global.h include/process.h include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/syscall.o : kernel/syscall.asm
	$(ASM) $(ASMKERNELFLAGS) -o $@ $<

lib/klib.o : lib/klib.c include/type.h include/const.h include/protect.h include/proto.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<

lib/kliba.o : lib/kliba.asm
	$(ASM) $(ASMKERNELFLAGS) -o $@ $<

lib/mem.o : lib/mem.asm
	$(ASM) $(ASMKERNELFLAGS) -o $@ $<
