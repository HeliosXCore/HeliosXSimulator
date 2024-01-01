#include "memory.hpp"
#include <fmt/core.h>

using namespace heliosxsimulator;

int main() {
    Memory mem(0x80000000, 0x10000);
    const uint32_t img[] = {
        0x00000297,  // auipc t0,0
        0x00028823,  // sb  zero,16(t0)
        0x0102c503,  // lbu a0,16(t0)
        0x00100073,  // ebreak (used as nemu_trap)
        0xdeadbeef,  // some data
    };
    mem.load(0x80000000, (const char *)img, sizeof(img) * sizeof(uint32_t));
    fmt::println("mem[0x80000000] = 0x{:x}", mem[0x80000000]);
    fmt::println("mem[0x80000004] = 0x{:x}", mem[0x80000004]);
    fmt::println("mem[0x80000008] = 0x{:x}", mem[0x80000008]);
    fmt::println("mem[0x8000000c] = 0x{:x}", mem[0x8000000c]);
    fmt::println("mem[0x80000010] = 0x{:x}", mem[0x80000010]);
}