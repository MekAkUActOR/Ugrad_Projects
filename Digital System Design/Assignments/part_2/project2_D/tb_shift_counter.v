//tb_shift_counter.v

`timescale 1 ns / 1 ns

`include "shift_counter.v"

module tb_shift_counter;

	wire [7:0] p_cnt;

	reg p_clk, p_rst;

	shift_counter shc1(
		.count(p_cnt),
		.clk  (p_clk),
		.reset(p_rst)
		);

	initial begin 
		$dumpfile("shift_counter.vcd");
		$dumpvars(0, tb_shift_counter);

		p_clk = 1'b0;
		forever begin 
			#10 p_clk = ~p_clk;
		end
	end

	initial begin 
		p_rst = 1'b0;
		#55 p_rst = 1'b1;

		#2000 $stop;
	end

	initial begin 
		$monitor("At time %tns, reset = %b, count = %b", $time, p_rst, p_cnt);
	end

endmodule