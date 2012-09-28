//4I No.36//

module seg_dec(ans,segout);
	input[3:0]	ans;
	output[7:0]	segout;

	assign segout = dec(ans);

	function[7:0]	dec;
		input[3:0]	ans;

		case(ans)
			4'b0000:	dec = 8'b11000000;	//00H
			4'b0001:	dec = 8'b11111001;	//01H
			4'b0010:	dec = 8'b10100100;	//02H
			4'b0011:	dec = 8'b10110000;	//03H
			4'b0100:	dec = 8'b10011001;	//04H
			4'b0101:	dec = 8'b10010010;	//05H
			4'b0110:	dec = 8'b10000010;	//06H
			4'b0111:	dec = 8'b11111000;	//07H
			4'b1000:	dec = 8'b10000000;	//08H
			4'b1001:	dec = 8'b10011000;	//09H
			4'b1010:	dec = 8'b10001000;	//0AH
			4'b1011:	dec = 8'b10000011;	//0BH
			4'b1100:	dec = 8'b10100111;	//0CH
			4'b1101:	dec = 8'b10100001;	//0DH
			4'b1110:	dec = 8'b10000110;	//0EH
			4'b1111:	dec = 8'b10001110;	//0FH
			default:	dec = 8'b11111111;
		endcase

	endfunction
endmodule