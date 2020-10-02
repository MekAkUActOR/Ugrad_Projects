//File: counter8b_updown.v

`timescale 1 ns / 1 ns

module counter8b_updown (
	output reg [7:0] count,
	input clk,    // Clock
	input reset,  // Asynchronous reset active low
	input dir
	
);
always @(posedge clk or negedge reset) begin
	if(~reset) begin
		count <= 8'b0;
	end else case (dir)
		1'b0:	count = count - 1'b1;
		1'b1:	count = count + 1'b1;
	
		default : /* default */;
	endcase
end

endmodule