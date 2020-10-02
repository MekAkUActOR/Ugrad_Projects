//File: vfreq_div2.v

`timescale 1 ns / 100 ps

module vfreq_div2 (
	output reg clk_out,
	input clk_in,    // Clock
	input set
	
);

always @(posedge clk_in)
begin
	if (!set) clk_out <= 1;
	else clk_out <= ~clk_out;
end

endmodule