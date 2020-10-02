//File: comb_str.v

`timescale 1 ns / 1 ns

module comb_str (
	output	y,
	input	sel,
	input	A, B, C, D
	
);
wire u1, u2;

nand #3 na1(u1, A, B);
nand #4 na2(u2, C, D);
bufif0 b0(y, u1, sel);
bufif1 b1(y, u2, sel);

endmodule