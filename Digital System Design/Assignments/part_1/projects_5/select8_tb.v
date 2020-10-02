//File: select8_tb.v

`timescale 1 ns / 1 ns
`include "select8.v"

module select8_tb;

	wire [3:0] p_y;

	reg [31:0] p_datain;
	reg [2:0] p_sel;

	select8 m_sel8(.y(p_y),
		.datain(p_datain),
		.sel(p_sel));

	initial
	begin
		$dumpfile("select8.vcd");
		$dumpvars(0, select8_tb);

		p_datain[3:0] = 4'd0;
		p_datain[7:4] = 4'd1;
		p_datain[11:8] = 4'd2;
		p_datain[15:12] = 4'd3;
		p_datain[19:16] = 4'd4;
		p_datain[23:20] = 4'd5;
		p_datain[27:24] = 4'd6;
		p_datain[31:28] = 4'd7;
	end

	initial
	begin
		p_sel = 3'b0;

		repeat(7) begin
		#100 p_sel = p_sel + 1;
		//$display("sel = %d, y = %d.", p_sel, p_y);
		end

		#100 $stop;
	end
	initial
	begin
		$monitor("sel = %d, y = %d.", p_sel, p_y);
	end

endmodule