//File: select8.v

`timescale 1 ns / 1 ns

module select8 (
	output reg [3:0] y,
	input [31:0] datain,
	input [2:0] sel
	
);
always @(*) begin
	case (sel)
		3'b000:		y = datain[3:0];
		3'b001:		y = datain[7:4];
		3'b010:		y = datain[11:8];
		3'b011:		y = datain[15:12];
		3'b100:		y = datain[19:16];
		3'b101:		y = datain[23:20];
		3'b110:		y = datain[27:24];
		3'b111:		y = datain[31:28];
	
		default : 	y = 4'bx;
	endcase

end

endmodule