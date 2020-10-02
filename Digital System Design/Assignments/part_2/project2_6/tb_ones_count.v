//File: tb_ones_count.v

`timescale 1 ns / 1 ns
`include "ones_count.v"

module tb_ones_count;

	wire [3:0] p_cnt;

	reg [7:0] p_din;

	ones_count one1(
		.count(p_cnt),
		.dat_in(p_din)
		);

	initial
	begin 
		$dumpfile("ones_count.vcd");
		$dumpvars(0, tb_ones_count);

		p_din = 8'b0;
		repeat(255)
		begin 
			#10 p_din = p_din + 1;
		end

		#10 $stop;

	end

	initial
	begin 
		$monitor("At time %tns, dat_in = %b, count = %d", $time, p_din, p_cnt);
	end

endmodule