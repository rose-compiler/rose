int main(int argc, char* argv[]) {
//	#pragma p5_reachable begin infeasible
//	#pragma p6_unreachable begin infeasible
	char c = 'a';
	signed char s_c = 'a';
	unsigned char u_c == 'a';
	if (c == s_c && s_c == u_c && c == u_c) {
	#pragma p9_reachable end
	}
	else {
	#pragma p10_unreachable end
	} 
	unsigned i_us = 23;
	int i_num = 23;
	signed int si_num = 23; 
	unsigned int ui_num = 23;
	unsigned u_num = 23;
	if (i_num == si_num 
	&& i_num == ui_num 
	&& i_num == i_us
	&& i_num == u_num
	&& si_num == ui_num 
	&& si_num == i_us 
	&& si_num == u_num
	&& ui_num == i_us
	&& ui_num == u_num
	&& i_us == u_num) {
	#pragma p1_reachable end
	}
	else {
	#pragma p2_unreachable end
	}
	short short_num = 12;
	short int short_int_num = 12;
	unsigned short us_short_num = 12;
	unsigned short int u_short_int = 12;
	signed short ss_short_num = 12;
	signed short int s_short_int = 12;
	if (short_num == short_int_num
	&& short_num == u_short_int
	&& short_num == us_short_int
	&& short_num == ss_short_num
	&& short_num == s_short_int
	&& short_int_num == u_short_int
	&& short_int_num == us_short_int
	&& short_int_num == ss_short_int
	&& short_int_num == s_short_int
	&& u_short_int == us_short_int
	&& u_short_int == ss_short_num
	&& u_short_int == s_short_num
	&& us_short_int == ss_short_num
	&& us_short_int == s_short_num
	&& ss_short_int == s_short_num) {
	
	#pragma p3_reachable end
	}
	else {
	#pragma p4_unreachable end
	}
	long n_l = 4000l;
	long int n_li = 4000l;
	signed long n_sl = 4000l;
	signed long int n_sli = 4000l;
	unsigned long int n_uli = 4000l;
	long long n_ll = 4000l;
	long long int n_lli = 4000l;
	signed long long n_sll = 4000l;
	signed long long int n_slli = 4000l;
	unsigned long long n_ull = 4000l;
	unsigned long long int n_ulli = 4000l;
		


        float float_num = 2.0;
        //unsigned float us_float_num = 2.0;
        //signed float s_float_num = 2.0;
	/*if (float_num == us_float_num && us_float_num == s_float_num && float_num == s_float_num) {
	#pragma p5_reachable end
	}
	else {
	#pragma p6_unreachable end
	}*/
	double double_num = 2.0111;
	unsigned double us_double_num = 2.0111;
	signed double s_double_num = 2.0111;
	if (double_num == us_double_num && us_double_num == s_double_num && s_double_num == double_num) {
	#pragma p7_reachable end
	}
	else {
	#pragma p8_unreachable end
	}
	
	return 0;
}
