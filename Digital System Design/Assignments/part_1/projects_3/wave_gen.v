//File: wave_gen.v

`timescale 1 ns / 100 ps
module wave_gen (
	output reg wave_out,
	input clk_10M,    // Clock
	input reset  // Asynchronous reset active low
	
);
reg [8:0] cnt;

always @(posedge clk_10M) begin
	if(~reset) begin
		cnt <= 9'b0;
		wave_out <= 1'b0;
	end
	else if(cnt == 9'd199) begin
		wave_out <= ~wave_out;
		cnt <= cnt + 1'b1;
	end
	else if(cnt == 9'd299) begin
		wave_out <= ~wave_out;
		cnt <= cnt + 1'b1;
	end
	else if(cnt == 9'd499)
		cnt <= 9'd0;
	else cnt <= cnt + 1'b1;
end

endmodule