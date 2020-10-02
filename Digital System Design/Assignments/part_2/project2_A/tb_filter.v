//File: tb_filter.v

`timescale 1 ns / 1 ns
`include "filter.v"

module tb_filter;

	wire o_sig;

	reg i_sig;
	reg p_clk, p_rst;

	filter f1(
		.sig_out(o_sig),
		.clock  (p_clk),
		.reset  (p_rst),
		.sig_in (i_sig)
		);

	initial begin 
		$dumpfile("filter.vcd");
		$dumpvars(0, tb_filter);

		p_clk = 1'b0;
		forever begin 
			#10 p_clk = ~p_clk;
		end
	end

	initial begin 
		p_rst = 1'b0;
		#100 p_rst = 1'b1;

		#405 p_rst = 1'b0;
		#105 p_rst = 1'b1;

		#500 $stop;
	end

	initial begin 
		forever begin 
			i_sig = 1;
			#100 i_sig = 0;
			#100;
		end
	end

	initial begin 
		$monitor("At time %t, reset = %b, sig_in = %b, sig_out = %b.", $time, p_rst, i_sig, o_sig);
	end

endmodule