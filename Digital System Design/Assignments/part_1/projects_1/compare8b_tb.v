//File: compare8b_tb.v
`timescale 10 ns / 1 ns
`include "compare8b.v"

module compare8b_tb;
	wire p_y;
	reg [7:0] p_a, p_b;

	compare8b tb_cmp8b(.y(p_y), .a(p_a), .b(p_b));

	initial
	begin
		$dumpfile("compare8b.vcd");
		$dumpvars(0, compare8b_tb);
		p_a = 0; p_b = 0;
		repeat(255)
		begin
			#1 p_a = p_a + 1;
			p_b = 0;
			repeat(255)
			begin
				#1 p_b = p_b + 1;
			end
		end
		#5 $stop;
	end

	initial
	begin
		$monitor("当前时间: %tns,",$time,"<---->y=%b, a=%b, b=%b",p_y,p_a,p_b);
	end

endmodule