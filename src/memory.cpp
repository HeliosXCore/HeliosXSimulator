#include "memory.hpp"

namespace heliosxsimulator {
    Memory::Memory(uint32_t base_addr, uint32_t size)
        : base_addr(base_addr), size(size) {
        mem = std::make_unique<char>(size);
    }

    void Memory::load(std::string filename) {}

    void Memory::load(uint32_t addr, const char* buf, size_t n) {
        for (int i = 0; i < n; i++) {
            auto ptr = mem.get();
            ptr[addr + i - base_addr] = buf[i];
        }
    }

    uint32_t& Memory::operator[](const uint32_t addr) {
        return *(uint32_t*)(mem.get() + addr - base_addr);
    }
}  // namespace heliosxsimulator