//File: dec_counter.v

`timescale 10 ns / 1 ns

module dec_counter (
	output reg [3:0] counter,
	input clk,    // Clock
	input reset  // Asynchronous reset active low
	
);
always @(posedge clk) 
begin
	if(~reset)
		counter <= 4'd0;
	else if(counter == 4'd9)
		counter <= 4'd0;
	else counter = counter + 4'd1;

end


endmodule