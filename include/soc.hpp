#pragma once
#include "emulator.hpp"
#include <verilated.h>
#include <verilated_vcd_c.h>
#include <memory>
#include <fstream>
#include <fmt/core.h>

#define COMMIT_TIMEOUT 5000

namespace heliosxsimulator {
    template <class Dut>
    class SocSimulator {
       public:
        std::shared_ptr<Dut> cpu_top;
        std::shared_ptr<EmulatorWrapper> emulator;

        virtual void connect_wire();
        virtual void initialize_dut();
        virtual bool trace_on();
        virtual void trace();
        virtual void setup();
        virtual void run(std::string trace_file);
        void detect_commit_timeout();

       protected:
        uint64_t clock;
        uint64_t cpu_clk;
        uint64_t cpu_reset;
        uint32_t cpu_inst_i[4];
        uint32_t read_dmem_data_i;

        uint32_t pc_o;
        bool dmem_we_o;
        uint32_t write_dmem_data_o;
        uint32_t dmem_addr_o;

        // trace info
        uint64_t debug_wen;
        uint64_t debug_wreg_num;
        uint32_t debug_wreg_data;

        uint64_t sim_time;
        uint64_t last_commit;
        bool running;

        std::ifstream trace_file;

        void tick();
    };
};  // namespace heliosxsimulator