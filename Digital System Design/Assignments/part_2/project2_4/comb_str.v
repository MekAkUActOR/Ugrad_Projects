//File: comb_str.v

`timescale 1 ns / 1 ns

module comb_str (
	output Y,
	input A, B, C, D
	
);
wire u1, u2, u3, u4;

not n1(u1, D);
and a1(u4, B, C, u1);
or o1(u3, A, D);
not n2(u2, u3);
and a2(Y, u2, u4);

endmodule