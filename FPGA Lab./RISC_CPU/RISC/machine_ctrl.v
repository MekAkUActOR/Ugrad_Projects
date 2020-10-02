/* File: machine_ctrl.v */
`timescale 1ns / 100ps

module machine_ctrl (
	output enable, 
	input clk_fetch, clk_ctrl,
	input reset
	
);

reg enable1, enable2;

//always @(posedge clk_fetch or posedge reset) begin : proc_enable
always @(posedge clk_fetch) begin
	enable1 <= 1'b1;
end

always @(negedge clk_ctrl)	// 引入时钟信号，使能够同步复位
	if(reset) begin
		enable2 <= 1'b0;
	end else enable2 <= 1'b1;

assign enable = enable1 & enable2;
/*
always @(enable)
	$display("enable-------------------------------------------------- %b", enable);
*/
endmodule
