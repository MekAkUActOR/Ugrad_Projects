//File: test.v

`timescale 1 ns / 1 ns

//module test(output reg [3:0] out1, out2,
//	output [3:0] outw,
//  input [1:0] in1, in2);
module test(out1, out2, outw, in1, in2);

	output reg [3:0] out1, out2;
	output [3:0] outw;
	input [1:0] in1, in2;

	wire temp1, temp2, temp3, temp4;
	reg tempr1, tempr2, tempr3, tempr4;

	assign 	{temp1, temp2} = in1,
			{temp3, temp4} = in2;

	always @* begin
		out1 <= {temp1, temp2, temp3, temp4};
		{tempr1, tempr2, tempr3, tempr4} <= {in1, in2};
		out2 <= {in1, in2};
	end

	assign outw = {tempr1, tempr2, tempr3, tempr4};

endmodule