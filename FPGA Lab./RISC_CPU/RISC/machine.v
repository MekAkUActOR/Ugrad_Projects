/* File: machine.v */
`timescale 1ns / 100ps

module machine (
	output reg halt,
	output reg inc_pc,
	output reg load_pc,
	output reg load_ir,
	output reg load_acc,
	output reg bus_enable,
	output reg mem_rd,
	output reg mem_wr,

	input enable,
	input clk_ctrl,
	input alu_zero_flag,
	input [2:0] opcode
	
);

parameter 	HALT 	=	3'b000,
			JRZ		=	3'b001,
			ADD		=	3'b010,
			AND		=	3'b011,
			XOR		=	3'b100,
			LOAD 	=	3'b101,
			STORE	=	3'b110,
			JUMP	=	3'b111;

parameter 	CYCLE0	=	3'b000,
			CYCLE1	=	3'b001,
			CYCLE2	=	3'b010,
			CYCLE3	=	3'b011,
			CYCLE4	=	3'b100,
			CYCLE5	=	3'b101,
			CYCLE6	=	3'b110,
			CYCLE7	=	3'b111;

reg [2:0] state;

always @(negedge clk_ctrl)
	if (enable != 1'b1)
	begin
		{load_acc, load_ir, load_pc, inc_pc} <= 4'b0000;
		{halt, bus_enable, mem_wr, mem_rd} <= 4'b0000;
		state <= CYCLE0;
	end
	else if(enable == 1'b1) instruction_cycle;

task instruction_cycle;
	casex (state)
		CYCLE0:
			begin
				{ load_acc, load_ir, load_pc, inc_pc } <= 4'b0100;
				{ halt, bus_enable, mem_wr, mem_rd } <= 4'b0001;
				//$display("取指高8位, %b", opcode);
				state <= CYCLE1;
			end
		CYCLE1:
			begin
				{ load_acc, load_ir, load_pc, inc_pc } <= 4'b0101;
				{ halt, bus_enable, mem_wr, mem_rd } <= 4'b0001;
				//$display("取指低8位, %b", opcode);
				state <= CYCLE2;
			end
		CYCLE2:
			begin
				{ load_acc, load_ir, load_pc, inc_pc } <= 4'b0000;
				{ halt, bus_enable, mem_wr, mem_rd } <= 4'b0000;
				//$display("空一个时钟周期, %b", opcode);
				state <= CYCLE3;
			end
		CYCLE3:
			begin
				if (opcode == HALT) begin
					{ load_acc, load_ir, load_pc, inc_pc } <= 4'b0001;
					{ halt, bus_enable, mem_wr, mem_rd } <= 4'b1000;
					//$display("halt下一条指令的地址已准备好, %b", opcode);
				end else begin
					{ load_acc, load_ir, load_pc, inc_pc } <= 4'b0001;
					{ halt, bus_enable, mem_wr, mem_rd } <= 4'b0000;
					//$display("下一条指令的地址已准备好, %b", opcode);
				end
				state <= CYCLE4;
			end
		CYCLE4:
			begin
				if (opcode == JUMP) begin
					{ load_acc, load_ir, load_pc, inc_pc } <= 4'b0010;
					{ halt, bus_enable, mem_wr, mem_rd } <= 4'b0000;
					//$display("loadpc准备跳转, %b", opcode);
				end else
					if (( opcode == ADD) || (opcode == AND) || ( opcode == XOR) || (opcode == LOAD))
					begin
						{ load_acc, load_ir, load_pc, inc_pc } <= 4'b0010;
						{ halt, bus_enable, mem_wr, mem_rd } <= 4'b0001;
						//$display("使用IR中的地址读操作数, %b", opcode);
					end
					else
						if (opcode == STORE) begin
							{ load_acc, load_ir, load_pc, inc_pc } <= 4'b0000;
							{ halt, bus_enable, mem_wr, mem_rd } <= 4'b0100;
							//$display("写数据已准备好, %b", opcode);
						end else begin
							{ load_acc, load_ir, load_pc, inc_pc } <= 4'b0000;
							{ halt, bus_enable, mem_wr, mem_rd } <= 4'b0000;
							//$display("CYCLE4空转, %b", opcode);
						end
				state <= CYCLE5;
			end
		CYCLE5:
			begin
				if ((opcode == ADD) || (opcode == AND) || (opcode == XOR) || (opcode == LOAD))
				begin
					{ load_acc, load_ir, load_pc, inc_pc } <= 4'b1000;
					{ halt, bus_enable, mem_wr, mem_rd } <= 4'b0001;
					//$display("使用IR中的地址读操作数 acc装入数据, %b", opcode);
				end
				else
					if ((opcode == JRZ) && (alu_zero_flag))
					begin
						{ load_acc, load_ir, load_pc, inc_pc } <= 4'b0001;
						{ halt, bus_enable, mem_wr, mem_rd } <= 4'b0000;
						//$display("JRZ判断准备跳过下一条语句, %b", opcode);
					end
					else
						if (opcode == JUMP)
						begin
							{ load_acc, load_ir, load_pc, inc_pc } <= 4'b0011;
							{ halt, bus_enable, mem_wr, mem_rd } <= 4'b0000;
							//$display("IR地址送入PC中准备跳转, %b", opcode);
						end
						else
							if (opcode == STORE)
							begin
								{ load_acc, load_ir, load_pc, inc_pc } <= 4'b0000;
								{ halt, bus_enable, mem_wr, mem_rd } <= 4'b0110;
								//$display("写数据已准备好, %b", opcode);
							end
							else
							begin
								{ load_acc, load_ir, load_pc, inc_pc } <= 4'b0000;
								{ halt, bus_enable, mem_wr, mem_rd } <= 4'b0000;
								//$display("CYCLE5空转, %b", opcode);
							end
				state <= CYCLE6;
			end
		CYCLE6:
			begin
				if (opcode == STORE)
				begin
					{ load_acc, load_ir, load_pc, inc_pc } <= 4'b0000;
					{ halt, bus_enable, mem_wr, mem_rd } <= 4'b0100;
					//$display("acc数据写到数据总线上, %b", opcode);
				end
				else
					if ((opcode == ADD) || (opcode == AND) || (opcode == XOR) || (opcode == LOAD))
					begin
						{ load_acc, load_ir, load_pc, inc_pc } <= 4'b0000;
						{ halt, bus_enable, mem_wr, mem_rd } <= 4'b0001;
						//$display("读出存储器的数据, %b", opcode);
					end
					else
					begin
						{ load_acc, load_ir, load_pc, inc_pc } <= 4'b0000;
						{ halt, bus_enable, mem_wr, mem_rd } <= 4'b0000;
						//$display("CYCLE6空转, %b", opcode);
					end
				state <= CYCLE7;
			end
		CYCLE7:
			begin
				if ((opcode == JRZ) && (alu_zero_flag == 1))
				begin
					{ load_acc, load_ir, load_pc, inc_pc } <= 4'b0001;
					{ halt, bus_enable, mem_wr, mem_rd } <= 4'b0000;
					//$display("JRZ跳过一条指令, %b", opcode);
				end
				else
				begin
					{ load_acc, load_ir, load_pc, inc_pc } <= 4'b0000;
					{ halt, bus_enable, mem_wr, mem_rd } <= 4'b0000;
					//$display("CYCLE7空转, %b", opcode);
				end
				state <= CYCLE0;
			end
		default:
			begin
				{ load_acc, load_ir, load_pc, inc_pc } <= 4'b0000;
				{ halt, bus_enable, mem_wr, mem_rd } <= 4'b0000;
				//$display("default空转, %b", opcode);
				state <= CYCLE0;
			end
	endcase
endtask


endmodule
