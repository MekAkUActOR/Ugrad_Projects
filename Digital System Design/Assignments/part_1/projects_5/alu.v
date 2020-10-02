//File: alu.v

`timescale 1 ns / 1 ns

`define OP_PLUS		3'd0
`define OP_MINUS	3'd1
`define OP_AND		3'd2
`define OP_OR		3'd3
`define OP_NOT		3'd4

module alu (
	output reg [7:0] y,
	input [2:0] opcode,
	input [7:0] b, a
	
);
always @(*) begin
	case (opcode)
		`OP_PLUS:	y = b + a;
		`OP_MINUS:	y = b - a;
		`OP_AND:	y = b & a;
		`OP_OR:		y = b | a;
		`OP_NOT:	y = ~a;
	
		default : 	y = 8'hx;
	endcase
end

endmodule