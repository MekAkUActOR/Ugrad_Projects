//File: comb_dataflow.v

`timescale 1 ns / 1 ns

module comb_dataflow (
	output Y,
	input A, B, C, D
);
wire u1, u2, u3, u4;

assign	u1 = ~D,	u4 = B & C & u1,
		u3 = A | D,	u2 = ~u3,
		Y = u2 & u4;

endmodule