//File: tb_comb_Y1.v

`timescale 1 ns / 1 ns
`include "comb_Y.v"

module tb_comb_Y1;

	wire p_Y;

	reg p_A, p_B, p_C;

	reg [2:0] temp;

	comb_Y1 c1(
		.Y(p_Y),
		.A(p_A),
		.B(p_B),
		.C(p_C)
		);

	initial
	begin 
		$dumpfile("comb_Y1.vcd");
		$dumpvars(0, tb_comb_Y1);

		temp = 3'b0;
		repeat(7)
		begin 
			{p_A, p_B, p_C} = temp;
			#10 temp = temp + 1;
		end
		{p_A, p_B, p_C} = temp;

		#10 $stop;
	end

	initial
	begin 
		$monitor("At time %t, A = %b, B = %b, C = %b, Y = %b.", $time, p_A, p_B, p_C, p_Y);
	end

endmodule