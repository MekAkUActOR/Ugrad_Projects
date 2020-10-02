/* File: tb_analys.v */
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
`include "address_decoder.v"
`include "RAM.v"
`include "ROM.v"

module test_bench;

	reg clock, reset;
	wire halt;

	wire [12:0] address;
	wire [7:0] data_bus;

	wire mem_wr, mem_rd;
	wire sel_rom, sel_ram;

	wire [2:0] opcode;
	wire clk_ctrl, clk_fetch, clk_alu;
	wire sm_enable, inc_pc, load_pc, load_acc, load_ir, bus_enable, alu_zero_flag;
	wire [12:0] addr_pc, addr_ir;
	wire [7:0] acc_out, alu_out;

	reg [7:0] n;

	clock_generator m_clkgen(.clk_ctrl(clk_ctrl), .clk_fetch(clk_fetch), .clk_alu(clk_alu), .clock(clock), .reset(reset));
	machine_ctrl m_mc(.enable(sm_enable), .clk_fetch(clk_fetch), .clk_ctrl(clk_ctrl), .reset(reset));
	machine m_sm(.halt(halt), .inc_pc(inc_pc), .load_pc(load_pc), .load_ir(load_ir), .load_acc(load_acc), .bus_enable(bus_enable), .mem_rd(mem_rd), .mem_wr(mem_wr), .clk_ctrl(clk_ctrl), .enable(sm_enable), .alu_zero_flag(alu_zero_flag), .opcode(opcode));
	PC m_pc(.addr_pc(addr_pc), .inc_pc(inc_pc), .clk_ctrl(clk_ctrl), .reset(reset), .load_pc(load_pc), .addr_ir(addr_ir));
	IR m_ir(.opcode(opcode), .addr_ir(addr_ir), .clk_ctrl(clk_ctrl), .reset(reset), .load_ir(load_ir), .data_bus(data_bus));
	ACC m_acc(.acc_out(acc_out), .clk_ctrl(clk_ctrl), .reset(reset), .load_acc(load_acc), .data_bus(alu_out));
	ALU m_alu(.alu_out(alu_out), .alu_zero_flag(alu_zero_flag), .clk_alu(clk_alu), .opcode(opcode), .acc_out(acc_out), .data_bus(data_bus));
	address_mux m_addrmux(.address(address), .clk_fetch(clk_fetch), .addr_pc(addr_pc), .addr_ir(addr_ir));
	bus_controller m_bus_ctrl(.data_bus(data_bus), .bus_enable(bus_enable), .acc_out(acc_out));
	
	reg [15:0] instruction;
	always @(*)
		instruction = { opcode, addr_ir };

	/*
	CPU m_cpu(
		.mem_rd (mem_rd),
		.mem_wr (mem_wr),
		.address(address),
		.halt   (halt),
		.data_bus(data_bus),
		.clock  (clock),
		.reset  (reset)
		);
	*/

	address_decoder m_ad(
		.sel_rom(sel_rom),
		.sel_ram(sel_ram),
		.address(address)
		);

	ROM m_rom(
		.data_bus(data_bus),
		.address (address),
		.sel_rom (sel_rom),
		.mem_rd  (mem_rd)
		);

	RAM m_ram(
		.data_bus(data_bus),
		.address (address),
		.sel_ram (sel_ram),
		.mem_rd  (mem_rd),
		.mem_wr  (mem_wr)
		);

/*
	always @(address)
		$display("address %b", address);

	always @(data_bus)
		$display("data_bus %b", data_bus);
*/


	initial begin 
		$dumpfile("analys.vcd");
		$dumpvars(0, test_bench);

		$readmemb("Fibonacci.asm", m_rom.memory);
		$display("ROM loaded successfully!");

		#1 clock = 1'b0;
		forever begin 
			#10 clock = ~clock;
		end
	end

	initial begin
		//reset = 1'b0;
		reset = 1'b1;
		#100 reset = 1'b0;
		#22000
		for(n=0; n<=34; n=n+1) $display("ROM: %b; RAM: %b.", m_rom.memory[n], m_ram.memory[n]);
		$stop;
	end

	initial begin 
		$monitor("At time %tns, reset = %b, instruction = %b, temp = %b, fn1 = %b, fn2 = %b.", $time, reset, instruction, m_ram.memory[0], m_ram.memory[1], m_ram.memory[2]);
	end

endmodule