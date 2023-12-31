#include "emulator.hpp"
#include <random>

namespace heliosxsimulator {

    void EmulatorWrapper::initialize_regs() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint32_t> dis(0, 1000);
        uint32_t regs[32];
        for (int i = 0; i < 32; i++) {
            regs[i] = dis(gen);
        }
        difftest_setregs(regs);
    }

    void EmulatorWrapper::initialize_memory() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint32_t> dis(0, 1000);
        uint32_t mem[0x10000];
        for (int i = 0; i < 0x10000; i++) {
            mem[i] = dis(gen);
        }
        difftest_memcpy_from_dut(0x80000000, mem, 0x10000);
    }

    void EmulatorWrapper::initialize() {
        difftest_init();
        // initialize_regs();
        // initialize_memory();
    }

    void EmulatorWrapper::exec(int n, DifftestResult *result) {
        difftest_exec(n, result);
    }

    void EmulatorWrapper::copy_from_dut(uint32_t dest, void *src, size_t n) {
        difftest_memcpy_from_dut(dest, src, n);
    }

}  // namespace heliosxsimulator