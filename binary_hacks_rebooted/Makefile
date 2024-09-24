.PHONY: all clean
all: hello_for_elfhack hello_for_elfhack.h hello_exec_byte
clean:
	rm -rf *.o hello_for_elfhack hello_for_elfhack.h hello_exec_byte

hello_for_elfhack.o: hello_for_elfhack.asm
	nasm -f elf64 $^

hello_for_elfhack: hello_for_elfhack.o
	ld -o $@ $^

hello_for_elfhack.h: hello_for_elfhack
	xxd -seek 0x1000 -len 0x31 -include hello_for_elfhack > $@

hello_exec_byte: hello_for_elfhack.h
	gcc -o $@ hello_exec_byte.c
