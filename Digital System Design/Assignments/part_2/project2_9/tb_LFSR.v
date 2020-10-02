//File: tb_LFSR.v

`timescale 1 ns / 1 ns
`include "LFSR.v"

module tb_LFSR;

	wire [1:26] p_q;

	reg p_clk, p_rstn, p_load;
	reg [1:26] p_din;

	LFSR L1(
		.q    (p_q),
		.clk  (p_clk),
		.rst_n(p_rstn),
		.load (p_load),
		.din  (p_din)
		);

	initial begin 
		$dumpfile("LFSR.vcd");
		$dumpvars(0, tb_LFSR);

		p_clk = 1'b0;
		forever begin 
			#10 p_clk = ~p_clk;
		end
	end

	initial begin 
		p_rstn = 1'b0;
		#100 p_rstn = 1'b1;
	end

	initial begin 
		p_load = 1'b1;
		p_din = 26'd31;
		#155 p_load = 1'b0;

		#455 p_load = 1'b1;
		p_din = 26'd30;
		#505 p_load = 1'b0;

		#1005 p_load = 1'b1;
		p_din = 26'b0;
		#1055 p_load = 1'b0;

		#1500 $stop;
	end

	initial begin 
		$monitor("At time %tns, rst_n = %b, load = %b, q = %b.", $time, p_rstn, p_load, p_q);
	end

endmodule