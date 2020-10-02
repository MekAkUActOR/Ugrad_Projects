//File: ALU.v

`timescale 1 ns / 1 ns

`define OP_AND		4'd0
`define OP_SUBSTR	4'd1
`define OP_SUBSTR_A	4'd2
`define OP_OR_AB	4'd3
`define OP_AND_AB	4'd4
`define OP_NOT_AB	4'd5
`define OP_EXOR		4'd6
`define OP_EXNOR	4'd7

module ALU (
	output reg c_out,
	output reg [7:0] sum,
	input [3:0] oper,
	input [7:0] a,
	input [7:0] b,
	input c_in
	
);
always @(*) begin
	case (oper)
		`OP_AND 	:	{c_out, sum} = a + b + c_in;
		`OP_SUBSTR 	:	{c_out, sum} = a + ~b + c_in;
		`OP_SUBSTR_A:	{c_out, sum} = b + ~a + ~c_in;
		`OP_OR_AB	:	{c_out, sum} = {1'b0, a | b};
		`OP_AND_AB	:	{c_out, sum} = {1'b0, a & b};
		`OP_NOT_AB	:	{c_out, sum} = {1'b0, (~a) & b};
		`OP_EXOR 	:	{c_out, sum} = {1'b0, a ^ b};
		`OP_EXNOR	:	{c_out, sum} = {1'b0, a ~^ b};
	
		default		:	{c_out, sum} = 9'bx;
	endcase

end


endmodule