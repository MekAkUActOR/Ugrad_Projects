//File: tb_counter8b_updown.v

`timescale 1 ns / 1 ns
`include "counter8b_updown.v"

module tb_counter8b_updown;

	wire [7:0] p_cnt;

	reg p_clk, p_rst, p_dir;

	counter8b_updown cnt8b(
		.count(p_cnt),
		.clk  (p_clk),
		.reset(p_rst),
		.dir  (p_dir)
		);

	initial begin 
		$dumpfile("counter8b_updown.vcd");
		$dumpvars(0, tb_counter8b_updown);

		p_clk = 1'b0;
		forever begin 
			#10 p_clk = ~p_clk;
		end
	end

	initial begin 
		p_rst = 1'b0;
		p_dir = 1'b1;
		#100 p_rst = 1'b1;

		#1955 p_rst = 1'b0;
		#100 p_rst = 1'b1;

		#5100 p_dir = 1'b0;

		#1955 p_rst = 1'b0;
		#100 p_rst = 1'b1;

		#5120 $stop;

	end

	initial begin 
		$monitor("At time %tns, reset = %b, dir = %b, count = %d.", $time, p_rst, p_dir, p_cnt);
	end

endmodule