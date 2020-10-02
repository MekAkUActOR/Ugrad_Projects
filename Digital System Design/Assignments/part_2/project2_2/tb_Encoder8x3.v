//File: tb_Encoder8x3.v

`timescale 1 ns / 1 ns
`include "Encoder8x3.v"

module tb_Encoder8x3;

	wire [2:0] p_code;

	reg [7:0] p_data;

	Encoder8x3 m_enc8x3(.code(p_code),
		.data(p_data));

	initial
	begin
		$dumpfile("Encoder8x3.vcd");
		$dumpvars(0, tb_Encoder8x3);

		p_data = 8'b1;

		repeat(7) #10 p_data = p_data << 1;
		
		#10 $stop;
	end

	initial
	begin 
		$monitor("data = %b, code = %d.", p_data, p_code);
	end

endmodule