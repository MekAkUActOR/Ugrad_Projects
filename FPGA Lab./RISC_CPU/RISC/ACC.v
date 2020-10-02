/* File: ACC.v */
`timescale 1ns / 100ps

module ACC (
	output reg [7:0] acc_out,
	input clk_ctrl,
	input reset,
	input load_acc,
	input [7:0] data_bus
	
);

//always @(posedge clk_ctrl or posedge reset)
always @(posedge clk_ctrl)	// �м����ݼĴ������ݴ�Ҫ���������
	if (reset) acc_out <= 8'b0;
	else
		if (load_acc) acc_out <= data_bus;

endmodule
