#pragma once
#include <verilated.h>
#include <verilated_vcd_c.h>
#include <memory>
#include <fstream>
#include <fmt/core.h>

namespace heliosxsimulator {
    template <class Dut>
    class SocSimulator {
       public:
        std::shared_ptr<Dut> cpu_top;

        virtual void connect_wire();
        virtual void initialize_dut();
        virtual bool trace_on();
        virtual void trace();
        virtual void run();

       protected:
        uint64_t cpu_clk;
        uint64_t cpu_reset;
        uint32_t[4] cpu_inst_i;
        uint32_t read_dmem_data_i;

        uint32_t pc_o;
        bool dmem_we_o;
        uint32_t write_dmem_data_o;
        uint32_t dmem_addr_o;

        // trace info
        uint64_t debug_wen;
        uint64_t debug_wreg_num;
        uint32_t debug_wreg_data;

        uint64_t sime_time;
        bool running;

        std::ifstream trace_file;
    };
};  // namespace heliosxsimulator