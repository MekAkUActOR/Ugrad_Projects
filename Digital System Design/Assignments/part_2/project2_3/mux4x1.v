//File: mux4x1.v

`timescale 1 ns / 1 ns
`include "mux2x1.v"

module mux4x1 (
	output dout,
	input [1:0] sel,
	input [3:0] din
	
);
wire [1:0] dm;

wire [3:0] din1 = {din[3], din[1], din[2], din[0]};

mux2x1 m1(.dout(dm[0]),
	.sel(sel[1]),
	.din(din1[1:0]));

mux2x1 m2(.dout(dm[1]),
	.sel(sel[1]),
	.din(din1[3:2]));

mux2x1 m3(.dout(dout),
	.sel(sel[0]),
	.din(dm));

endmodule