//File: filter.v

`timescale 1 ns / 1 ns

module filter (
	output reg sig_out,
	input clock,    // Clock
	input reset,  // Asynchronous reset active low
	input sig_in
	
);
reg [3:0] rgt;

reg j, k;

always @(posedge clock or negedge reset) begin
	if(~reset) begin
		sig_out <= 1'b0;
		rgt <= 4'b0;
	end else begin
		rgt[3] <= sig_in;
		rgt[2] <= rgt[3];
		rgt[1] <= rgt[2];
		rgt[0] <= rgt[1];
		k <= ~rgt[2] & ~rgt[1] & ~rgt[0];
		j <= rgt[2] & rgt[1] & rgt[0];

		case ({j,k})
			2'b01:	sig_out <= 1'b0;
			2'b10:	sig_out <= 1'b1;
			2'b11:	sig_out <= ~sig_out;
		
			default : /* default */;
		endcase
		//$display("rgt = %b, sig_in = %b, sig_out = %b.", rgt, sig_in, sig_out);
	end
end

endmodule