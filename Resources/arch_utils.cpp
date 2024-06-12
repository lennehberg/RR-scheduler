//
// Created by lennehberg on 6/12/24.
//
#include "arch_utils.h"
#ifdef __x86_64__
address_t translate_address(address_t addr)
{
address_t ret;
asm volatile("xor    %%fs:0x30,%0\n"
             "rol    $0x11,%0\n"
             : "=g" (ret)
             : "0" (addr));
return ret;
}
#else
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%gs:0x18,%0\n"
                 "rol    $0x9,%0\n"
    : "=g" (ret)
    : "0" (addr));
    return ret;
}
#endif