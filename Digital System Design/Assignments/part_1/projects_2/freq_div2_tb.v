//File: freq_div2_tb.v

`include "freq_div2.v"
`timescale 1 ns / 100 ps 
`define CLOCK_CYCLE 20

module freq_div2_tb;
	reg p_clk_in, p_rst;

	wire p_clk_out;

	freq_div2 m_freq_div2(.clk_out(p_clk_out),
		.clk_in(p_clk_in),
		.reset(p_rst));

	initial
	begin
		$dumpfile("freq_div2.vcd");
		$dumpvars(0, freq_div2_tb);
		p_clk_in = 1'b0;
		forever #`CLOCK_CYCLE p_clk_in = ~p_clk_in;
	end 

	initial
	begin
		p_rst = 1'b0;

		#70 p_rst = 1'b1;
		#130 p_rst = 1'b0;
		#110 p_rst = 1;
		#10000 $stop;
	end

endmodule