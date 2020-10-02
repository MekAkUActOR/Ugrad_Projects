/* File: ALU.v */
`timescale 1ns / 100ps

module ALU (
	output reg [7:0] alu_out,
	output alu_zero_flag,
	input clk_alu,
	input [2:0] opcode,
	inout [7:0] acc_out,
	input [7:0] data_bus
	
);

parameter	HALT	=	3'b000,
			JRZ		=	3'b001,
			ADD		=	3'b010,
			AND		=	3'b011,
			XOR		=	3'b100,
			LOAD	=	3'b101,
			STORE	=	3'b110,
			JUMP	=	3'b111;

assign alu_zero_flag = !alu_out;	// JRZ判断用

always @(posedge clk_alu)
	casex (opcode)	// 依据指令进行相应计算
		HALT:	alu_out <= acc_out;
		JRZ:	alu_out <= acc_out;
		ADD:	alu_out <= acc_out + data_bus;
		AND:	alu_out <= acc_out & data_bus;
		XOR:	alu_out <= acc_out ^ data_bus;
		LOAD:	alu_out <= data_bus;
		STORE:	alu_out <= acc_out;
		JUMP:	alu_out <= acc_out;
		default:
				alu_out <= 8'bx;
	endcase

endmodule
