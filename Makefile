AS=nasm
CPP_HEADERS = $(shell find . -name "*.h")
#CPP_SOURCES = $(wildcard kernel/*.cpp)
CPP_SOURCES = $(shell find . -name "*.cpp")
ASM_SOURCES = $(wildcard kernel/*.asm Drivers/*.asm)
CPP_OBJ = ${CPP_SOURCES:.cpp=.o}
ASM_OBJ = ${ASM_SOURCES:.asm=.o}

.PHONY: build clean run run_qemu b q all run

all: os_image

build: os_image

b: os_image
	bochs

q: os_image
	qemu-system-x86_64 -drive format=raw,file=$<

kernel.bin: ${CPP_OBJ} ${ASM_OBJ}
	ld -nostartfiles -m elf_i386 -o $@ -Ttext 0x9000 --oformat binary kernel/KernelEntry.o $^

os_image: boot_sector.bin kernel.bin
	cat $^ > $@
	dd if=/dev/zero of=$@ bs=1 count=1 seek=1MB


boot_sector.bin: boot/boot_sector.asm
	$(AS) $< -f bin -I boot/ -o $@

%.o: %.cpp ${CPP_HEADERS}
	g++ -ffreestanding -I./stdlib/ -fno-exceptions -g -fno-rtti -fno-asynchronous-unwind-tables -m32 -march=i386 -fno-pie -c $< -o $@ -fcf-protection=none

%.o: %.asm
	nasm $< -f elf -o $@

clean:
	find . -name "*.o" -delete
	find . -name "*.bin" -delete
	find . -name os_image -delete
