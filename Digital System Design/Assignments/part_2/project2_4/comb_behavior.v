//File: comb_behavior.v

`timescale 1 ns / 1 ns

module comb_behavior (
	output reg Y,
	input A, B, C, D
	
);
reg u1, u2, u3, u4;

always @(*) begin
	
	u1 = ~D;
	u4 = B & C & u1;
	u3 = A | D;
	u2 = ~u3;
	Y = u2 & u4;

end

endmodule