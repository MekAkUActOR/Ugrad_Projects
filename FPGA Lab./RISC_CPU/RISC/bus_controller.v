/* File: bus_controller.v */
`timescale 1ns / 100ps

module bus_controller (
	output [7:0] data_bus,
	input [7:0] acc_out,
	input bus_enable
	
);

assign data_bus = (bus_enable) ? acc_out : 8'bz;

endmodule
