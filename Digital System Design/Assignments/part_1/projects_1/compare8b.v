//File: compare8b.v
`timescale 10 ns / 1 ns

module compare8b (
	output y,
	input [7:0] a,
	input [7:0] b

);

assign y = (a > b) ? 1 : 0;

endmodule