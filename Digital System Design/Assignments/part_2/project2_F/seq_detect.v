//seq_detect.v

`timescale 1 ns / 100 ps

module seq_detect (
	output reg flag,
	input din,
	input clk,
	input rst_n
	
);
parameter	IDLE= 9'b0_0000_0001,
			A	= 9'b0_0000_0010,
			B 	= 9'b0_0000_0100,
			C 	= 9'b0_0000_1000,
			D 	= 9'b0_0001_0000,
			E 	= 9'b0_0010_0000,
			F 	= 9'b0_0100_0000,
			G 	= 9'b0_1000_0000,
			H	= 9'b1_0000_0000;

reg [8:0] state;

always @(negedge clk) begin
	if(~rst_n) begin
		flag <= 1'b0;
		state <= IDLE;
	end else begin
		case (state)
			IDLE	:	if(din) begin flag <= 1'b0; state <= A; end
						else begin flag <= 1'b0; 	state <= B; end

			A 		:	if(din) begin flag <= 1'b0; state <= C; end
						else begin flag <= 1'b0;	state <= B; end

			B 		:	if(din) begin flag <= 1'b0; state <= D; end
						else begin flag <= 1'b0;	state <= B; end

			C 		: 	if(din) begin flag <= 1'b0; state <= C; end
						else begin flag <= 1'b0; 	state <= E; end

			D 		: 	if(din) begin flag <= 1'b0; state <= F; end
						else begin flag <= 1'b0; 	state <= B; end

			E    	:	if(din) begin flag <= 1'b1; state <= G; end
						else begin flag <= 1'b0; 	state <= B; end

			F 		:	if(din) begin flag <= 1'b0; state <= C; end
						else begin flag <= 1'b1; 	state <= H; end

			G 		:	if(din) begin flag <= 1'b0; state <= F; end
						else begin flag <= 1'b0; 	state <= B; end

			H 		: 	if(din) begin flag <= 1'b1; state <= G; end
						else begin flag <= 1'b0; 	state <= B; end
		
			default : 	begin flag <= 1'b0;	state <= IDLE; end
		endcase
	end
end

endmodule