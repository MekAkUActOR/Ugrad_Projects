//File: tb_dec_counter.v

`timescale 10 ns / 1 ns
`include "dec_counter.v"

module tb_dec_counter;

	wire [3:0] p_counter;

	reg p_clk, p_reset;

	dec_counter m_decc(
		.counter(p_counter),
		.clk    (p_clk),
		.reset  (p_reset)
		);

	initial
	begin 
		$dumpfile("dec_counter.vcd");
		$dumpvars(0, tb_dec_counter);

		p_clk = 1'b0;
		forever
		begin 
			#10 p_clk = ~p_clk;
		end

	end

	initial
	begin 
		p_reset = 1'b0;
		#100 p_reset = 1'b1;
		#1000 $stop;

	end

	initial
	begin 
		$monitor("At time %tns, reset = %b, counter = %d.", $time, p_reset, p_counter);
	end

endmodule
