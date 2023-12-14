#include "soc.hpp"

namespace heliosxsimulator {

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
    void SocSimulator<Dut>::trace() {
        uint32_t ref_pc;
        uint32_t ref_wen;
        uint32_t ref_wreg_num;
        uint32_t ref_wreg_data;

        if (trace_on() && debug_wen && debug_wreg_num) {
            while (trace_file >> std::hex >> ref_pc >> ref_wen >>
                   ref_wreg_num >> ref_wreg_data) {
            }
            if (ref_pc != pc_o || ref_wen != debug_wen ||
                ref_wreg_num != debug_wreg_data ||
                ref_wreg_data != debug_wreg_data) {
                std::cout << "Trace failed at time " << sim_time << std::endl;
                fmt::println(
                    "Expected: pc: {:x}, wen: {:x}, wreg_num: {:x}, "
                    "wreg_data: ",
                    pc_o, debug_wen, debug_wreg_num, debug_wreg_data);
                fmt::println(
                    "Actual: pc: {:x}, wen: {:x}, wreg_num: {:x}, wreg_data: "
                    "{:x}",
                    ref_pc, ref_wen, ref_wreg_num, ref_wreg_data);
                running = false;
            }
        }
    }

    template <class Dut>
    void SocSimulator<Dut>::run() {
        sime_time = 0;
        while (!Verilated::gotFinish() && sime_time > 0 && running) {
            cpu_clk = !cpu_clk;
            cpu_top->eval();
            connect_wire();
        }
    }

}  // namespace heliosxsimulator
