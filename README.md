# Dynamic Direct Syscaller
Grabs the ID of a syscall dynamically and calls the syscall directly

## How to use:
```cpp
int main()
{
    const HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    const uintptr_t func_addr = uintptr_t(GetProcAddress(ntdll, "NtReadVirtualMemory"));
 
    uint32_t ssn = 0;
    syscall("NtReadVirtualMemory", GetCurrentProcess(), PVOID(func_addr + 4), &ssn, sizeof(uint8_t), nullptr);
 
    std:: cout << ssn << std::endl;
 
    return 0;
}
```
This is a basic example of syscalling NtReadVirtualMemory, using it to read its own SSN

## How does it work?
Its easy! To resolve the ID of a syscall you can get its address using GetProcAddress, and you must then read the 5th byte which holds the SSN. Then to call it directly we use the ASM stub
