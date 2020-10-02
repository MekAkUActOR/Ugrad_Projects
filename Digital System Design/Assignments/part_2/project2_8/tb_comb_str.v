//File: tb_comb_str.v

`timescale 1 ns / 1 ns
`include "comb_str.v"

module tb_comb_str;

	wire p_y;

	reg p_sel;
	reg [3:0] temp;

	comb_str m_cb1(
		.y(p_y),
		.sel(p_sel),
		.A(temp[0]),
		.B(temp[1]),
		.C(temp[2]),
		.D(temp[3])
		);

	initial
	begin 
		$dumpfile("comb_str.vcd");
		$dumpvars(0, tb_comb_str);

		temp = 4'b0;
		repeat(16)
		begin 
			p_sel = 1'b0;
			#3 p_sel = ~p_sel;
			#3 p_sel = ~p_sel;
			#3 p_sel = ~p_sel;
			#3 temp = temp + 4'b1;
		end

		$stop;
	end

	initial
	begin 
		$monitor("At time %tns, A = %b, B = %b, C = %b, D = %b, sel = %b, y = %b.", $time, temp[0], temp[1], temp[2], temp[3], p_sel, p_y);
	end



endmodule