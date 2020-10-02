/* File: RAM.v */
`timescale 1ns / 100ps

module RAM (
	inout [7:0] data_bus,
	input [12:0] address,
	input sel_ram, mem_rd, mem_wr
	
);

reg [7:0] memory[13'b0_1111_1111_1111:0];
integer offset = 13'b1_0000_0000_0000;
assign data_bus = (mem_rd && sel_ram) ? memory[address - offset] : 8'bz;
always @(posedge mem_wr)
	memory[address - offset] <= data_bus;

endmodule
