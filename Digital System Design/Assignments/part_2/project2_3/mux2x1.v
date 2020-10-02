//File: mux2x1.v
//517021911065 刘浩文

`timescale 1 ns / 1 ns

module mux2x1 (
	output dout,
	input sel,
	input [1:0] din
	
);
bufif0 b0(dout, din[0], sel);
bufif1 b1(dout, din[1], sel);

endmodule