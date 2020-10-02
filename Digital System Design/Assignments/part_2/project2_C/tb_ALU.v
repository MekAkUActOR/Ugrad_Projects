//tb_ALU.v

`timescale 1 ns / 1 ns
`include "ALU.v"

module tb_ALU;

	wire [7:0] p_sum;
	wire p_cout;

	reg [7:0] p_a, p_b;
	reg [3:0] p_op;
	reg p_cin;

	ALU A1(
		.c_out(p_cout),
		.sum  (p_sum),
		.oper (p_op),
		.a    (p_a),
		.b    (p_b),
		.c_in (p_cin)
		);

	initial begin 
		$dumpfile("ALU.vcd");
		$dumpvars(0, tb_ALU);

		p_a = ($random) % 256;
		{p_cin, p_b} = ($random) % 512;

		p_op = 4'd0;

		repeat(79) begin 
			#10;

			p_a = ($random) % 256;
			{p_cin, p_b} = ($random) % 512;

			p_op = p_op + 1'b1;
		end

		#10 $stop;
	end

	initial begin 
		$monitor("oper = %d, a = %b, b = %b, c_out = %b, sum = %b.", p_op, p_a, p_b, p_cout, p_sum);
	end

endmodule