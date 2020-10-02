//File: nonblocking.v

`timescale 1 ns / 1 ns

module nonblocking (
	output reg [3:0] c,
	output reg [3:0] b,
	input [3:0] a,
	input clk    // Clock

);
always @(posedge clk) begin
	b <= a;
	c <= b;

	$display("非阻塞赋值：c = %d, b = %d, a = %d. ", c, b, a);

end

endmodule