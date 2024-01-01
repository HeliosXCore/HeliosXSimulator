#include "memory.hpp"
#include "error_handler.hpp"
#include <fmt/core.h>
#include <iostream>
#include <fstream>

using namespace heliosxsimulator;

int main() {
    std::ofstream out("memory.coe", std::ios::binary);
    const uint32_t img[] = {
        0x00000297,  // auipc t0,0
        0x00028823,  // sb  zero,16(t0)
        0x0102c503,  // lbu a0,16(t0)
        0x00100073,  // ebreak (used as nemu_trap)
        0xdeadbeef,  // some data
    };
    if (!out.is_open()) {
        std::cerr << "Cannot open file" << std::endl;
        return 1;
    } else {
        out.write((const char *)img, sizeof(img) * sizeof(uint32_t));
        out.close();
    }

    Memory mem(0x80000000, 0x10000);
    // mem.load(0x80000000, (const char *)img, sizeof(img) * sizeof(uint32_t));
    mem.load("memory.coe");
    fmt::println("mem[0x80000000] = 0x{:x}", mem[0x80000000]);
    fmt::println("mem[0x80000004] = 0x{:x}", mem[0x80000004]);
    fmt::println("mem[0x80000008] = 0x{:x}", mem[0x80000008]);
    fmt::println("mem[0x8000000c] = 0x{:x}", mem[0x8000000c]);
    fmt::println("mem[0x80000010] = 0x{:x}", mem[0x80000010]);

    uint32_t ack_o, data_o;
    // Read 0x80000010
    mem.apply(1, 0, 0x80000010, 0, 0xf, ack_o, data_o);
    ASSERT(ack_o == 0, "ack_o should be 0");
    ASSERT(data_o == 0x0, "data_o should be 0x0");

    // Write 0x80000014
    mem.apply(1, 1, 0x80000014, 0x12345678, 0xf, ack_o, data_o);
    ASSERT(ack_o == 1, "ack_o should be 1");
    ASSERT(data_o == 0xdeadbeef, "data_o should be 0xdeadbeef and not 0x{:x}",
           data_o);

    // Read 0x80000014
    mem.apply(1, 0, 0x80000014, 0, 0xf, ack_o, data_o);
    ASSERT(ack_o == 1, "ack_o should be 0");
    ASSERT(data_o == 0x12345678, "data_o should be 0x12345678 and not 0x{:x}",
           data_o);
}