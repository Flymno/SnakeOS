.section .text
.global load_new_page_directory
.type load_new_page_directory, @function

load_new_page_directory:
    mov 4(%esp), %eax
   	mov %eax, %cr3
    ret