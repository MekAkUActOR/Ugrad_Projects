//File: blocking.v

`timescale 1 ns / 1 ns

module blocking (
	output reg [3:0] c, b,
	input [3:0] a,
	input clk   // Clock
	
);
always @(posedge clk) begin
	//b = a;
	c = b;
	//b = a;

	$display("阻塞赋值：c = %d, b = %d, a = %d. ", c, b, a);

end

always @(posedge clk) begin
	//b = a;
	a = b;
	//b = a;

	$display("阻塞赋值：c = %d, b = %d, a = %d. ", c, b, a);

end

endmodule