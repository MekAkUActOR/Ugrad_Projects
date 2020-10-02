//File: freq_div2.v

`timescale 1 ns / 100 ps

module freq_div2 (
	output reg clk_out,
	input clk_in,    // Clock
	input reset
	
);

always @(posedge clk_in)
begin
	if (!reset) clk_out <= 0;
	else clk_out <= ~clk_out;
end

endmodule