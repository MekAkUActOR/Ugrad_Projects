//File: tb_sram.v

`timescale 1 ns / 1 ns
`include "sram.v"

module tb_sram;

	wire [7:0] p_dout;

	reg [7:0] p_din;
	reg [7:0] p_addr;
	reg p_wr, p_rd, p_cs;

	sram sram1(
		.dout(p_dout),
		.din (p_din),
		.addr(p_addr),
		.wr  (p_wr),
		.rd  (p_rd),
		.cs  (p_cs)
		);

	initial begin 
		$dumpfile("sram.vcd");
		$dumpvars(0, tb_sram);

		p_addr = 8'b0;
		repeat(200) begin 
			p_din = p_addr;
			#10 p_addr = p_addr + 1'b1;
		end

		p_addr = 8'b0;
		p_din = 8'b1111_1111;
		repeat(256) begin 
			#10 p_addr = p_addr + 1'b1;
		end
	end

	initial begin 
		p_cs = 1'b0;
		#100 p_cs = 1'b1;
	end

	initial begin 
		p_wr = 1'b0;
		forever begin 
			#5 p_wr = ~p_wr;
		end
	end

	initial begin 
		p_rd = 1'b1;
		#2000 p_rd = 1'b0;

		#2600 $stop;
	end

	initial begin 
		$monitor("At time %tns, cs = %b, rd = %b, wr = %b, addr = %b, din = %b, dout = %b", $time, p_cs, p_rd, p_wr, p_addr, p_din, p_dout);
	end

endmodule