/* File: address_decoder.v */
`timescale 1ns / 100ps

module address_decoder (
	output reg sel_rom, sel_ram,
	input [12:0] address
	
);

always @(address)
	casex (address)
		13'b1_xxxx_xxxx_xxxx:	{ sel_rom, sel_ram } <= 2'b01;
		13'b0_xxxx_xxxx_xxxx:	{ sel_rom, sel_ram } <= 2'b10;
		default:				{ sel_rom, sel_ram } <= 2'b00;
	endcase

endmodule
