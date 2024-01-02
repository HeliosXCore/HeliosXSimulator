#pragma once
#include <stdint.h>
#include <string>
#include <memory>

namespace heliosxsimulator {
    union Instruction {
        uint32_t instructions[4];

        uint8_t padding[128];
    };

    class Memory {
       public:
        Memory(uint32_t base_addr, uint32_t size);
        void load(std::string filename);
        void load(uint32_t addr, const char *buf, size_t n);
        void apply(uint32_t wb_cycle, uint32_t wb_we, uint32_t wb_addr,
                   uint32_t wb_data, uint8_t wb_sel, uint32_t &ack_o,
                   uint32_t &data_o);
        void fetch(uint32_t cycle, uint32_t pc, Instruction &inst_o,
                   uint32_t &inst_valid_o);
        uint32_t &operator[](const uint32_t addr);

        std::unique_ptr<char> mem;
        uint32_t base_addr;
        uint32_t size;
        int next_ack;
        uint32_t next_data;
        Instruction next_inst;
    };
}  // namespace heliosxsimulator