//File: compare1b_tb.v
`timescale 10 ns / 1 ns
`include "compare1b.v"

module compare1b_tb;
	wire p_y;
	reg p_a, p_b;

	compare1b m_cmp1b(.y(p_y), .b(p_b), .a(p_a));

	initial
	begin
		$dumpfile("compare1b.vcd");
		$dumpvars(0, compare1b_tb);
		p_a = 0; p_b = 0;
		#100 p_a = 0; p_b = 1;
		#100 p_a = 1; p_b = 0;
		#100 p_a = 1; p_b = 1;
		#100 $stop;
	end

	initial
	begin
		$monitor("当前时间: %tns,",$time,"<---->y=%b, b=%b, a=%b",p_y,p_b,p_a);
	end

endmodule