#pragma once
#include "difftest.hpp"

namespace heliosxsimulator {
    class EmulatorWrapper {
       public:
        void initialize_regs();
        void initialize_memory();
        void initialize();
        void exec(int n, DifftestResult *result);
        void copy_from_dut(uint32_t dest, void *src, size_t n);
    };
}  // namespace heliosxsimulator
