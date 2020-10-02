//File: tb_mux2x1.v

`timescale 1 ns / 1 ns
`include "mux2x1.v"

module tb_mux2x1;

	wire p_dout;

	reg [1:0] p_din;
	reg p_sel;

	mux2x1 m_m2x1(.dout(p_dout),
		.sel(p_sel),
		.din(p_din));

	initial
	begin 
		$dumpfile("mux2x1.vcd");
		$dumpvars(0, tb_mux2x1);

		p_din = 2'b10;
		p_sel = 1'b0;
		#10 p_sel = 1'b1;

		#10 $stop;
	end

	initial
	begin 
		$monitor("in0 = %b, in1 = %b, s = %b, out = %b.", p_din[0], p_din[1], p_sel, p_dout);
	end

endmodule