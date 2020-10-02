//File: tb_seq_detect.v

`timescale 1 ns / 100 ps 
`include "seq_detect.v"

module tb_seq_detect;

	wire p_flag;

	reg p_clk, p_rst, p_din;

	seq_detect seqd1(
		.flag (p_flag),
		.din  (p_din),
		.clk  (p_clk),
		.rst_n(p_rst)
		);

	initial begin 
		$dumpfile("seq_detect.vcd");
		$dumpvars(0, tb_seq_detect);

		p_rst = 1'b0;
		#25 p_rst = 1'b1;
	end

	initial begin 
		p_clk = 1'b0;
		forever begin 
			#10 p_clk = ~p_clk;
		end
	end

	parameter SIZE = 32;
	reg [SIZE - 1 : 0] data = 32'b0011_0110_1110_0001_0111_1010_0000_1101;

	initial begin : SERIES
		integer i;
		p_din = 1'b0;
		#30;
		for (i = 0; i < SIZE; i++) begin
			p_din = data[SIZE - 1];
			data = data << 1;
			#20;
		end

		$stop;
	end

	initial begin 
		#10 $display("At time %tns, reset = %b, datain = %b, flag = %b.", $time, p_rst, p_din, p_flag);
		forever begin 
			#20 $display("At time %tns, reset = %b, datain = %b, flag = %b.", $time, p_rst, p_din, p_flag);
		end
	end

endmodule