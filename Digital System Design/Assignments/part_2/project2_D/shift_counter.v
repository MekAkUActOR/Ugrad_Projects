//File: shift_counter.v

`timescale 1 ns / 1 ns

module shift_counter (
	output reg [7:0] count,
	input clk,    // Clock
	input reset  // Synchronous reset active low
	
);
reg [13:0] temp;

always @(posedge clk) begin
	if(~reset) begin
		count <= 8'b1;
		temp <= 14'b1;
	end else begin
		temp <= {temp[12:0], temp[13]};
		if(|temp[13:8])
			count <= {temp[7], temp[8], temp[9], temp[10], temp[11], temp[12], temp[13], 1'b0};
		else count <= temp[7:0];
	end
end

endmodule