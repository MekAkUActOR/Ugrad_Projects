//File: sram.v

`timescale 1 ns / 1 ns

/*module sram #(parameter DATA_WIDTH = 8, ADDR_WIDTH = 8)
(
	output reg [DATA_WIDTH - 1:0] dout,
	input [DATA_WIDTH - 1:0] din,
	input [ADDR_WIDTH - 1:0] addr,
	input wr,
	input rd,
	input cs
	
);
parameter RAM_DEPTH = 1 << ADDR_WIDTH;
reg [DATA_WIDTH - 1:0] mem [0:RAM_DEPTH - 1];

always @(posedge wr, negedge rd) begin 
	if(cs) begin
		if(~rd) begin 
			dout <= mem[addr];
		end
		else begin 
			mem[addr] <= din;
			dout <= 8'bz;
		end
	end else dout <= 8'bz;

end

endmodule*/

module sram #(parameter DATA_WIDTH = 8, ADDR_WIDTH = 8)
(
	output [DATA_WIDTH - 1:0] dout,
	input [DATA_WIDTH - 1:0] din,
	input [ADDR_WIDTH - 1:0] addr,
	input wr,
	input rd,
	input cs
	
);
parameter RAM_DEPTH = 1 << ADDR_WIDTH;
reg [DATA_WIDTH - 1:0] mem [0:RAM_DEPTH - 1];

reg [DATA_WIDTH - 1:0] data;
reg flag;

always @(negedge rd, addr, posedge flag) begin
	if(~rd) begin 
		flag <= 1;
		data <= mem[addr];
	end
	else data <= 8'bz;
end

always @(posedge wr) begin 
	if(cs) begin 
		if(~rd) flag <= 0;
		mem[addr] <= din;
		#1;
		if(~rd) flag <= 1;

	end
end

assign dout = (cs&&flag) ? data : 8'bz;

endmodule