#include "emulator.hpp"
#include "difftest.hpp"
#include <fmt/core.h>

using namespace heliosxsimulator;

int main() {
    fmt::println("EmulatorWrapper test......");
    EmulatorWrapper emulator;
    emulator.initialize();

    const uint32_t img[] = {
        0x00000297,  // auipc t0,0
        0x00028823,  // sb  zero,16(t0)
        0x0102c503,  // lbu a0,16(t0)
        0x00100073,  // ebreak (used as nemu_trap)
        0xdeadbeef,  // some data
    };

    emulator.copy_from_dut(0x80000000, (void *)img, sizeof(img));
    uint32_t last_commit = 0;
    uint32_t cycle = 0;
    while (true) {
        DifftestResult result;
        emulator.exec(1, &result);
        if (result.wen != 0) {
            last_commit = cycle;
            fmt::println("pc: {:x}, wen: {:x}, wreg_num: {:x}, wreg_data: {:x}",
                         result.pc, result.wen, result.reg_id, result.reg_val);
        }
        cycle++;
        if (cycle - last_commit > 500) {
            fmt::println("Commit timeout at cycle {}, last commmit: {}", cycle,
                         last_commit);
            break;
        }
    }
}