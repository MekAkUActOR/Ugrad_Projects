//File: top.v

`timescale 1 ns / 100 ps 
`include "mealy.v"
`include "moore.v"

module top;

	wire p_mlflag;
	wire p_mrflag;

	reg p_clk, p_rst, p_din;

	mealy meal(
		.flag (p_mlflag),
		.din  (p_din),
		.clk  (p_clk),
		.rst(p_rst)
		);

	moore moor(
		.flag (p_mrflag),
		.din  (p_din),
		.clk  (p_clk),
		.rst(p_rst)
		);

	initial begin 
		$dumpfile("mealy_moore.vcd");
		$dumpvars(0, top);

		p_rst = 1'b1;
		#25 p_rst = 1'b0;

		#250.5 p_rst = 1'b1;
		#5 p_rst = 1'b0;
	end

	initial begin 
		p_clk = 1'b0;
		forever begin 
			#10 p_clk = ~p_clk;
		end
	end

	parameter SIZE = 64;
	reg [SIZE - 1 : 0] data = 64'b0011_1011_0101_0101_0110_1010_1000_1101_1001_0101_0101_1010_1010_1010_1010_1010;

	initial begin : SERIES
		integer i;
		p_din = 1'b0;
		#30;
		for (i = 0; i < SIZE; i++) begin
			p_din = data[0];
			data = data >> 1;
			#20;
		end

		$stop;
	end

	initial begin 
		$display("At time %tns, reset = %b, datain = %b, mealy_flag = %b, moore_flag = %b.", $time, p_rst, p_din, p_mlflag, p_mrflag);
		forever begin 
			#20 $display("At time %tns, reset = %b, datain = %b, mealy_flag = %b, moore_flag = %b.", $time, p_rst, p_din, p_mlflag, p_mrflag);
		end
	end

endmodule