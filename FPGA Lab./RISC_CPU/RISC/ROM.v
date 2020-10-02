/* File: ROM.v */
`timescale 1ns / 100ps

module ROM (
	output [7:0] data_bus,
	input [12:0] address,
	input sel_rom, mem_rd
	
);

reg [7:0] memory[13'b0_1111_1111_1111:0];

assign data_bus = (mem_rd && sel_rom) ? memory[address] : 8'bz;

endmodule
