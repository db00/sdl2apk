.data  
msg:  
    .ascii "Hello world, hello AT&T asm!\n"  
    len = . - msg  
  
.text  
.global _start  
  
_start:  
    movl    $len,   %edx    # 显示的字符数  
    movl    $msg,   %ecx    # 缓冲区指针  
    movl    $1, %ebx    # 文件描述符  
    movl    $4, %eax    # 系统调用号，_write  
    int $0x80       # 系统调用  
  
    movl    $0, %ebx    # 传给_exit的参数  
    movl    $1, %eax    # 系统调用号，_exit  
    int $0x80       # 系统调用
#as -o t.o t.s
#ld -o test t.o
