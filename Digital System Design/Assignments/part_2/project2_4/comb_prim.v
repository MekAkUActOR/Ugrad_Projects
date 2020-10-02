//File: comb_prim.v

`timescale 1 ns / 1 ns

primitive prim_udp(output Y,
	input A, 
	input B, 
	input C, 
	input D
	);
table
	//	A B C D : Y
		0 0 0 0 : 0;
		0 0 0 1 : 0;
		0 0 1 0 : 0;
		0 0 1 1 : 0;
		0 1 0 0 : 0;
		0 1 0 1 : 0;
		0 1 1 0 : 1;
		0 1 1 1 : 0;
		1 0 0 0 : 0;
		1 0 0 1 : 0;
		1 0 1 0 : 0;
		1 0 1 1 : 0;
		1 1 0 0 : 0;
		1 1 0 1 : 0;
		1 1 1 0 : 0;
		1 1 1 1 : 0;
endtable

endprimitive


module comb_prim (
	output Y,
	input A, B, C, D
	
);
prim_udp(Y, A, B, C, D);

endmodule