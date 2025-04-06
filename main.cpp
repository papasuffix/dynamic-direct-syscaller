#include <iostream>
#include <type_traits>
#include <tuple>

#include <Windows.h>

extern "C" void* syscall_stub();

template<std::size_t n, typename tuple, std::size_t... c>
auto tuple_tail_impl(tuple&& tup, std::index_sequence<c...>)
{
    return std::make_tuple(std::get<n + c>(std::forward<tuple>(tup))...);
}

template <std::size_t n, typename... arguments>
auto drop_first_n(arguments&&... args)
{
    constexpr std::size_t size = sizeof...(args);
    static_assert(n <= size, "can't drop more arguments than exist");
    return tuple_tail_impl<n>(std::make_tuple(std::forward<arguments>(args)...), std::make_index_sequence<size - n>{});
}

template <typename... arguments>
void* call_syscall_stub(arguments... args)
{
    auto func = (void*(*)(arguments...))(&syscall_stub);
    return func(args...);
}

template <typename... arguments>
NTSTATUS do_syscall(uint32_t idx, arguments... args)
{
    NTSTATUS status = 0;

    void* arg1 = nullptr;
    void* arg2 = nullptr;
    void* arg3 = nullptr;
    void* arg4 = nullptr;

    if constexpr (sizeof...(args) >= 1) arg1 = (void*)std::get<0>(std::tuple<arguments...>(args...));
    if constexpr (sizeof...(args) >= 2) arg2 = (void*)std::get<1>(std::tuple<arguments...>(args...));
    if constexpr (sizeof...(args) >= 3) arg3 = (void*)std::get<2>(std::tuple<arguments...>(args...));
    if constexpr (sizeof...(args) >= 4) arg4 = (void*)std::get<3>(std::tuple<arguments...>(args...));

    if constexpr (sizeof...(args) > 4)
    {
        auto args_tail = drop_first_n<4>(std::forward<arguments>(args)...);
        std::apply([arg1, arg2, arg3, arg4, idx, &status](auto&&... args)
        {
            status = (NTSTATUS)call_syscall_stub(arg1, arg2, arg3, arg4, idx, nullptr, args...);
        }, args_tail);
    }
    else
    {
        status = (NTSTATUS)call_syscall_stub(arg1, arg2, arg3, arg4, idx, nullptr);
    }

    return status;
}

uint32_t get_syscall_id(const char* name)
{
    static HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    const uintptr_t func_addr = uintptr_t(GetProcAddress(ntdll, name));

    const uint32_t ssn = *(uint8_t*)(func_addr + 4);

    return ssn;
}

template <typename... arguments>
NTSTATUS syscall(const char* name, arguments... args)
{
    const uint32_t ssn = get_syscall_id(name);
    return do_syscall(ssn, args...);
}

int main()
{
    const HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    const uintptr_t func_addr = uintptr_t(GetProcAddress(ntdll, "NtReadVirtualMemory"));

    uint32_t ssn = 0;
    syscall("NtReadVirtualMemory", GetCurrentProcess(), PVOID(func_addr + 4), &ssn, sizeof(uint8_t), nullptr);

    std:: cout << ssn << std::endl;

    return 0;
}