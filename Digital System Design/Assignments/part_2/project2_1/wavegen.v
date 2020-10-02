//File: wavegen.v
//517021911065 刘浩文

`timescale 10 ns / 1 ns

module wavegen;

	reg wave;

	initial
	begin 
		$dumpfile("wavegen.vcd");
		$dumpvars(0, wavegen);
	end

	initial
	fork
		wave = 1'b0;
		#2	wave = ~wave;
		#3	wave = ~wave;
		#12	wave = ~wave;
		#22	wave = ~wave;
		#24 wave = ~wave;
		#27 wave = ~wave;
		#32 wave = ~wave;
		#35 $stop;
	join

endmodule