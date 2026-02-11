x86_64_asm_source_files := $(shell find boot/ -name *.asm)
x86_64_asm_object_files := $(patsubst boot/%.asm, build/boot/%.o, $(x86_64_asm_source_files))

init_source_files := $(shell find init -name *.c)
init_object_files := $(patsubst init/%.c, build/init/%.o, $(init_source_files))

kernel_source_files := $(shell find kernel -name *.c)
kernel_object_files := $(patsubst kernel/%.c, build/kernel/%.o, $(kernel_source_files))

asm_kernel_source_files := $(shell find kernel -name *.asm)
asm_kernel_object_files := $(patsubst kernel/%.asm, build/kernel/%_asm_.o, $(asm_kernel_source_files))

lib_source_files := $(shell find lib -name *.c)
lib_object_files := $(patsubst lib/%.c, build/lib/%.o, $(lib_source_files))

$(asm_kernel_object_files): build/kernel/%_asm_.o : kernel/%.asm
	mkdir -p $(dir $@)
	nasm -f elf64 $< -o $@

$(lib_object_files): build/lib/%.o : lib/%.c
	mkdir -p $(dir $@) && \
	x86_64-elf-gcc -c -I include -mcmodel=kernel -mno-red-zone -ffreestanding $< -o $@

$(kernel_object_files): build/kernel/%.o : kernel/%.c
	mkdir -p $(dir $@) && \
	x86_64-elf-gcc -c -I include -mcmodel=kernel -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mgeneral-regs-only -ffreestanding $< -o $@

$(init_object_files): build/init/%.o : init/%.c
	mkdir -p $(dir $@) && \
	x86_64-elf-gcc -c -I include -mcmodel=kernel -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mgeneral-regs-only -ffreestanding $< -o $@

$(x86_64_asm_object_files): build/boot/%.o : boot/%.asm
	mkdir -p $(dir $@)
	nasm -f elf64 $< -o $@

.PHONY: build-x86_64
build-x86_64: $(x86_64_asm_object_files) $(asm_kernel_object_files) $(init_object_files) $(kernel_object_files) $(lib_object_files)
	mkdir -p dist/
	x86_64-elf-ld -n -o dist/kernel.bin -T targets/linker.ld $(x86_64_asm_object_files) $(asm_kernel_object_files) $(init_object_files) $(kernel_object_files) $(lib_object_files) files/*.o
	cp dist/kernel.bin targets/iso/boot/kernel.bin
	grub-mkrescue /usr/lib/grub/i386-pc -o dist/kernel.iso targets/iso

.PHONY: clean
clean:
	rm -rf build clean