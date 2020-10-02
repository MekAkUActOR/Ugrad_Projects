//File: freq_div20_tb.v

`define CLOCK_CYCLE 50

`include "freq_div20.v"

module freq_div20_tb;

	reg p_clk_10M;
	reg p_rst;

	wire p_clk_500K;

	freq_div20 m_fd20(.clk_500K(p_clk_500K),
		.clk_10M(p_clk_10M),
		.reset  (p_rst));

	initial
	begin
		$dumpfile("freq_div20.vcd");
		$dumpvars(0, freq_div20_tb);

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

		#10000 $stop;
	end

endmodule