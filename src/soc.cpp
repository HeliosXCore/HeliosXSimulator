#include "soc.hpp"
#include <iostream>

namespace heliosxsimulator {

    template <class Dut>
    void SocSimulator<Dut>::tick() {
        cpu_clk = !cpu_clk;
        cpu_top->clk_i ^= 1;
    }

    template <class Dut>
    void SocSimulator<Dut>::initialize_dut() {
        cpu_top->cpu_clk_i = 0;
        cpu_top->cpu_reset_i = 0;
        cpu_top->cpu_inst_i = 0;
        cpu_top->read_dmem_data_i = 0;
    }

    template <class Dut>
    void SocSimulator<Dut>::connect_wire() {
        cpu_top->cpu_clk_i = cpu_clk;
        cpu_top->cpu_reset_i = cpu_reset;
        cpu_top->cpu_inst_i = cpu_inst_i;
        cpu_top->read_dmem_data_i = read_dmem_data_i;

        pc_o = cpu_top->pc_o;
        dmem_we_o = cpu_top->dmem_we_o;
        write_dmem_data_o = cpu_top->write_dmem_data_o;
        dmem_addr_o = cpu_top->dmem_addr_o;
    }

    template <class Dut>
    void SocSimulator<Dut>::detect_commit_timeout() {
        if (debug_wen) {
            last_commit = sim_time;
        } else if (sim_time - last_commit > COMMIT_TIMEOUT) {
            std::cout << "Commit timeout at time " << sim_time << std::endl;
            running = false;
        }
    }

    template <class Dut>
    void SocSimulator<Dut>::trace() {
        uint32_t ref_pc;
        uint32_t ref_wen;
        uint32_t ref_wreg_num;
        uint32_t ref_wreg_data;

        if (trace_on() && debug_wen && debug_wreg_num) {
            //     while (trace_file >> std::hex >> ref_pc >> ref_wen >>
            //            ref_wreg_num >> ref_wreg_data) {
            //     }
            //     if (ref_pc != pc_o || ref_wen != debug_wen ||
            //         ref_wreg_num != debug_wreg_data ||
            //         ref_wreg_data != debug_wreg_data) {
            //         std::cout << "Trace failed at time " << sim_time <<
            //         std::endl; fmt::println(
            //             "Expected: pc: {:x}, wen: {:x}, wreg_num: {:x}, "
            //             "wreg_data: ",
            //             pc_o, debug_wen, debug_wreg_num, debug_wreg_data);
            //         fmt::println(
            //             "Actual: pc: {:x}, wen: {:x}, wreg_num: {:x},
            //             wreg_data: "
            //             "{:x}",
            //             ref_pc, ref_wen, ref_wreg_num, ref_wreg_data);
            //         running = false;
            //     }
            // }

            std::shared_ptr<DifftestResult> difftest_result =
                std::make_shared<DifftestResult>();
            emulator->exec(1, difftest_result.get());
            if (difftest_result->pc != pc_o ||
                difftest_result->wen != debug_wen ||
                difftest_result->reg_id != debug_wreg_num ||
                difftest_result->reg_val != debug_wreg_data) {
                std::cout << "Trace failed at time " << sim_time << std::endl;
                fmt::println(
                    "Expected: pc: {:x}, wen: {:x}, wreg_num: {:x}, "
                    "wreg_data: ",
                    pc_o, debug_wen, debug_wreg_num, debug_wreg_data);
                fmt::println(
                    "Actual: pc: {:x}, wen: {:x}, wreg_num: {:x}, wreg_data: "
                    "{:x}",
                    difftest_result->pc, difftest_result->wen,
                    difftest_result->reg_id, difftest_result->reg_val);
                running = false;
            }
        }
    }

    template <class Dut>
    void SocSimulator<Dut>::run(std::string trace_file) {
        Verilated::traceEverOn(true);
        auto m_trace = std::make_unique<VerilatedVcdC>();
        cpu_top->trace(m_trace.get(), 99);
        m_trace->open(trace_file.c_str());

        sim_time = 0;
        while (!Verilated::gotFinish() && sim_time > 0 && running) {
            if ((sim_time % clock) == 0) {
                tick();
            }
            cpu_top->eval();

            if ((sim_time % clock) == 0) {
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
    }

}  // namespace heliosxsimulator
