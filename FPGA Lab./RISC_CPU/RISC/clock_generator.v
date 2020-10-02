/* File: clock_generator.v */
`timescale 1ns / 100ps

module clock_generator (
	output clk_ctrl,
	output reg clk_fetch, clk_alu,
	input clock, reset
	
);

reg [7:0] state;
parameter	idle	=	8'b0000_0000,
			cycle0	=	8'b0000_0001,
			cycle1	=	8'b0000_0010,
			cycle2	=	8'b0000_0100,
			cycle3	=	8'b0000_1000,
			cycle4	=	8'b0001_0000,
			cycle5	=	8'b0010_0000,
			cycle6	=	8'b0100_0000,
			cycle7	=	8'b1000_0000;

assign clk_ctrl = ~clock;	// 总控时钟
always @(negedge clock) begin : proc_state
	if(reset) begin
		clk_fetch <= 0;	// 控制CPU启动，取指或取数据
		clk_alu <= 0;	// 控制
		state <= idle;
	end else
		case (state)
			idle	:	state <= cycle0;
			cycle0	:	begin clk_alu <= ~clk_alu; state <= cycle1; end
			cycle1	:	begin clk_alu <= ~clk_alu; state <= cycle2; end
			cycle2	:	state <= cycle3;
			cycle3	:	begin clk_fetch <= ~clk_fetch; state <= cycle4; end
			cycle4	:	state <= cycle5;
			cycle5	:	state <= cycle6;
			cycle6	:	state <= cycle7;
			cycle7	:	begin clk_fetch <= ~clk_fetch; state <= cycle0; end
			default	: 	state <= idle;
		endcase
end

endmodule
