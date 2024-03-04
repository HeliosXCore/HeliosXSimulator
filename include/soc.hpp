#pragma once
#include "emulator.hpp"
#include "memory.hpp"
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
        std::unique_ptr<Memory> imem;
        std::unique_ptr<Memory> dmem;

        SocSimulator(std::shared_ptr<Dut> cpu_top,
                     std::shared_ptr<EmulatorWrapper> emulator,
                     std::unique_ptr<Memory> imem, std::unique_ptr<Memory> dmem,
                     uint64_t clock, uint64_t start_time)
            : cpu_top(cpu_top),
              emulator(emulator),
              imem(std::move(imem)),
              dmem(std::move(dmem)),
              clock(clock),
              start_time(start_time) {
            cpu_clk = 0;
            cpu_reset = 1;
            cpu_inst_i = 0;
            read_dmem_data_i = 0;
            running = false;
            last_commit = 0;
            last_pc = 0x0;
        }

        void detect_commit_timeout() {
            if (debug_wen) {
                last_commit = sim_time;
            } else if (sim_time - last_commit > COMMIT_TIMEOUT) {
                fmt::println("Commit timeout at time {}", sim_time);
                running = false;
            }
        }

        void reset_dut() {
            if (sim_time >= 0 && sim_time < start_time) {
                initialize_dut();
            }
        }

        virtual void input() {
            Instruction inst_o;
            uint32_t inst_value_o;

            uint64_t start_input_time = start_time - 2 * clock;
            if (sim_time >= start_input_time) {
                cpu_reset = 0;
            }

            if (sim_time >= start_input_time && sim_time % (2 * clock) == 0) {
                imem->fetch(1, cpu_top->iaddr_o, inst_o, inst_value_o);
                cpu_inst_i = inst_o.instructions[0];
            }
        }

        virtual void connect_wire() {
            cpu_top->clk_i = cpu_clk;
            cpu_top->reset_i = cpu_reset;
            cpu_top->idata_i = cpu_inst_i;
            cpu_top->dmem_rdata_i = read_dmem_data_i;

            dmem_we_o = cpu_top->dmem_we_o;
            write_dmem_data_o = cpu_top->dmem_wdata_o;
            dmem_addr_o = cpu_top->dmem_waddr_o;

            debug_pc_o = cpu_top->debug_pc_o;
            debug_wen = cpu_top->debug_reg_wen_o;
            debug_wreg_data = cpu_top->debug_reg_wdata_o;
            debug_wreg_num = cpu_top->debug_reg_id_o;
        }

        virtual void initialize_dut() {
            cpu_reset = 1;
            cpu_inst_i = 0;
            read_dmem_data_i = 0;
        }

        virtual bool trace_on() { return true; }

        virtual void trace() {
            uint32_t ref_pc;
            uint32_t ref_wen;
            uint32_t ref_wreg_num;
            uint32_t ref_wreg_data;

            if (trace_on() && (debug_wen || debug_pc_o != last_pc)) {
                DifftestResult result;
                emulator->exec(1, &result);
                ref_pc = result.pc;
                ref_wen = result.wen;
                ref_wreg_num = result.reg_id;
                ref_wreg_data = result.reg_val;
                if (ref_pc != debug_pc_o || ref_wen != debug_wen ||
                    ref_wreg_num != debug_wreg_num ||
                    ref_wreg_data != debug_wreg_data) {
                    fmt::println("Trace failed at time {}", sim_time);
                    fmt::println(
                        "Expected: pc: {:#x}, wen: {:#x}, wreg_num: {:#x}, "
                        "wreg_data: "
                        "{:#x}",
                        ref_pc, ref_wen, ref_wreg_num, ref_wreg_data);
                    fmt::println(
                        "Actual: pc: {:#x}, wen: {:#x}, wreg_num: {:#x}, "
                        "wreg_data: "
                        "{:#x}",
                        debug_pc_o, debug_wen, debug_wreg_num, debug_wreg_data);
                    running = false;
                } else {
#ifdef DEBUG
                    fmt::println(
                        "Trace passed at time {}, pc: {:#x}, wen: {}, "
                        "wreg_num: "
                        "{}, "
                        "wreg_data: {:#x}",
                        sim_time, debug_pc_o, debug_wen, debug_wreg_num,
                        debug_wreg_data);
#endif
                    last_pc = debug_pc_o;
                }
            }
        }

        virtual void setup() {
            initialize_dut();
            connect_wire();
            trace();
        }

        virtual void run(std::string trace_file) {
            Verilated::traceEverOn(true);
            auto m_trace = std::make_unique<VerilatedVcdC>();
            cpu_top->trace(m_trace.get(), 99);
            m_trace->open(trace_file.c_str());

            sim_time = 0;
            running = true;

            while (!Verilated::gotFinish() && sim_time >= 0 && running) {
                reset_dut();
                if ((sim_time % clock) == 0) {
                    tick();
                }
                cpu_top->eval();

                if ((sim_time % (2 * clock)) == 0) {
                    input();
                    // 信号连线
                    connect_wire();
                    // 检查是否很长时间没有进行提交并结束仿真
                    detect_commit_timeout();
                    // Trace 判断 Dut 运行是否正确
                    trace();
                }

                m_trace->dump(sim_time);
                sim_time++;
            }

            m_trace->close();

            fmt::println("Simulation finished at time {}", sim_time);
        }

       protected:
        uint64_t clock;
        uint64_t cpu_clk;
        uint64_t cpu_reset;
        uint32_t cpu_inst_i;
        uint32_t read_dmem_data_i;

        // uint32_t pc_o;
        bool dmem_we_o;
        uint32_t write_dmem_data_o;
        uint32_t dmem_addr_o;

        // trace info
        uint64_t debug_wen;
        uint64_t debug_wreg_num;
        uint32_t debug_wreg_data;
        uint32_t debug_pc_o;

        uint32_t last_pc;

        uint64_t start_time;
        uint64_t sim_time;
        uint64_t last_commit;
        bool running;

        std::ifstream trace_file;

        void inline tick() {
            cpu_clk = !cpu_clk;
            cpu_top->clk_i ^= 1;
        }
    };
};  // namespace heliosxsimulator