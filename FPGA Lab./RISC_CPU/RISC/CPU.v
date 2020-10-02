/* File: CPU.v */
`timescale 1ns / 100ps
`include "ACC.v"
`include "address_mux.v"
`include "ALU.v"
`include "bus_controller.v"
`include "clock_generator.v"
`include "IR.v"
`include "machine.v"
`include "machine_ctrl.v"
`include "PC.v"

module CPU (
	output mem_rd, mem_wr,
	output [12:0] address,
	output halt,
	inout [7:0] data_bus,
	input clock, reset
	
);
/*
always @(*) begin
	$display("addr_pc %b", addr_pc);
end

always @(*) begin
	$display("instruction %b", { opcode, addr_ir });
end
*/

wire [2:0] opcode;
wire clk_ctrl, clk_fetch, clk_alu;
wire sm_enable, inc_pc, load_pc, load_acc, load_ir, bus_enable, alu_zero_flag;
wire [12:0] addr_pc, addr_ir;
wire [7:0] acc_out, alu_out;

clock_generator m_clkgen(.clk_ctrl(clk_ctrl), .clk_fetch(clk_fetch), .clk_alu(clk_alu), .clock(clock), .reset(reset));
machine_ctrl m_mc(.enable(sm_enable), .clk_fetch(clk_fetch), .clk_ctrl(clk_ctrl), .reset(reset));
machine m_sm(.halt(halt), .inc_pc(inc_pc), .load_pc(load_pc), .load_ir(load_ir), .load_acc(load_acc), .bus_enable(bus_enable), .mem_rd(mem_rd), .mem_wr(mem_wr), .clk_ctrl(clk_ctrl), .enable(sm_enable), .alu_zero_flag(alu_zero_flag), .opcode(opcode));
PC m_pc(.addr_pc(addr_pc), .inc_pc(inc_pc), .clk_ctrl(clk_ctrl), .reset(reset), .load_pc(load_pc), .addr_ir(addr_ir));
IR m_ir(.opcode(opcode), .addr_ir(addr_ir), .clk_ctrl(clk_ctrl), .reset(reset), .load_ir(load_ir), .data_bus(data_bus));
ACC m_acc(.acc_out(acc_out), .clk_ctrl(clk_ctrl), .reset(reset), .load_acc(load_acc), .data_bus(alu_out));
ALU m_alu(.alu_out(alu_out), .alu_zero_flag(alu_zero_flag), .clk_alu(clk_alu), .opcode(opcode), .acc_out(acc_out), .data_bus(data_bus));
address_mux m_addrmux(.address(address), .clk_fetch(clk_fetch), .addr_pc(addr_pc), .addr_ir(addr_ir));
bus_controller m_bus_ctrl(.data_bus(data_bus), .bus_enable(bus_enable), .acc_out(acc_out));

endmodule
