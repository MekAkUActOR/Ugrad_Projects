/* File: PC.v */
`timescale 1ns / 100ps

module PC (
	output reg [12:0] addr_pc,
	input inc_pc,
	input clk_ctrl,
	input reset,
	input load_pc,
	input [12:0] addr_ir
	
);

reg flag;

//always @(posedge inc_pc or posedge reset) begin : proc_addr_pc
always @(posedge inc_pc or negedge flag) begin
	if (!flag) addr_pc <= 13'b0;
	else
	if (load_pc) begin	// load_pc信号为真，载入指定的地址（JUMP）
		addr_pc <= addr_ir;
		//$display("pc %b", addr_ir);
	end
	else begin	// 一般情况下pc自增
		addr_pc <= addr_pc + 1'b1;
		//$display("pc %b", addr_pc + 1'b1);
	end
end

always @(negedge clk_ctrl)	// 引入时钟信号，使其能同步复位
	if(reset) begin
		flag <= 1'b0;
		//$display("pc %b", 13'b0);
	end else flag <= 1'b1;

/*
always @(addr_pc)
	$display("pc %b", addr_pc);
*/
endmodule