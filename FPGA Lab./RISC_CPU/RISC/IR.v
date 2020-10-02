/* File: IR.v */
`timescale 1ns / 100ps

module IR (
	output reg [2:0] opcode,
	output reg [12:0] addr_ir,
	input clk_ctrl,
	input reset,
	input load_ir,
	input [7:0] data_bus
	
);

reg state;

always @(posedge clk_ctrl) begin : proc_state
	if(reset) begin
		opcode <= 3'b0;
		addr_ir <= 13'b0;
		state <= 1'b0;
	end
	else
		if (load_ir)
			casex(state)
				1'b0:	begin	// 取指令高8位
							opcode <= data_bus[7:5];
							addr_ir[12:8] <= data_bus[4:0];
							state <= 1'b1;
							//$display("ir state0 %b %b", data_bus[7:5], data_bus[4:0]);
						end
				1'b1:	begin	// 取指令低8位
							addr_ir[7:0] <= data_bus;
							state <= 1'b0;
							//$display("ir state1 %b", data_bus);
						end
				default:
						begin
							opcode <= 3'bx;
							addr_ir <= 13'bx;
							state <= 1'bx;
							//$display("ir statex %b %b", 3'bx, 13'bx);
						end
			endcase
		else state <= 1'b0;
end

endmodule
