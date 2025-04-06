.code
syscall_stub proc
    mov r10, rcx
    pop rcx
    pop rax
    mov [rsp], rcx
    mov rax, [rsp + 24]
    syscall
    sub rsp, 8
    jmp qword ptr [rsp + 8]
syscall_stub endp
end