//File: tb_comb_Y2.v

`timescale 1 ns / 1 ns
`include "comb_Y.v"

module tb_comb_Y2;

	wire p_Y;

	reg p_A, p_B, p_C, p_D;

	reg [3:0] temp;

	comb_Y2 c2(
		.Y(p_Y),
		.A(p_A),
		.B(p_B),
		.C(p_C),
		.D(p_D)
		);

	initial
	begin 
		$dumpfile("comb_Y2.vcd");
		$dumpvars(0, tb_comb_Y2);

		temp = 4'b0;
		repeat(15)
		begin 
			{p_A, p_B, p_C, p_D} = temp;
			#10 temp = temp + 1;
		end
		{p_A, p_B, p_C, p_D} = temp;

		#10 $stop;
	end

	initial
	begin 
		$monitor("At time %t, A = %b, B = %b, C = %b, D = %b, Y = %b.", $time, p_A, p_B, p_C, p_D, p_Y);
	end

endmodule
