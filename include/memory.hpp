#pragma once
#include <stdint.h>
#include <string>
#include <memory>

namespace heliosxsimulator {
    class Memory {
       public:
        Memory(uint32_t base_addr, uint32_t size);
        void load(std::string filename);
        void load(uint32_t addr, const char *buf, size_t n);
        uint32_t &operator[](const uint32_t addr);

        std::unique_ptr<char> mem;
        uint32_t base_addr;
        uint32_t size;
    };
}  // namespace heliosxsimulator