//4I No.36//

module calculator(num1,num2,select,seg_n1,seg_n2,seg_h,seg_l);
	input[3:0]	num1,num2;
	input[3:0]	select;
	output[7:0]	seg_n1,seg_n2;
	output[7:0]	seg_h,seg_l;

	//モジュール間接続信号
	wire[3:0]	bcd_n1,bcd_n2,bcd_h,bcd_l;

	//下位モジュール呼び出し

	cal cal(num1,num2,select,bcd_h,bcd_l,bcd_n1,bcd_n2);

	seg_dec seg_3(bcd_n1,seg_n1);
	seg_dec seg_2(bcd_n2,seg_n2);
	seg_dec seg_1(bcd_h,seg_h);
	seg_dec seg_0(bcd_l,seg_l);

endmodule