//File: wave_gen_tb.v

`timescale 1 ns / 100 ps
`define CLOCK_CYCLE 50
`include "wave_gen.v"

module wave_gen_tb;

	reg p_clk_10M;
	reg p_rst;

	wire p_wave_out;

	wave_gen m_wvg(.wave_out(p_wave_out),
		.clk_10M(p_clk_10M),
		.reset(p_rst));

	initial
	begin
		$dumpfile("wave_gen.vcd");
		$dumpvars(0, wave_gen_tb);
		p_clk_10M = 1'b0;

		forever
		begin
			#`CLOCK_CYCLE;
			p_clk_10M = ~p_clk_10M;
		end
	end

	initial
	begin
		p_rst = 1'b1;

		#125 p_rst = 1'b0;
		#960 p_rst = 1'b1;

		#1000000 $stop;
	end

endmodule