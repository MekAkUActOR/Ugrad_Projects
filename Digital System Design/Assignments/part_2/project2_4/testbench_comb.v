//File: testbench_comb.v

`timescale 1 ns / 1 ns
`include "comb_str.v"
`include "comb_dataflow.v"
`include "comb_behavior.v"
`include "comb_prim.v"

module testbench_comb;

	wire [3:0] p_Y;

	reg p_A, p_B, p_C, p_D;

	reg [3:0] temp;

	comb_str c1(.Y(p_Y[0]),
		.A(p_A), .B(p_B), .C(p_C), .D(p_D));

	comb_dataflow c2(.Y(p_Y[1]),
		.A(p_A), .B(p_B), .C(p_C), .D(p_D));

	comb_behavior c3(.Y(p_Y[2]),
		.A(p_A), .B(p_B), .C(p_C), .D(p_D));

	comb_prim c4(.Y(p_Y[3]),
		.A(p_A), .B(p_B), .C(p_C), .D(p_D));

	initial
	begin 
		$dumpfile("comb.vcd");
		$dumpvars(0, testbench_comb);

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
		$monitor("At time %t, A = %b, B = %b, C = %b, D = %b, Y_str = %b, Y_df = %b, Y_bh = %b, Y_pr = %b.", $time, p_A, p_B, p_C, p_D, p_Y[0], p_Y[1], p_Y[2], p_Y[3]);
	end

endmodule