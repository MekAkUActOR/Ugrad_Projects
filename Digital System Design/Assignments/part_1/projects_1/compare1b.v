//数字系统设计作业Part_1
//File: compare1b.v
`timescale 10 ns/ 1 ns

module compare1b (
	output y,
	input b,
	input a
	
);
assign y = (a==b) ? 1 : 0;

endmodule