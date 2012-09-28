//4I No.36//

module cal(num1,num2,select,ans_h,ans_l,num1_seg,num2_seg);
	input[3:0]	num1,num2;
	input[3:0]	select;
	output[3:0]	ans_h,ans_l;
	output[3:0]	num1_seg,num2_seg;

	assign num1_seg = num1;
	assign num2_seg = num2;

	assign {ans_h,ans_l} = cal(num1,num2,select);

	function[7:0]	cal;
		input[3:0]	num1,num2;
		input[3:0]	select;

		if(select == 4'b1110)
			cal[7:0] = num1 + num2;

		else if(select == 4'b1101)
		begin
			if(num1 >= num2)
				cal[7:0] = num1 - num2;
			else if(num1 < num2)
				cal[7:0] = 0;
		end

		else if(select == 4'b1011)
			cal[7:0] = num1 * num2;

		else if(select == 4'b0111)
		begin
			if(num1 >= num2)
				cal[7:0] = num1 / num2;
			else if(num1 < num2)
				cal[7:0] = 0;
		end

	endfunction
endmodule
		