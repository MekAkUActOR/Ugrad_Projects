/* File: address_mux.v */
`timescale 1ns / 100ps

module address_mux (
	output [12:0] address,
	input clk_fetch,
	input [12:0] addr_pc, addr_ir
	
);

assign address = (clk_fetch) ? addr_pc : addr_ir;

endmodule
