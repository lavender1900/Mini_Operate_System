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
OBJS		= kernel/kernel.o kernel/start.o kernel/global.o kernel/protect.o kernel/i8259A.o kernel/interrupts.o kernel/main.o kernel/process.o kernel/syscall.o kernel/clock.o kernel/system_calls.o kernel/keyboard.o kernel/tty.o kernel/console.o kernel/ipc.o kernel/api.o kernel/systask.o lib/time.o lib/string.o lib/printf.o lib/vsprintf.o lib/kliba.o lib/klib.o lib/memory.o lib/page.o
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

kernel/start.o : kernel/start.c include/type.h include/protect.h include/io.h include/global.h include/asm_lib.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/global.o : kernel/global.c include/type.h include/const.h include/process.h include/system_calls.h include/global.h include/tty.h include/console.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/protect.o : kernel/protect.c include/protect.h include/i386macros.h include/global.h include/interrupts.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/i8259A.o : kernel/i8259A.c include/8259A.h include/asm_lib.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/main.o : kernel/main.c include/type.h include/global.h include/protect.h include/process.h include/i386macros.h include/asm_lib.h include/clock.h include/keyboard.h include/tty.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/process.o : kernel/process.c include/io.h include/time.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/clock.o : kernel/clock.c include/clock.h include/global.h include/asm_lib.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/interrupts.o : kernel/interrupts.c include/type.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/system_calls.o : kernel/system_calls.c include/process.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/keyboard.o : kernel/keyboard.c include/keyboard.h include/keymap.h include/tty.h include/asm_lib.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/tty.o : kernel/tty.c include/const.h include/type.h include/keyboard.h include/global.h include/tty.h include/console.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/console.o : kernel/console.c include/const.h include/type.h include/proto.h include/console.h include/global.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/syscall.o : kernel/syscall.asm
	$(ASM) $(ASMKERNELFLAGS) -o $@ $<

kernel/ipc.o : kernel/ipc.c include/type.h include/const.h include/io.h include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/api.o : kernel/api.c include/type.h include/const.h include/api.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/systask.o : kernel/systask.c include/type.h include/const.h include/ipc.h include/api.h
	$(CC) $(CFLAGS) -o $@ $<

lib/printf.o : lib/printf.c include/const.h include/proto.h include/type.h
	$(CC) $(CFLAGS) -o $@ $<

lib/vsprintf.o : lib/vsprintf.c include/type.h include/string.h
	$(CC) $(CFLAGS) -o $@ $<

lib/time.o : lib/time.c include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

lib/string.o : lib/string.c
	$(CC) $(CFLAGS) -o $@ $<

lib/page.o : lib/page.c include/type.h include/global.h include/protect.h
	$(CC) $(CFLAGS) -o $@ $<

lib/klib.o : lib/klib.c include/string.h include/io.h
	$(CC) $(CFLAGS) -o $@ $<

lib/kliba.o : lib/kliba.asm
	$(ASM) $(ASMKERNELFLAGS) -o $@ $<

lib/memory.o : lib/memory.asm
	$(ASM) $(ASMKERNELFLAGS) -o $@ $<
