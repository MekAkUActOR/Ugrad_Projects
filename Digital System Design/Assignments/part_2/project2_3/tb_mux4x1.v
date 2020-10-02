//File: tb_mux4x1.v

`timescale 1 ns / 1 ns
`include "mux4x1.v"

module tb_mux4x1;

	wire p_dout;

	reg [3:0] p_din;
	reg [1:0] p_sel;

	mux4x1 m_m4x1(.dout(p_dout),
		.sel(p_sel),
		.din(p_din));

	initial
	begin 
		$dumpfile("mux4x1.vcd");
		$dumpvars(0, tb_mux4x1);

		p_din = 4'b0001;
		repeat(4)
		begin 
			p_sel = 2'b00;
			#10 p_sel = 2'b01;
			#10 p_sel = 2'b10;
			#10 p_sel = 2'b11;
			#10 p_din = p_din << 1;
		end

		#10 $stop;
	end

	initial
	begin 
		$monitor("in = %b, s = %b, out = %b.", p_din, p_sel, p_dout);
	end

endmodule