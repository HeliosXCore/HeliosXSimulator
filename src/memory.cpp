#include "memory.hpp"

namespace heliosxsimulator {
    Memory::Memory(uint32_t base_addr, uint32_t size)
        : base_addr(base_addr), size(size) {
        mem = std::make_unique<char>(size);
        next_ack = 0;
        next_data = 0;
    }

    void Memory::load(std::string filename) {}

    void Memory::load(uint32_t addr, const char* buf, size_t n) {
        for (int i = 0; i < n; i++) {
            auto ptr = mem.get();
            ptr[addr + i - base_addr] = buf[i];
        }
    }

    void Memory::apply(uint32_t wb_cycle, uint32_t wb_we, uint32_t wb_addr,
                       uint32_t wb_data, uint8_t wb_sel, uint32_t& ack_o,
                       uint32_t& data_o) {
        uint32_t sel = 0;
        if (wb_sel & 0x8) {
            sel |= 0xff000000;
        }
        if (wb_sel & 0x4) {
            sel |= 0x00ff0000;
        }
        if (wb_sel & 0x2) {
            sel |= 0x0000ff00;
        }
        if (wb_sel & 0x1) {
            sel |= 0x000000ff;
        }

        ack_o = next_ack && wb_cycle;
        data_o = next_data;
        next_data = wb_data;
        next_ack = 0;

        if (wb_cycle) {
            if (wb_we) {
                if (sel == 0xffffffffu) {
                    *(uint32_t*)(mem.get() + wb_addr - base_addr) = wb_data;
                } else {
                    uint32_t old_data =
                        *(uint32_t*)(mem.get() + wb_addr - base_addr);
                    uint32_t new_data = (old_data & ~sel) | (wb_data & sel);
                    *(uint32_t*)(mem.get() + wb_addr - base_addr) = new_data;
                }
            }

            next_ack = 1;
            next_data = *(uint32_t*)(mem.get() + wb_addr - base_addr);
        }
    }

    uint32_t& Memory::operator[](const uint32_t addr) {
        return *(uint32_t*)(mem.get() + addr - base_addr);
    }
}  // namespace heliosxsimulator