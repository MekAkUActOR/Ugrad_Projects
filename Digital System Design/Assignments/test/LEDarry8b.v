`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    13:06:46 05/12/2020 
// Design Name: 
// Module Name:    LEDarry8b 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module LEDarry8b(
    output [7:0] arry,
    input clk,
	input reset
);
reg [25:0] count;
always @(posedge clk) begin
	if(~reset) begin
		count <= 26'b0;
		arry <= 8'b0;
	end else begin
		if(count >= 50000000) begin
			count <= 26'b0;
			if((arry == 8'b0) || (arry == 8'b1111_1111)) begin
				arry <= 8'b1;
			end else begin
				arry <= arry * 2 + 1'b1
			end
		end
		else begin
			count <= count + 1'b1;
		end
	end
end

endmodule