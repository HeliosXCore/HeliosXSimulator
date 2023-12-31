#pragma once
#include <stdint.h>
#include <stddef.h>

extern "C" {
    struct DifftestResult {
        int reg_id;
        uint32_t reg_val;
        int8_t wen;
        uint32_t pc;
    };
    void difftest_setregs(const void *r);
    void difftest_memcpy_from_dut(uint32_t dest, void *src, size_t n);
    void difftest_getregs(void *r);
    uint32_t difftest_getreg_by_id(int reg_id);
    void difftest_setregs(const void *r);
    void difftest_setreg_by_id(int reg_id, uint32_t value);
    void difftest_exec(uint64_t n, DifftestResult *difftest_result);
    void difftest_init();
}