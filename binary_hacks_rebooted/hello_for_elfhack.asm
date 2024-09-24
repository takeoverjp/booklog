section .text
  global _start
section .text
_start:
  call callee
  db 'Hello, World!', 0x0a
callee:
  mov rdx, 0xe
  pop rsi
  mov rdi, 0x1
  mov rax, 0x1
  syscall
  mov rdi, 0x0
  mov rax, 0x3c
  syscall
