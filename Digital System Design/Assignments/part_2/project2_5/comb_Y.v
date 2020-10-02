//File: comb_Y.v

`timescale 1 ns / 1 ns

module comb_Y1 (
	output	Y,
	input	A, B, C
	
);
assign	Y = ~A & ~B & C 
		| ~A & B & ~C 
		| A & ~B & ~C 
		| A & ~B & C;

endmodule

module comb_Y2 (
	output	Y,
	input	A, B, C, D
	
);
assign	Y = ~A & B & ~C & ~D 
		| ~A & B & ~C & D 
		| ~A & B & C & ~D 
		| ~A & B & C & D 
		| A & ~B & C & D 
		| A & B & ~C & ~D 
		| A & B & ~C & D;

endmodule