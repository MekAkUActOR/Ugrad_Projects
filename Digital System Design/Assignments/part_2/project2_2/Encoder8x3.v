//File: Encoder8x3.v
//517021911065 刘浩文

`timescale 1 ns / 1 ns

module Encoder8x3 (
	output reg [2:0] code,
	input [7:0] data
	
);
always @(*) begin

	case (data)
		8'b0000_0001:	code = 3'd0;
		8'b0000_0010:	code = 3'd1;
		8'b0000_0100:	code = 3'd2;
		8'b0000_1000:	code = 3'd3;
		8'b0001_0000:	code = 3'd4;
		8'b0010_0000:	code = 3'd5;
		8'b0100_0000:	code = 3'd6;
		8'b1000_0000:	code = 3'd7;
	
		default :		code = 3'dx;
	endcase

end

endmodule