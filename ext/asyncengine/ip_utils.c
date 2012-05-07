
#line 1 "ip_utils.rl"
#include "ip_utils.h"


/** machine **/

#line 44 "ip_utils.rl"



/** Data **/

#line 15 "ip_utils.c"
static const int ip_utils_parser_start = 1;
static const int ip_utils_parser_first_final = 228;
static const int ip_utils_parser_error = 0;

static const int ip_utils_parser_en_main = 1;


#line 49 "ip_utils.rl"


/** Exec **/
enum enum_ip_type ip_utils_parser_execute(const char *str, size_t len)
{
  int cs = 0;
  const char *p, *pe;
  enum enum_ip_type ip_type = ip_type_error;

  p = str;
  pe = str+len;

  
#line 37 "ip_utils.c"
	{
	cs = ip_utils_parser_start;
	}

#line 62 "ip_utils.rl"
  
#line 44 "ip_utils.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	switch( (*p) ) {
		case 50: goto st77;
		case 58: goto st81;
		case 91: goto st84;
	}
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto st2;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st83;
		} else if ( (*p) >= 65 )
			goto st83;
	} else
		goto st80;
	goto st0;
st0:
cs = 0;
	goto _out;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	switch( (*p) ) {
		case 46: goto st3;
		case 58: goto st19;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st16;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st76;
	} else
		goto st76;
	goto st0;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	if ( (*p) == 50 )
		goto st14;
	if ( (*p) > 49 ) {
		if ( 51 <= (*p) && (*p) <= 57 )
			goto st12;
	} else if ( (*p) >= 48 )
		goto st4;
	goto st0;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	if ( (*p) == 46 )
		goto st5;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st12;
	goto st0;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( (*p) == 50 )
		goto st10;
	if ( (*p) > 49 ) {
		if ( 51 <= (*p) && (*p) <= 57 )
			goto st8;
	} else if ( (*p) >= 48 )
		goto st6;
	goto st0;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	if ( (*p) == 46 )
		goto st7;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st8;
	goto st0;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	if ( (*p) == 50 )
		goto tr20;
	if ( (*p) > 49 ) {
		if ( 51 <= (*p) && (*p) <= 57 )
			goto tr21;
	} else if ( (*p) >= 48 )
		goto tr19;
	goto st0;
tr19:
#line 29 "ip_utils.rl"
	{
    ip_type = ip_type_ipv4;
  }
	goto st228;
st228:
	if ( ++p == pe )
		goto _test_eof228;
case 228:
#line 152 "ip_utils.c"
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr21;
	goto st0;
tr21:
#line 29 "ip_utils.rl"
	{
    ip_type = ip_type_ipv4;
  }
	goto st229;
st229:
	if ( ++p == pe )
		goto _test_eof229;
case 229:
#line 166 "ip_utils.c"
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr257;
	goto st0;
tr257:
#line 29 "ip_utils.rl"
	{
    ip_type = ip_type_ipv4;
  }
	goto st230;
tr87:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st230;
tr169:
#line 37 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6_reference;
  }
	goto st230;
st230:
	if ( ++p == pe )
		goto _test_eof230;
case 230:
#line 192 "ip_utils.c"
	goto st0;
tr20:
#line 29 "ip_utils.rl"
	{
    ip_type = ip_type_ipv4;
  }
	goto st231;
st231:
	if ( ++p == pe )
		goto _test_eof231;
case 231:
#line 204 "ip_utils.c"
	if ( (*p) == 53 )
		goto tr258;
	if ( (*p) > 52 ) {
		if ( 54 <= (*p) && (*p) <= 57 )
			goto tr257;
	} else if ( (*p) >= 48 )
		goto tr21;
	goto st0;
tr258:
#line 29 "ip_utils.rl"
	{
    ip_type = ip_type_ipv4;
  }
	goto st232;
st232:
	if ( ++p == pe )
		goto _test_eof232;
case 232:
#line 223 "ip_utils.c"
	if ( 48 <= (*p) && (*p) <= 53 )
		goto tr257;
	goto st0;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	if ( (*p) == 46 )
		goto st7;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st9;
	goto st0;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
	if ( (*p) == 46 )
		goto st7;
	goto st0;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	switch( (*p) ) {
		case 46: goto st7;
		case 53: goto st11;
	}
	if ( (*p) > 52 ) {
		if ( 54 <= (*p) && (*p) <= 57 )
			goto st9;
	} else if ( (*p) >= 48 )
		goto st8;
	goto st0;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
	if ( (*p) == 46 )
		goto st7;
	if ( 48 <= (*p) && (*p) <= 53 )
		goto st9;
	goto st0;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
	if ( (*p) == 46 )
		goto st5;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st13;
	goto st0;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
	if ( (*p) == 46 )
		goto st5;
	goto st0;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
	switch( (*p) ) {
		case 46: goto st5;
		case 53: goto st15;
	}
	if ( (*p) > 52 ) {
		if ( 54 <= (*p) && (*p) <= 57 )
			goto st13;
	} else if ( (*p) >= 48 )
		goto st12;
	goto st0;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
	if ( (*p) == 46 )
		goto st5;
	if ( 48 <= (*p) && (*p) <= 53 )
		goto st13;
	goto st0;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
	switch( (*p) ) {
		case 46: goto st3;
		case 58: goto st19;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st17;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st75;
	} else
		goto st75;
	goto st0;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
	switch( (*p) ) {
		case 46: goto st3;
		case 58: goto st19;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st18;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st18;
	} else
		goto st18;
	goto st0;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
	if ( (*p) == 58 )
		goto st19;
	goto st0;
st19:
	if ( ++p == pe )
		goto _test_eof19;
case 19:
	if ( (*p) == 58 )
		goto tr30;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st20;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st20;
	} else
		goto st20;
	goto st0;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
	if ( (*p) == 58 )
		goto st24;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st21;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st21;
	} else
		goto st21;
	goto st0;
st21:
	if ( ++p == pe )
		goto _test_eof21;
case 21:
	if ( (*p) == 58 )
		goto st24;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st22;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st22;
	} else
		goto st22;
	goto st0;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
	if ( (*p) == 58 )
		goto st24;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st23;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st23;
	} else
		goto st23;
	goto st0;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
	if ( (*p) == 58 )
		goto st24;
	goto st0;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
	if ( (*p) == 58 )
		goto tr36;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st25;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st25;
	} else
		goto st25;
	goto st0;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
	if ( (*p) == 58 )
		goto st29;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st26;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st26;
	} else
		goto st26;
	goto st0;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
	if ( (*p) == 58 )
		goto st29;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st27;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st27;
	} else
		goto st27;
	goto st0;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
	if ( (*p) == 58 )
		goto st29;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st28;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st28;
	} else
		goto st28;
	goto st0;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
	if ( (*p) == 58 )
		goto st29;
	goto st0;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
	if ( (*p) == 58 )
		goto tr42;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st30;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st30;
	} else
		goto st30;
	goto st0;
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
	if ( (*p) == 58 )
		goto st34;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st31;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st31;
	} else
		goto st31;
	goto st0;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
	if ( (*p) == 58 )
		goto st34;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st32;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st32;
	} else
		goto st32;
	goto st0;
st32:
	if ( ++p == pe )
		goto _test_eof32;
case 32:
	if ( (*p) == 58 )
		goto st34;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st33;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st33;
	} else
		goto st33;
	goto st0;
st33:
	if ( ++p == pe )
		goto _test_eof33;
case 33:
	if ( (*p) == 58 )
		goto st34;
	goto st0;
st34:
	if ( ++p == pe )
		goto _test_eof34;
case 34:
	if ( (*p) == 58 )
		goto tr48;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st35;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st35;
	} else
		goto st35;
	goto st0;
st35:
	if ( ++p == pe )
		goto _test_eof35;
case 35:
	if ( (*p) == 58 )
		goto st39;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st36;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st36;
	} else
		goto st36;
	goto st0;
st36:
	if ( ++p == pe )
		goto _test_eof36;
case 36:
	if ( (*p) == 58 )
		goto st39;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st37;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st37;
	} else
		goto st37;
	goto st0;
st37:
	if ( ++p == pe )
		goto _test_eof37;
case 37:
	if ( (*p) == 58 )
		goto st39;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st38;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st38;
	} else
		goto st38;
	goto st0;
st38:
	if ( ++p == pe )
		goto _test_eof38;
case 38:
	if ( (*p) == 58 )
		goto st39;
	goto st0;
st39:
	if ( ++p == pe )
		goto _test_eof39;
case 39:
	if ( (*p) == 58 )
		goto tr54;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st40;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st40;
	} else
		goto st40;
	goto st0;
st40:
	if ( ++p == pe )
		goto _test_eof40;
case 40:
	if ( (*p) == 58 )
		goto st44;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st41;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st41;
	} else
		goto st41;
	goto st0;
st41:
	if ( ++p == pe )
		goto _test_eof41;
case 41:
	if ( (*p) == 58 )
		goto st44;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st42;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st42;
	} else
		goto st42;
	goto st0;
st42:
	if ( ++p == pe )
		goto _test_eof42;
case 42:
	if ( (*p) == 58 )
		goto st44;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st43;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st43;
	} else
		goto st43;
	goto st0;
st43:
	if ( ++p == pe )
		goto _test_eof43;
case 43:
	if ( (*p) == 58 )
		goto st44;
	goto st0;
st44:
	if ( ++p == pe )
		goto _test_eof44;
case 44:
	switch( (*p) ) {
		case 50: goto st65;
		case 58: goto tr62;
	}
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto st45;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st69;
		} else if ( (*p) >= 65 )
			goto st69;
	} else
		goto st68;
	goto st0;
st45:
	if ( ++p == pe )
		goto _test_eof45;
case 45:
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st62;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st59;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st64;
	} else
		goto st64;
	goto st0;
st46:
	if ( ++p == pe )
		goto _test_eof46;
case 46:
	if ( (*p) == 50 )
		goto st57;
	if ( (*p) > 49 ) {
		if ( 51 <= (*p) && (*p) <= 57 )
			goto st55;
	} else if ( (*p) >= 48 )
		goto st47;
	goto st0;
st47:
	if ( ++p == pe )
		goto _test_eof47;
case 47:
	if ( (*p) == 46 )
		goto st48;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st55;
	goto st0;
st48:
	if ( ++p == pe )
		goto _test_eof48;
case 48:
	if ( (*p) == 50 )
		goto st53;
	if ( (*p) > 49 ) {
		if ( 51 <= (*p) && (*p) <= 57 )
			goto st51;
	} else if ( (*p) >= 48 )
		goto st49;
	goto st0;
st49:
	if ( ++p == pe )
		goto _test_eof49;
case 49:
	if ( (*p) == 46 )
		goto st50;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st51;
	goto st0;
st50:
	if ( ++p == pe )
		goto _test_eof50;
case 50:
	if ( (*p) == 50 )
		goto tr77;
	if ( (*p) > 49 ) {
		if ( 51 <= (*p) && (*p) <= 57 )
			goto tr78;
	} else if ( (*p) >= 48 )
		goto tr76;
	goto st0;
tr76:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st233;
st233:
	if ( ++p == pe )
		goto _test_eof233;
case 233:
#line 782 "ip_utils.c"
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr78;
	goto st0;
tr78:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st234;
st234:
	if ( ++p == pe )
		goto _test_eof234;
case 234:
#line 796 "ip_utils.c"
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr87;
	goto st0;
tr77:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st235;
st235:
	if ( ++p == pe )
		goto _test_eof235;
case 235:
#line 810 "ip_utils.c"
	if ( (*p) == 53 )
		goto tr259;
	if ( (*p) > 52 ) {
		if ( 54 <= (*p) && (*p) <= 57 )
			goto tr87;
	} else if ( (*p) >= 48 )
		goto tr78;
	goto st0;
tr259:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st236;
st236:
	if ( ++p == pe )
		goto _test_eof236;
case 236:
#line 829 "ip_utils.c"
	if ( 48 <= (*p) && (*p) <= 53 )
		goto tr87;
	goto st0;
st51:
	if ( ++p == pe )
		goto _test_eof51;
case 51:
	if ( (*p) == 46 )
		goto st50;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st52;
	goto st0;
st52:
	if ( ++p == pe )
		goto _test_eof52;
case 52:
	if ( (*p) == 46 )
		goto st50;
	goto st0;
st53:
	if ( ++p == pe )
		goto _test_eof53;
case 53:
	switch( (*p) ) {
		case 46: goto st50;
		case 53: goto st54;
	}
	if ( (*p) > 52 ) {
		if ( 54 <= (*p) && (*p) <= 57 )
			goto st52;
	} else if ( (*p) >= 48 )
		goto st51;
	goto st0;
st54:
	if ( ++p == pe )
		goto _test_eof54;
case 54:
	if ( (*p) == 46 )
		goto st50;
	if ( 48 <= (*p) && (*p) <= 53 )
		goto st52;
	goto st0;
st55:
	if ( ++p == pe )
		goto _test_eof55;
case 55:
	if ( (*p) == 46 )
		goto st48;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st56;
	goto st0;
st56:
	if ( ++p == pe )
		goto _test_eof56;
case 56:
	if ( (*p) == 46 )
		goto st48;
	goto st0;
st57:
	if ( ++p == pe )
		goto _test_eof57;
case 57:
	switch( (*p) ) {
		case 46: goto st48;
		case 53: goto st58;
	}
	if ( (*p) > 52 ) {
		if ( 54 <= (*p) && (*p) <= 57 )
			goto st56;
	} else if ( (*p) >= 48 )
		goto st55;
	goto st0;
st58:
	if ( ++p == pe )
		goto _test_eof58;
case 58:
	if ( (*p) == 46 )
		goto st48;
	if ( 48 <= (*p) && (*p) <= 53 )
		goto st56;
	goto st0;
st59:
	if ( ++p == pe )
		goto _test_eof59;
case 59:
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st62;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st60;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st63;
	} else
		goto st63;
	goto st0;
st60:
	if ( ++p == pe )
		goto _test_eof60;
case 60:
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st62;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st61;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st61;
	} else
		goto st61;
	goto st0;
st61:
	if ( ++p == pe )
		goto _test_eof61;
case 61:
	if ( (*p) == 58 )
		goto st62;
	goto st0;
st62:
	if ( ++p == pe )
		goto _test_eof62;
case 62:
	if ( (*p) == 58 )
		goto tr87;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr86;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr86;
	} else
		goto tr86;
	goto st0;
tr86:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st237;
st237:
	if ( ++p == pe )
		goto _test_eof237;
case 237:
#line 977 "ip_utils.c"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr260;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr260;
	} else
		goto tr260;
	goto st0;
tr260:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st238;
st238:
	if ( ++p == pe )
		goto _test_eof238;
case 238:
#line 997 "ip_utils.c"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr261;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr261;
	} else
		goto tr261;
	goto st0;
tr261:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st239;
st239:
	if ( ++p == pe )
		goto _test_eof239;
case 239:
#line 1017 "ip_utils.c"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr87;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr87;
	} else
		goto tr87;
	goto st0;
st63:
	if ( ++p == pe )
		goto _test_eof63;
case 63:
	if ( (*p) == 58 )
		goto st62;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st61;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st61;
	} else
		goto st61;
	goto st0;
st64:
	if ( ++p == pe )
		goto _test_eof64;
case 64:
	if ( (*p) == 58 )
		goto st62;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st63;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st63;
	} else
		goto st63;
	goto st0;
st65:
	if ( ++p == pe )
		goto _test_eof65;
case 65:
	switch( (*p) ) {
		case 46: goto st46;
		case 53: goto st66;
		case 58: goto st62;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 52 )
			goto st59;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st64;
		} else if ( (*p) >= 65 )
			goto st64;
	} else
		goto st67;
	goto st0;
st66:
	if ( ++p == pe )
		goto _test_eof66;
case 66:
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st62;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 53 )
			goto st60;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st63;
		} else if ( (*p) >= 65 )
			goto st63;
	} else
		goto st63;
	goto st0;
st67:
	if ( ++p == pe )
		goto _test_eof67;
case 67:
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st62;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st63;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st63;
	} else
		goto st63;
	goto st0;
st68:
	if ( ++p == pe )
		goto _test_eof68;
case 68:
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st62;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st67;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st64;
	} else
		goto st64;
	goto st0;
tr62:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st240;
st240:
	if ( ++p == pe )
		goto _test_eof240;
case 240:
#line 1142 "ip_utils.c"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr86;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr86;
	} else
		goto tr86;
	goto st0;
st69:
	if ( ++p == pe )
		goto _test_eof69;
case 69:
	if ( (*p) == 58 )
		goto st62;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st64;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st64;
	} else
		goto st64;
	goto st0;
tr54:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st241;
st241:
	if ( ++p == pe )
		goto _test_eof241;
case 241:
#line 1177 "ip_utils.c"
	if ( (*p) == 50 )
		goto tr91;
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto tr90;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto tr93;
		} else if ( (*p) >= 65 )
			goto tr93;
	} else
		goto tr92;
	goto st0;
tr90:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st242;
st242:
	if ( ++p == pe )
		goto _test_eof242;
case 242:
#line 1202 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st70;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr262;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr264;
	} else
		goto tr264;
	goto st0;
tr262:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st243;
st243:
	if ( ++p == pe )
		goto _test_eof243;
case 243:
#line 1226 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st70;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr265;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr266;
	} else
		goto tr266;
	goto st0;
tr265:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st244;
st244:
	if ( ++p == pe )
		goto _test_eof244;
case 244:
#line 1250 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st70;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr267;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr267;
	} else
		goto tr267;
	goto st0;
tr267:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st245;
st245:
	if ( ++p == pe )
		goto _test_eof245;
case 245:
#line 1274 "ip_utils.c"
	if ( (*p) == 58 )
		goto st70;
	goto st0;
st70:
	if ( ++p == pe )
		goto _test_eof70;
case 70:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr86;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr86;
	} else
		goto tr86;
	goto st0;
tr266:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st246;
st246:
	if ( ++p == pe )
		goto _test_eof246;
case 246:
#line 1301 "ip_utils.c"
	if ( (*p) == 58 )
		goto st70;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr267;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr267;
	} else
		goto tr267;
	goto st0;
tr264:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st247;
st247:
	if ( ++p == pe )
		goto _test_eof247;
case 247:
#line 1323 "ip_utils.c"
	if ( (*p) == 58 )
		goto st70;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr266;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr266;
	} else
		goto tr266;
	goto st0;
tr91:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st248;
st248:
	if ( ++p == pe )
		goto _test_eof248;
case 248:
#line 1345 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 53: goto tr268;
		case 58: goto st70;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 52 )
			goto tr262;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto tr264;
		} else if ( (*p) >= 65 )
			goto tr264;
	} else
		goto tr269;
	goto st0;
tr268:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st249;
st249:
	if ( ++p == pe )
		goto _test_eof249;
case 249:
#line 1373 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st70;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 53 )
			goto tr265;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto tr266;
		} else if ( (*p) >= 65 )
			goto tr266;
	} else
		goto tr266;
	goto st0;
tr269:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st250;
st250:
	if ( ++p == pe )
		goto _test_eof250;
case 250:
#line 1400 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st70;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr266;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr266;
	} else
		goto tr266;
	goto st0;
tr92:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st251;
st251:
	if ( ++p == pe )
		goto _test_eof251;
case 251:
#line 1424 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st70;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr269;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr264;
	} else
		goto tr264;
	goto st0;
tr93:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st252;
st252:
	if ( ++p == pe )
		goto _test_eof252;
case 252:
#line 1448 "ip_utils.c"
	if ( (*p) == 58 )
		goto st70;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr264;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr264;
	} else
		goto tr264;
	goto st0;
tr48:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st253;
st253:
	if ( ++p == pe )
		goto _test_eof253;
case 253:
#line 1470 "ip_utils.c"
	if ( (*p) == 50 )
		goto tr95;
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto tr94;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto tr97;
		} else if ( (*p) >= 65 )
			goto tr97;
	} else
		goto tr96;
	goto st0;
tr94:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st254;
st254:
	if ( ++p == pe )
		goto _test_eof254;
case 254:
#line 1495 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st71;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr270;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr272;
	} else
		goto tr272;
	goto st0;
tr270:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st255;
st255:
	if ( ++p == pe )
		goto _test_eof255;
case 255:
#line 1519 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st71;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr273;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr274;
	} else
		goto tr274;
	goto st0;
tr273:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st256;
st256:
	if ( ++p == pe )
		goto _test_eof256;
case 256:
#line 1543 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st71;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr275;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr275;
	} else
		goto tr275;
	goto st0;
tr275:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st257;
st257:
	if ( ++p == pe )
		goto _test_eof257;
case 257:
#line 1567 "ip_utils.c"
	if ( (*p) == 58 )
		goto st71;
	goto st0;
st71:
	if ( ++p == pe )
		goto _test_eof71;
case 71:
	if ( (*p) == 50 )
		goto tr91;
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto tr90;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto tr93;
		} else if ( (*p) >= 65 )
			goto tr93;
	} else
		goto tr92;
	goto st0;
tr274:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st258;
st258:
	if ( ++p == pe )
		goto _test_eof258;
case 258:
#line 1599 "ip_utils.c"
	if ( (*p) == 58 )
		goto st71;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr275;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr275;
	} else
		goto tr275;
	goto st0;
tr272:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st259;
st259:
	if ( ++p == pe )
		goto _test_eof259;
case 259:
#line 1621 "ip_utils.c"
	if ( (*p) == 58 )
		goto st71;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr274;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr274;
	} else
		goto tr274;
	goto st0;
tr95:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st260;
st260:
	if ( ++p == pe )
		goto _test_eof260;
case 260:
#line 1643 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 53: goto tr276;
		case 58: goto st71;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 52 )
			goto tr270;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto tr272;
		} else if ( (*p) >= 65 )
			goto tr272;
	} else
		goto tr277;
	goto st0;
tr276:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st261;
st261:
	if ( ++p == pe )
		goto _test_eof261;
case 261:
#line 1671 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st71;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 53 )
			goto tr273;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto tr274;
		} else if ( (*p) >= 65 )
			goto tr274;
	} else
		goto tr274;
	goto st0;
tr277:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st262;
st262:
	if ( ++p == pe )
		goto _test_eof262;
case 262:
#line 1698 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st71;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr274;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr274;
	} else
		goto tr274;
	goto st0;
tr96:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st263;
st263:
	if ( ++p == pe )
		goto _test_eof263;
case 263:
#line 1722 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st71;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr277;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr272;
	} else
		goto tr272;
	goto st0;
tr97:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st264;
st264:
	if ( ++p == pe )
		goto _test_eof264;
case 264:
#line 1746 "ip_utils.c"
	if ( (*p) == 58 )
		goto st71;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr272;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr272;
	} else
		goto tr272;
	goto st0;
tr42:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st265;
st265:
	if ( ++p == pe )
		goto _test_eof265;
case 265:
#line 1768 "ip_utils.c"
	if ( (*p) == 50 )
		goto tr99;
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto tr98;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto tr101;
		} else if ( (*p) >= 65 )
			goto tr101;
	} else
		goto tr100;
	goto st0;
tr98:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st266;
st266:
	if ( ++p == pe )
		goto _test_eof266;
case 266:
#line 1793 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st72;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr278;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr280;
	} else
		goto tr280;
	goto st0;
tr278:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st267;
st267:
	if ( ++p == pe )
		goto _test_eof267;
case 267:
#line 1817 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st72;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr281;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr282;
	} else
		goto tr282;
	goto st0;
tr281:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st268;
st268:
	if ( ++p == pe )
		goto _test_eof268;
case 268:
#line 1841 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st72;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr283;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr283;
	} else
		goto tr283;
	goto st0;
tr283:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st269;
st269:
	if ( ++p == pe )
		goto _test_eof269;
case 269:
#line 1865 "ip_utils.c"
	if ( (*p) == 58 )
		goto st72;
	goto st0;
st72:
	if ( ++p == pe )
		goto _test_eof72;
case 72:
	if ( (*p) == 50 )
		goto tr95;
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto tr94;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto tr97;
		} else if ( (*p) >= 65 )
			goto tr97;
	} else
		goto tr96;
	goto st0;
tr282:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st270;
st270:
	if ( ++p == pe )
		goto _test_eof270;
case 270:
#line 1897 "ip_utils.c"
	if ( (*p) == 58 )
		goto st72;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr283;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr283;
	} else
		goto tr283;
	goto st0;
tr280:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st271;
st271:
	if ( ++p == pe )
		goto _test_eof271;
case 271:
#line 1919 "ip_utils.c"
	if ( (*p) == 58 )
		goto st72;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr282;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr282;
	} else
		goto tr282;
	goto st0;
tr99:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st272;
st272:
	if ( ++p == pe )
		goto _test_eof272;
case 272:
#line 1941 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 53: goto tr284;
		case 58: goto st72;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 52 )
			goto tr278;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto tr280;
		} else if ( (*p) >= 65 )
			goto tr280;
	} else
		goto tr285;
	goto st0;
tr284:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st273;
st273:
	if ( ++p == pe )
		goto _test_eof273;
case 273:
#line 1969 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st72;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 53 )
			goto tr281;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto tr282;
		} else if ( (*p) >= 65 )
			goto tr282;
	} else
		goto tr282;
	goto st0;
tr285:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st274;
st274:
	if ( ++p == pe )
		goto _test_eof274;
case 274:
#line 1996 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st72;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr282;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr282;
	} else
		goto tr282;
	goto st0;
tr100:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st275;
st275:
	if ( ++p == pe )
		goto _test_eof275;
case 275:
#line 2020 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st72;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr285;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr280;
	} else
		goto tr280;
	goto st0;
tr101:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st276;
st276:
	if ( ++p == pe )
		goto _test_eof276;
case 276:
#line 2044 "ip_utils.c"
	if ( (*p) == 58 )
		goto st72;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr280;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr280;
	} else
		goto tr280;
	goto st0;
tr36:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st277;
st277:
	if ( ++p == pe )
		goto _test_eof277;
case 277:
#line 2066 "ip_utils.c"
	if ( (*p) == 50 )
		goto tr103;
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto tr102;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto tr105;
		} else if ( (*p) >= 65 )
			goto tr105;
	} else
		goto tr104;
	goto st0;
tr102:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st278;
st278:
	if ( ++p == pe )
		goto _test_eof278;
case 278:
#line 2091 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st73;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr286;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr288;
	} else
		goto tr288;
	goto st0;
tr286:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st279;
st279:
	if ( ++p == pe )
		goto _test_eof279;
case 279:
#line 2115 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st73;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr289;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr290;
	} else
		goto tr290;
	goto st0;
tr289:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st280;
st280:
	if ( ++p == pe )
		goto _test_eof280;
case 280:
#line 2139 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st73;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr291;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr291;
	} else
		goto tr291;
	goto st0;
tr291:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st281;
st281:
	if ( ++p == pe )
		goto _test_eof281;
case 281:
#line 2163 "ip_utils.c"
	if ( (*p) == 58 )
		goto st73;
	goto st0;
st73:
	if ( ++p == pe )
		goto _test_eof73;
case 73:
	if ( (*p) == 50 )
		goto tr99;
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto tr98;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto tr101;
		} else if ( (*p) >= 65 )
			goto tr101;
	} else
		goto tr100;
	goto st0;
tr290:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st282;
st282:
	if ( ++p == pe )
		goto _test_eof282;
case 282:
#line 2195 "ip_utils.c"
	if ( (*p) == 58 )
		goto st73;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr291;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr291;
	} else
		goto tr291;
	goto st0;
tr288:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st283;
st283:
	if ( ++p == pe )
		goto _test_eof283;
case 283:
#line 2217 "ip_utils.c"
	if ( (*p) == 58 )
		goto st73;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr290;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr290;
	} else
		goto tr290;
	goto st0;
tr103:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st284;
st284:
	if ( ++p == pe )
		goto _test_eof284;
case 284:
#line 2239 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 53: goto tr292;
		case 58: goto st73;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 52 )
			goto tr286;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto tr288;
		} else if ( (*p) >= 65 )
			goto tr288;
	} else
		goto tr293;
	goto st0;
tr292:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st285;
st285:
	if ( ++p == pe )
		goto _test_eof285;
case 285:
#line 2267 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st73;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 53 )
			goto tr289;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto tr290;
		} else if ( (*p) >= 65 )
			goto tr290;
	} else
		goto tr290;
	goto st0;
tr293:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st286;
st286:
	if ( ++p == pe )
		goto _test_eof286;
case 286:
#line 2294 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st73;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr290;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr290;
	} else
		goto tr290;
	goto st0;
tr104:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st287;
st287:
	if ( ++p == pe )
		goto _test_eof287;
case 287:
#line 2318 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st73;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr293;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr288;
	} else
		goto tr288;
	goto st0;
tr105:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st288;
st288:
	if ( ++p == pe )
		goto _test_eof288;
case 288:
#line 2342 "ip_utils.c"
	if ( (*p) == 58 )
		goto st73;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr288;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr288;
	} else
		goto tr288;
	goto st0;
tr30:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st289;
st289:
	if ( ++p == pe )
		goto _test_eof289;
case 289:
#line 2364 "ip_utils.c"
	if ( (*p) == 50 )
		goto tr110;
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto tr109;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto tr112;
		} else if ( (*p) >= 65 )
			goto tr112;
	} else
		goto tr111;
	goto st0;
tr109:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st290;
st290:
	if ( ++p == pe )
		goto _test_eof290;
case 290:
#line 2389 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st74;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr294;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr296;
	} else
		goto tr296;
	goto st0;
tr294:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st291;
st291:
	if ( ++p == pe )
		goto _test_eof291;
case 291:
#line 2413 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st74;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr297;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr298;
	} else
		goto tr298;
	goto st0;
tr297:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st292;
st292:
	if ( ++p == pe )
		goto _test_eof292;
case 292:
#line 2437 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st74;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr299;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr299;
	} else
		goto tr299;
	goto st0;
tr299:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st293;
st293:
	if ( ++p == pe )
		goto _test_eof293;
case 293:
#line 2461 "ip_utils.c"
	if ( (*p) == 58 )
		goto st74;
	goto st0;
st74:
	if ( ++p == pe )
		goto _test_eof74;
case 74:
	if ( (*p) == 50 )
		goto tr103;
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto tr102;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto tr105;
		} else if ( (*p) >= 65 )
			goto tr105;
	} else
		goto tr104;
	goto st0;
tr298:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st294;
st294:
	if ( ++p == pe )
		goto _test_eof294;
case 294:
#line 2493 "ip_utils.c"
	if ( (*p) == 58 )
		goto st74;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr299;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr299;
	} else
		goto tr299;
	goto st0;
tr296:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st295;
st295:
	if ( ++p == pe )
		goto _test_eof295;
case 295:
#line 2515 "ip_utils.c"
	if ( (*p) == 58 )
		goto st74;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr298;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr298;
	} else
		goto tr298;
	goto st0;
tr110:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st296;
st296:
	if ( ++p == pe )
		goto _test_eof296;
case 296:
#line 2537 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 53: goto tr300;
		case 58: goto st74;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 52 )
			goto tr294;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto tr296;
		} else if ( (*p) >= 65 )
			goto tr296;
	} else
		goto tr301;
	goto st0;
tr300:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st297;
st297:
	if ( ++p == pe )
		goto _test_eof297;
case 297:
#line 2565 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st74;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 53 )
			goto tr297;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto tr298;
		} else if ( (*p) >= 65 )
			goto tr298;
	} else
		goto tr298;
	goto st0;
tr301:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st298;
st298:
	if ( ++p == pe )
		goto _test_eof298;
case 298:
#line 2592 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st74;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr298;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr298;
	} else
		goto tr298;
	goto st0;
tr111:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st299;
st299:
	if ( ++p == pe )
		goto _test_eof299;
case 299:
#line 2616 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st74;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr301;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr296;
	} else
		goto tr296;
	goto st0;
tr112:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st300;
st300:
	if ( ++p == pe )
		goto _test_eof300;
case 300:
#line 2640 "ip_utils.c"
	if ( (*p) == 58 )
		goto st74;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr296;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr296;
	} else
		goto tr296;
	goto st0;
st75:
	if ( ++p == pe )
		goto _test_eof75;
case 75:
	if ( (*p) == 58 )
		goto st19;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st18;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st18;
	} else
		goto st18;
	goto st0;
st76:
	if ( ++p == pe )
		goto _test_eof76;
case 76:
	if ( (*p) == 58 )
		goto st19;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st75;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st75;
	} else
		goto st75;
	goto st0;
st77:
	if ( ++p == pe )
		goto _test_eof77;
case 77:
	switch( (*p) ) {
		case 46: goto st3;
		case 53: goto st78;
		case 58: goto st19;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 52 )
			goto st16;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st76;
		} else if ( (*p) >= 65 )
			goto st76;
	} else
		goto st79;
	goto st0;
st78:
	if ( ++p == pe )
		goto _test_eof78;
case 78:
	switch( (*p) ) {
		case 46: goto st3;
		case 58: goto st19;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 53 )
			goto st17;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st75;
		} else if ( (*p) >= 65 )
			goto st75;
	} else
		goto st75;
	goto st0;
st79:
	if ( ++p == pe )
		goto _test_eof79;
case 79:
	switch( (*p) ) {
		case 46: goto st3;
		case 58: goto st19;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st75;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st75;
	} else
		goto st75;
	goto st0;
st80:
	if ( ++p == pe )
		goto _test_eof80;
case 80:
	switch( (*p) ) {
		case 46: goto st3;
		case 58: goto st19;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st79;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st76;
	} else
		goto st76;
	goto st0;
st81:
	if ( ++p == pe )
		goto _test_eof81;
case 81:
	if ( (*p) == 58 )
		goto tr108;
	goto st0;
tr108:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st301;
st301:
	if ( ++p == pe )
		goto _test_eof301;
case 301:
#line 2774 "ip_utils.c"
	if ( (*p) == 50 )
		goto tr303;
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto tr302;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto tr305;
		} else if ( (*p) >= 65 )
			goto tr305;
	} else
		goto tr304;
	goto st0;
tr302:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st302;
st302:
	if ( ++p == pe )
		goto _test_eof302;
case 302:
#line 2799 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st82;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr306;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr308;
	} else
		goto tr308;
	goto st0;
tr306:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st303;
st303:
	if ( ++p == pe )
		goto _test_eof303;
case 303:
#line 2823 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st82;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr309;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr310;
	} else
		goto tr310;
	goto st0;
tr309:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st304;
st304:
	if ( ++p == pe )
		goto _test_eof304;
case 304:
#line 2847 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st82;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr311;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr311;
	} else
		goto tr311;
	goto st0;
tr311:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st305;
st305:
	if ( ++p == pe )
		goto _test_eof305;
case 305:
#line 2871 "ip_utils.c"
	if ( (*p) == 58 )
		goto st82;
	goto st0;
st82:
	if ( ++p == pe )
		goto _test_eof82;
case 82:
	if ( (*p) == 50 )
		goto tr110;
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto tr109;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto tr112;
		} else if ( (*p) >= 65 )
			goto tr112;
	} else
		goto tr111;
	goto st0;
tr310:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st306;
st306:
	if ( ++p == pe )
		goto _test_eof306;
case 306:
#line 2903 "ip_utils.c"
	if ( (*p) == 58 )
		goto st82;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr311;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr311;
	} else
		goto tr311;
	goto st0;
tr308:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st307;
st307:
	if ( ++p == pe )
		goto _test_eof307;
case 307:
#line 2925 "ip_utils.c"
	if ( (*p) == 58 )
		goto st82;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr310;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr310;
	} else
		goto tr310;
	goto st0;
tr303:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st308;
st308:
	if ( ++p == pe )
		goto _test_eof308;
case 308:
#line 2947 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 53: goto tr312;
		case 58: goto st82;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 52 )
			goto tr306;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto tr308;
		} else if ( (*p) >= 65 )
			goto tr308;
	} else
		goto tr313;
	goto st0;
tr312:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st309;
st309:
	if ( ++p == pe )
		goto _test_eof309;
case 309:
#line 2975 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st82;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 53 )
			goto tr309;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto tr310;
		} else if ( (*p) >= 65 )
			goto tr310;
	} else
		goto tr310;
	goto st0;
tr313:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st310;
st310:
	if ( ++p == pe )
		goto _test_eof310;
case 310:
#line 3002 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st82;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr310;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr310;
	} else
		goto tr310;
	goto st0;
tr304:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st311;
st311:
	if ( ++p == pe )
		goto _test_eof311;
case 311:
#line 3026 "ip_utils.c"
	switch( (*p) ) {
		case 46: goto st46;
		case 58: goto st82;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr313;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr308;
	} else
		goto tr308;
	goto st0;
tr305:
#line 33 "ip_utils.rl"
	{
    ip_type = ip_type_ipv6;
  }
	goto st312;
st312:
	if ( ++p == pe )
		goto _test_eof312;
case 312:
#line 3050 "ip_utils.c"
	if ( (*p) == 58 )
		goto st82;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr308;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr308;
	} else
		goto tr308;
	goto st0;
st83:
	if ( ++p == pe )
		goto _test_eof83;
case 83:
	if ( (*p) == 58 )
		goto st19;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st76;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st76;
	} else
		goto st76;
	goto st0;
st84:
	if ( ++p == pe )
		goto _test_eof84;
case 84:
	if ( (*p) == 58 )
		goto st214;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st85;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st85;
	} else
		goto st85;
	goto st0;
st85:
	if ( ++p == pe )
		goto _test_eof85;
case 85:
	if ( (*p) == 58 )
		goto st89;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st86;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st86;
	} else
		goto st86;
	goto st0;
st86:
	if ( ++p == pe )
		goto _test_eof86;
case 86:
	if ( (*p) == 58 )
		goto st89;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st87;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st87;
	} else
		goto st87;
	goto st0;
st87:
	if ( ++p == pe )
		goto _test_eof87;
case 87:
	if ( (*p) == 58 )
		goto st89;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st88;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st88;
	} else
		goto st88;
	goto st0;
st88:
	if ( ++p == pe )
		goto _test_eof88;
case 88:
	if ( (*p) == 58 )
		goto st89;
	goto st0;
st89:
	if ( ++p == pe )
		goto _test_eof89;
case 89:
	if ( (*p) == 58 )
		goto st201;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st90;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st90;
	} else
		goto st90;
	goto st0;
st90:
	if ( ++p == pe )
		goto _test_eof90;
case 90:
	if ( (*p) == 58 )
		goto st94;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st91;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st91;
	} else
		goto st91;
	goto st0;
st91:
	if ( ++p == pe )
		goto _test_eof91;
case 91:
	if ( (*p) == 58 )
		goto st94;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st92;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st92;
	} else
		goto st92;
	goto st0;
st92:
	if ( ++p == pe )
		goto _test_eof92;
case 92:
	if ( (*p) == 58 )
		goto st94;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st93;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st93;
	} else
		goto st93;
	goto st0;
st93:
	if ( ++p == pe )
		goto _test_eof93;
case 93:
	if ( (*p) == 58 )
		goto st94;
	goto st0;
st94:
	if ( ++p == pe )
		goto _test_eof94;
case 94:
	if ( (*p) == 58 )
		goto st188;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st95;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st95;
	} else
		goto st95;
	goto st0;
st95:
	if ( ++p == pe )
		goto _test_eof95;
case 95:
	if ( (*p) == 58 )
		goto st99;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st96;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st96;
	} else
		goto st96;
	goto st0;
st96:
	if ( ++p == pe )
		goto _test_eof96;
case 96:
	if ( (*p) == 58 )
		goto st99;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st97;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st97;
	} else
		goto st97;
	goto st0;
st97:
	if ( ++p == pe )
		goto _test_eof97;
case 97:
	if ( (*p) == 58 )
		goto st99;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st98;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st98;
	} else
		goto st98;
	goto st0;
st98:
	if ( ++p == pe )
		goto _test_eof98;
case 98:
	if ( (*p) == 58 )
		goto st99;
	goto st0;
st99:
	if ( ++p == pe )
		goto _test_eof99;
case 99:
	if ( (*p) == 58 )
		goto st175;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st100;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st100;
	} else
		goto st100;
	goto st0;
st100:
	if ( ++p == pe )
		goto _test_eof100;
case 100:
	if ( (*p) == 58 )
		goto st104;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st101;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st101;
	} else
		goto st101;
	goto st0;
st101:
	if ( ++p == pe )
		goto _test_eof101;
case 101:
	if ( (*p) == 58 )
		goto st104;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st102;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st102;
	} else
		goto st102;
	goto st0;
st102:
	if ( ++p == pe )
		goto _test_eof102;
case 102:
	if ( (*p) == 58 )
		goto st104;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st103;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st103;
	} else
		goto st103;
	goto st0;
st103:
	if ( ++p == pe )
		goto _test_eof103;
case 103:
	if ( (*p) == 58 )
		goto st104;
	goto st0;
st104:
	if ( ++p == pe )
		goto _test_eof104;
case 104:
	if ( (*p) == 58 )
		goto st162;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st105;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st105;
	} else
		goto st105;
	goto st0;
st105:
	if ( ++p == pe )
		goto _test_eof105;
case 105:
	if ( (*p) == 58 )
		goto st109;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st106;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st106;
	} else
		goto st106;
	goto st0;
st106:
	if ( ++p == pe )
		goto _test_eof106;
case 106:
	if ( (*p) == 58 )
		goto st109;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st107;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st107;
	} else
		goto st107;
	goto st0;
st107:
	if ( ++p == pe )
		goto _test_eof107;
case 107:
	if ( (*p) == 58 )
		goto st109;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st108;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st108;
	} else
		goto st108;
	goto st0;
st108:
	if ( ++p == pe )
		goto _test_eof108;
case 108:
	if ( (*p) == 58 )
		goto st109;
	goto st0;
st109:
	if ( ++p == pe )
		goto _test_eof109;
case 109:
	if ( (*p) == 58 )
		goto st149;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st110;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st110;
	} else
		goto st110;
	goto st0;
st110:
	if ( ++p == pe )
		goto _test_eof110;
case 110:
	if ( (*p) == 58 )
		goto st114;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st111;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st111;
	} else
		goto st111;
	goto st0;
st111:
	if ( ++p == pe )
		goto _test_eof111;
case 111:
	if ( (*p) == 58 )
		goto st114;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st112;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st112;
	} else
		goto st112;
	goto st0;
st112:
	if ( ++p == pe )
		goto _test_eof112;
case 112:
	if ( (*p) == 58 )
		goto st114;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st113;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st113;
	} else
		goto st113;
	goto st0;
st113:
	if ( ++p == pe )
		goto _test_eof113;
case 113:
	if ( (*p) == 58 )
		goto st114;
	goto st0;
st114:
	if ( ++p == pe )
		goto _test_eof114;
case 114:
	switch( (*p) ) {
		case 50: goto st143;
		case 58: goto st147;
	}
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto st115;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st148;
		} else if ( (*p) >= 65 )
			goto st148;
	} else
		goto st146;
	goto st0;
st115:
	if ( ++p == pe )
		goto _test_eof115;
case 115:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st137;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st134;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st142;
	} else
		goto st142;
	goto st0;
st116:
	if ( ++p == pe )
		goto _test_eof116;
case 116:
	if ( (*p) == 50 )
		goto st132;
	if ( (*p) > 49 ) {
		if ( 51 <= (*p) && (*p) <= 57 )
			goto st130;
	} else if ( (*p) >= 48 )
		goto st117;
	goto st0;
st117:
	if ( ++p == pe )
		goto _test_eof117;
case 117:
	if ( (*p) == 46 )
		goto st118;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st130;
	goto st0;
st118:
	if ( ++p == pe )
		goto _test_eof118;
case 118:
	if ( (*p) == 50 )
		goto st128;
	if ( (*p) > 49 ) {
		if ( 51 <= (*p) && (*p) <= 57 )
			goto st126;
	} else if ( (*p) >= 48 )
		goto st119;
	goto st0;
st119:
	if ( ++p == pe )
		goto _test_eof119;
case 119:
	if ( (*p) == 46 )
		goto st120;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st126;
	goto st0;
st120:
	if ( ++p == pe )
		goto _test_eof120;
case 120:
	if ( (*p) == 50 )
		goto st124;
	if ( (*p) > 49 ) {
		if ( 51 <= (*p) && (*p) <= 57 )
			goto st122;
	} else if ( (*p) >= 48 )
		goto st121;
	goto st0;
st121:
	if ( ++p == pe )
		goto _test_eof121;
case 121:
	if ( (*p) == 93 )
		goto tr169;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st122;
	goto st0;
st122:
	if ( ++p == pe )
		goto _test_eof122;
case 122:
	if ( (*p) == 93 )
		goto tr169;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st123;
	goto st0;
st123:
	if ( ++p == pe )
		goto _test_eof123;
case 123:
	if ( (*p) == 93 )
		goto tr169;
	goto st0;
st124:
	if ( ++p == pe )
		goto _test_eof124;
case 124:
	switch( (*p) ) {
		case 53: goto st125;
		case 93: goto tr169;
	}
	if ( (*p) > 52 ) {
		if ( 54 <= (*p) && (*p) <= 57 )
			goto st123;
	} else if ( (*p) >= 48 )
		goto st122;
	goto st0;
st125:
	if ( ++p == pe )
		goto _test_eof125;
case 125:
	if ( (*p) == 93 )
		goto tr169;
	if ( 48 <= (*p) && (*p) <= 53 )
		goto st123;
	goto st0;
st126:
	if ( ++p == pe )
		goto _test_eof126;
case 126:
	if ( (*p) == 46 )
		goto st120;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st127;
	goto st0;
st127:
	if ( ++p == pe )
		goto _test_eof127;
case 127:
	if ( (*p) == 46 )
		goto st120;
	goto st0;
st128:
	if ( ++p == pe )
		goto _test_eof128;
case 128:
	switch( (*p) ) {
		case 46: goto st120;
		case 53: goto st129;
	}
	if ( (*p) > 52 ) {
		if ( 54 <= (*p) && (*p) <= 57 )
			goto st127;
	} else if ( (*p) >= 48 )
		goto st126;
	goto st0;
st129:
	if ( ++p == pe )
		goto _test_eof129;
case 129:
	if ( (*p) == 46 )
		goto st120;
	if ( 48 <= (*p) && (*p) <= 53 )
		goto st127;
	goto st0;
st130:
	if ( ++p == pe )
		goto _test_eof130;
case 130:
	if ( (*p) == 46 )
		goto st118;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st131;
	goto st0;
st131:
	if ( ++p == pe )
		goto _test_eof131;
case 131:
	if ( (*p) == 46 )
		goto st118;
	goto st0;
st132:
	if ( ++p == pe )
		goto _test_eof132;
case 132:
	switch( (*p) ) {
		case 46: goto st118;
		case 53: goto st133;
	}
	if ( (*p) > 52 ) {
		if ( 54 <= (*p) && (*p) <= 57 )
			goto st131;
	} else if ( (*p) >= 48 )
		goto st130;
	goto st0;
st133:
	if ( ++p == pe )
		goto _test_eof133;
case 133:
	if ( (*p) == 46 )
		goto st118;
	if ( 48 <= (*p) && (*p) <= 53 )
		goto st131;
	goto st0;
st134:
	if ( ++p == pe )
		goto _test_eof134;
case 134:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st137;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st135;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st141;
	} else
		goto st141;
	goto st0;
st135:
	if ( ++p == pe )
		goto _test_eof135;
case 135:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st137;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st136;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st136;
	} else
		goto st136;
	goto st0;
st136:
	if ( ++p == pe )
		goto _test_eof136;
case 136:
	if ( (*p) == 58 )
		goto st137;
	goto st0;
st137:
	if ( ++p == pe )
		goto _test_eof137;
case 137:
	if ( (*p) == 58 )
		goto st123;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st138;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st138;
	} else
		goto st138;
	goto st0;
st138:
	if ( ++p == pe )
		goto _test_eof138;
case 138:
	if ( (*p) == 93 )
		goto tr169;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st139;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st139;
	} else
		goto st139;
	goto st0;
st139:
	if ( ++p == pe )
		goto _test_eof139;
case 139:
	if ( (*p) == 93 )
		goto tr169;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st140;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st140;
	} else
		goto st140;
	goto st0;
st140:
	if ( ++p == pe )
		goto _test_eof140;
case 140:
	if ( (*p) == 93 )
		goto tr169;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st123;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st123;
	} else
		goto st123;
	goto st0;
st141:
	if ( ++p == pe )
		goto _test_eof141;
case 141:
	if ( (*p) == 58 )
		goto st137;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st136;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st136;
	} else
		goto st136;
	goto st0;
st142:
	if ( ++p == pe )
		goto _test_eof142;
case 142:
	if ( (*p) == 58 )
		goto st137;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st141;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st141;
	} else
		goto st141;
	goto st0;
st143:
	if ( ++p == pe )
		goto _test_eof143;
case 143:
	switch( (*p) ) {
		case 46: goto st116;
		case 53: goto st144;
		case 58: goto st137;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 52 )
			goto st134;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st142;
		} else if ( (*p) >= 65 )
			goto st142;
	} else
		goto st145;
	goto st0;
st144:
	if ( ++p == pe )
		goto _test_eof144;
case 144:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st137;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 53 )
			goto st135;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st141;
		} else if ( (*p) >= 65 )
			goto st141;
	} else
		goto st141;
	goto st0;
st145:
	if ( ++p == pe )
		goto _test_eof145;
case 145:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st137;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st141;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st141;
	} else
		goto st141;
	goto st0;
st146:
	if ( ++p == pe )
		goto _test_eof146;
case 146:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st137;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st145;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st142;
	} else
		goto st142;
	goto st0;
st147:
	if ( ++p == pe )
		goto _test_eof147;
case 147:
	if ( (*p) == 93 )
		goto tr169;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st138;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st138;
	} else
		goto st138;
	goto st0;
st148:
	if ( ++p == pe )
		goto _test_eof148;
case 148:
	if ( (*p) == 58 )
		goto st137;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st142;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st142;
	} else
		goto st142;
	goto st0;
st149:
	if ( ++p == pe )
		goto _test_eof149;
case 149:
	switch( (*p) ) {
		case 50: goto st157;
		case 93: goto tr169;
	}
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto st150;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st161;
		} else if ( (*p) >= 65 )
			goto st161;
	} else
		goto st160;
	goto st0;
st150:
	if ( ++p == pe )
		goto _test_eof150;
case 150:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st154;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st151;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st156;
	} else
		goto st156;
	goto st0;
st151:
	if ( ++p == pe )
		goto _test_eof151;
case 151:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st154;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st152;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st155;
	} else
		goto st155;
	goto st0;
st152:
	if ( ++p == pe )
		goto _test_eof152;
case 152:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st154;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st153;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st153;
	} else
		goto st153;
	goto st0;
st153:
	if ( ++p == pe )
		goto _test_eof153;
case 153:
	switch( (*p) ) {
		case 58: goto st154;
		case 93: goto tr169;
	}
	goto st0;
st154:
	if ( ++p == pe )
		goto _test_eof154;
case 154:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st138;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st138;
	} else
		goto st138;
	goto st0;
st155:
	if ( ++p == pe )
		goto _test_eof155;
case 155:
	switch( (*p) ) {
		case 58: goto st154;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st153;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st153;
	} else
		goto st153;
	goto st0;
st156:
	if ( ++p == pe )
		goto _test_eof156;
case 156:
	switch( (*p) ) {
		case 58: goto st154;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st155;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st155;
	} else
		goto st155;
	goto st0;
st157:
	if ( ++p == pe )
		goto _test_eof157;
case 157:
	switch( (*p) ) {
		case 46: goto st116;
		case 53: goto st158;
		case 58: goto st154;
		case 93: goto tr169;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 52 )
			goto st151;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st156;
		} else if ( (*p) >= 65 )
			goto st156;
	} else
		goto st159;
	goto st0;
st158:
	if ( ++p == pe )
		goto _test_eof158;
case 158:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st154;
		case 93: goto tr169;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 53 )
			goto st152;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st155;
		} else if ( (*p) >= 65 )
			goto st155;
	} else
		goto st155;
	goto st0;
st159:
	if ( ++p == pe )
		goto _test_eof159;
case 159:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st154;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st155;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st155;
	} else
		goto st155;
	goto st0;
st160:
	if ( ++p == pe )
		goto _test_eof160;
case 160:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st154;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st159;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st156;
	} else
		goto st156;
	goto st0;
st161:
	if ( ++p == pe )
		goto _test_eof161;
case 161:
	switch( (*p) ) {
		case 58: goto st154;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st156;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st156;
	} else
		goto st156;
	goto st0;
st162:
	if ( ++p == pe )
		goto _test_eof162;
case 162:
	switch( (*p) ) {
		case 50: goto st170;
		case 93: goto tr169;
	}
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto st163;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st174;
		} else if ( (*p) >= 65 )
			goto st174;
	} else
		goto st173;
	goto st0;
st163:
	if ( ++p == pe )
		goto _test_eof163;
case 163:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st167;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st164;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st169;
	} else
		goto st169;
	goto st0;
st164:
	if ( ++p == pe )
		goto _test_eof164;
case 164:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st167;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st165;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st168;
	} else
		goto st168;
	goto st0;
st165:
	if ( ++p == pe )
		goto _test_eof165;
case 165:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st167;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st166;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st166;
	} else
		goto st166;
	goto st0;
st166:
	if ( ++p == pe )
		goto _test_eof166;
case 166:
	switch( (*p) ) {
		case 58: goto st167;
		case 93: goto tr169;
	}
	goto st0;
st167:
	if ( ++p == pe )
		goto _test_eof167;
case 167:
	if ( (*p) == 50 )
		goto st157;
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto st150;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st161;
		} else if ( (*p) >= 65 )
			goto st161;
	} else
		goto st160;
	goto st0;
st168:
	if ( ++p == pe )
		goto _test_eof168;
case 168:
	switch( (*p) ) {
		case 58: goto st167;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st166;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st166;
	} else
		goto st166;
	goto st0;
st169:
	if ( ++p == pe )
		goto _test_eof169;
case 169:
	switch( (*p) ) {
		case 58: goto st167;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st168;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st168;
	} else
		goto st168;
	goto st0;
st170:
	if ( ++p == pe )
		goto _test_eof170;
case 170:
	switch( (*p) ) {
		case 46: goto st116;
		case 53: goto st171;
		case 58: goto st167;
		case 93: goto tr169;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 52 )
			goto st164;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st169;
		} else if ( (*p) >= 65 )
			goto st169;
	} else
		goto st172;
	goto st0;
st171:
	if ( ++p == pe )
		goto _test_eof171;
case 171:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st167;
		case 93: goto tr169;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 53 )
			goto st165;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st168;
		} else if ( (*p) >= 65 )
			goto st168;
	} else
		goto st168;
	goto st0;
st172:
	if ( ++p == pe )
		goto _test_eof172;
case 172:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st167;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st168;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st168;
	} else
		goto st168;
	goto st0;
st173:
	if ( ++p == pe )
		goto _test_eof173;
case 173:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st167;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st172;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st169;
	} else
		goto st169;
	goto st0;
st174:
	if ( ++p == pe )
		goto _test_eof174;
case 174:
	switch( (*p) ) {
		case 58: goto st167;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st169;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st169;
	} else
		goto st169;
	goto st0;
st175:
	if ( ++p == pe )
		goto _test_eof175;
case 175:
	switch( (*p) ) {
		case 50: goto st183;
		case 93: goto tr169;
	}
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto st176;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st187;
		} else if ( (*p) >= 65 )
			goto st187;
	} else
		goto st186;
	goto st0;
st176:
	if ( ++p == pe )
		goto _test_eof176;
case 176:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st180;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st177;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st182;
	} else
		goto st182;
	goto st0;
st177:
	if ( ++p == pe )
		goto _test_eof177;
case 177:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st180;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st178;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st181;
	} else
		goto st181;
	goto st0;
st178:
	if ( ++p == pe )
		goto _test_eof178;
case 178:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st180;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st179;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st179;
	} else
		goto st179;
	goto st0;
st179:
	if ( ++p == pe )
		goto _test_eof179;
case 179:
	switch( (*p) ) {
		case 58: goto st180;
		case 93: goto tr169;
	}
	goto st0;
st180:
	if ( ++p == pe )
		goto _test_eof180;
case 180:
	if ( (*p) == 50 )
		goto st170;
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto st163;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st174;
		} else if ( (*p) >= 65 )
			goto st174;
	} else
		goto st173;
	goto st0;
st181:
	if ( ++p == pe )
		goto _test_eof181;
case 181:
	switch( (*p) ) {
		case 58: goto st180;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st179;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st179;
	} else
		goto st179;
	goto st0;
st182:
	if ( ++p == pe )
		goto _test_eof182;
case 182:
	switch( (*p) ) {
		case 58: goto st180;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st181;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st181;
	} else
		goto st181;
	goto st0;
st183:
	if ( ++p == pe )
		goto _test_eof183;
case 183:
	switch( (*p) ) {
		case 46: goto st116;
		case 53: goto st184;
		case 58: goto st180;
		case 93: goto tr169;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 52 )
			goto st177;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st182;
		} else if ( (*p) >= 65 )
			goto st182;
	} else
		goto st185;
	goto st0;
st184:
	if ( ++p == pe )
		goto _test_eof184;
case 184:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st180;
		case 93: goto tr169;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 53 )
			goto st178;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st181;
		} else if ( (*p) >= 65 )
			goto st181;
	} else
		goto st181;
	goto st0;
st185:
	if ( ++p == pe )
		goto _test_eof185;
case 185:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st180;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st181;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st181;
	} else
		goto st181;
	goto st0;
st186:
	if ( ++p == pe )
		goto _test_eof186;
case 186:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st180;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st185;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st182;
	} else
		goto st182;
	goto st0;
st187:
	if ( ++p == pe )
		goto _test_eof187;
case 187:
	switch( (*p) ) {
		case 58: goto st180;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st182;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st182;
	} else
		goto st182;
	goto st0;
st188:
	if ( ++p == pe )
		goto _test_eof188;
case 188:
	switch( (*p) ) {
		case 50: goto st196;
		case 93: goto tr169;
	}
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto st189;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st200;
		} else if ( (*p) >= 65 )
			goto st200;
	} else
		goto st199;
	goto st0;
st189:
	if ( ++p == pe )
		goto _test_eof189;
case 189:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st193;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st190;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st195;
	} else
		goto st195;
	goto st0;
st190:
	if ( ++p == pe )
		goto _test_eof190;
case 190:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st193;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st191;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st194;
	} else
		goto st194;
	goto st0;
st191:
	if ( ++p == pe )
		goto _test_eof191;
case 191:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st193;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st192;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st192;
	} else
		goto st192;
	goto st0;
st192:
	if ( ++p == pe )
		goto _test_eof192;
case 192:
	switch( (*p) ) {
		case 58: goto st193;
		case 93: goto tr169;
	}
	goto st0;
st193:
	if ( ++p == pe )
		goto _test_eof193;
case 193:
	if ( (*p) == 50 )
		goto st183;
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto st176;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st187;
		} else if ( (*p) >= 65 )
			goto st187;
	} else
		goto st186;
	goto st0;
st194:
	if ( ++p == pe )
		goto _test_eof194;
case 194:
	switch( (*p) ) {
		case 58: goto st193;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st192;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st192;
	} else
		goto st192;
	goto st0;
st195:
	if ( ++p == pe )
		goto _test_eof195;
case 195:
	switch( (*p) ) {
		case 58: goto st193;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st194;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st194;
	} else
		goto st194;
	goto st0;
st196:
	if ( ++p == pe )
		goto _test_eof196;
case 196:
	switch( (*p) ) {
		case 46: goto st116;
		case 53: goto st197;
		case 58: goto st193;
		case 93: goto tr169;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 52 )
			goto st190;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st195;
		} else if ( (*p) >= 65 )
			goto st195;
	} else
		goto st198;
	goto st0;
st197:
	if ( ++p == pe )
		goto _test_eof197;
case 197:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st193;
		case 93: goto tr169;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 53 )
			goto st191;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st194;
		} else if ( (*p) >= 65 )
			goto st194;
	} else
		goto st194;
	goto st0;
st198:
	if ( ++p == pe )
		goto _test_eof198;
case 198:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st193;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st194;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st194;
	} else
		goto st194;
	goto st0;
st199:
	if ( ++p == pe )
		goto _test_eof199;
case 199:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st193;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st198;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st195;
	} else
		goto st195;
	goto st0;
st200:
	if ( ++p == pe )
		goto _test_eof200;
case 200:
	switch( (*p) ) {
		case 58: goto st193;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st195;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st195;
	} else
		goto st195;
	goto st0;
st201:
	if ( ++p == pe )
		goto _test_eof201;
case 201:
	switch( (*p) ) {
		case 50: goto st209;
		case 93: goto tr169;
	}
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto st202;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st213;
		} else if ( (*p) >= 65 )
			goto st213;
	} else
		goto st212;
	goto st0;
st202:
	if ( ++p == pe )
		goto _test_eof202;
case 202:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st206;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st203;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st208;
	} else
		goto st208;
	goto st0;
st203:
	if ( ++p == pe )
		goto _test_eof203;
case 203:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st206;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st204;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st207;
	} else
		goto st207;
	goto st0;
st204:
	if ( ++p == pe )
		goto _test_eof204;
case 204:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st206;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st205;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st205;
	} else
		goto st205;
	goto st0;
st205:
	if ( ++p == pe )
		goto _test_eof205;
case 205:
	switch( (*p) ) {
		case 58: goto st206;
		case 93: goto tr169;
	}
	goto st0;
st206:
	if ( ++p == pe )
		goto _test_eof206;
case 206:
	if ( (*p) == 50 )
		goto st196;
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto st189;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st200;
		} else if ( (*p) >= 65 )
			goto st200;
	} else
		goto st199;
	goto st0;
st207:
	if ( ++p == pe )
		goto _test_eof207;
case 207:
	switch( (*p) ) {
		case 58: goto st206;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st205;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st205;
	} else
		goto st205;
	goto st0;
st208:
	if ( ++p == pe )
		goto _test_eof208;
case 208:
	switch( (*p) ) {
		case 58: goto st206;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st207;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st207;
	} else
		goto st207;
	goto st0;
st209:
	if ( ++p == pe )
		goto _test_eof209;
case 209:
	switch( (*p) ) {
		case 46: goto st116;
		case 53: goto st210;
		case 58: goto st206;
		case 93: goto tr169;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 52 )
			goto st203;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st208;
		} else if ( (*p) >= 65 )
			goto st208;
	} else
		goto st211;
	goto st0;
st210:
	if ( ++p == pe )
		goto _test_eof210;
case 210:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st206;
		case 93: goto tr169;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 53 )
			goto st204;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st207;
		} else if ( (*p) >= 65 )
			goto st207;
	} else
		goto st207;
	goto st0;
st211:
	if ( ++p == pe )
		goto _test_eof211;
case 211:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st206;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st207;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st207;
	} else
		goto st207;
	goto st0;
st212:
	if ( ++p == pe )
		goto _test_eof212;
case 212:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st206;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st211;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st208;
	} else
		goto st208;
	goto st0;
st213:
	if ( ++p == pe )
		goto _test_eof213;
case 213:
	switch( (*p) ) {
		case 58: goto st206;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st208;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st208;
	} else
		goto st208;
	goto st0;
st214:
	if ( ++p == pe )
		goto _test_eof214;
case 214:
	if ( (*p) == 58 )
		goto st215;
	goto st0;
st215:
	if ( ++p == pe )
		goto _test_eof215;
case 215:
	switch( (*p) ) {
		case 50: goto st223;
		case 93: goto tr169;
	}
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto st216;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st227;
		} else if ( (*p) >= 65 )
			goto st227;
	} else
		goto st226;
	goto st0;
st216:
	if ( ++p == pe )
		goto _test_eof216;
case 216:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st220;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st217;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st222;
	} else
		goto st222;
	goto st0;
st217:
	if ( ++p == pe )
		goto _test_eof217;
case 217:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st220;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st218;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st221;
	} else
		goto st221;
	goto st0;
st218:
	if ( ++p == pe )
		goto _test_eof218;
case 218:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st220;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st219;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st219;
	} else
		goto st219;
	goto st0;
st219:
	if ( ++p == pe )
		goto _test_eof219;
case 219:
	switch( (*p) ) {
		case 58: goto st220;
		case 93: goto tr169;
	}
	goto st0;
st220:
	if ( ++p == pe )
		goto _test_eof220;
case 220:
	if ( (*p) == 50 )
		goto st209;
	if ( (*p) < 51 ) {
		if ( 48 <= (*p) && (*p) <= 49 )
			goto st202;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st213;
		} else if ( (*p) >= 65 )
			goto st213;
	} else
		goto st212;
	goto st0;
st221:
	if ( ++p == pe )
		goto _test_eof221;
case 221:
	switch( (*p) ) {
		case 58: goto st220;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st219;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st219;
	} else
		goto st219;
	goto st0;
st222:
	if ( ++p == pe )
		goto _test_eof222;
case 222:
	switch( (*p) ) {
		case 58: goto st220;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st221;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st221;
	} else
		goto st221;
	goto st0;
st223:
	if ( ++p == pe )
		goto _test_eof223;
case 223:
	switch( (*p) ) {
		case 46: goto st116;
		case 53: goto st224;
		case 58: goto st220;
		case 93: goto tr169;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 52 )
			goto st217;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st222;
		} else if ( (*p) >= 65 )
			goto st222;
	} else
		goto st225;
	goto st0;
st224:
	if ( ++p == pe )
		goto _test_eof224;
case 224:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st220;
		case 93: goto tr169;
	}
	if ( (*p) < 54 ) {
		if ( 48 <= (*p) && (*p) <= 53 )
			goto st218;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto st221;
		} else if ( (*p) >= 65 )
			goto st221;
	} else
		goto st221;
	goto st0;
st225:
	if ( ++p == pe )
		goto _test_eof225;
case 225:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st220;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st221;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st221;
	} else
		goto st221;
	goto st0;
st226:
	if ( ++p == pe )
		goto _test_eof226;
case 226:
	switch( (*p) ) {
		case 46: goto st116;
		case 58: goto st220;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st225;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st222;
	} else
		goto st222;
	goto st0;
st227:
	if ( ++p == pe )
		goto _test_eof227;
case 227:
	switch( (*p) ) {
		case 58: goto st220;
		case 93: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st222;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st222;
	} else
		goto st222;
	goto st0;
	}
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof228: cs = 228; goto _test_eof; 
	_test_eof229: cs = 229; goto _test_eof; 
	_test_eof230: cs = 230; goto _test_eof; 
	_test_eof231: cs = 231; goto _test_eof; 
	_test_eof232: cs = 232; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof11: cs = 11; goto _test_eof; 
	_test_eof12: cs = 12; goto _test_eof; 
	_test_eof13: cs = 13; goto _test_eof; 
	_test_eof14: cs = 14; goto _test_eof; 
	_test_eof15: cs = 15; goto _test_eof; 
	_test_eof16: cs = 16; goto _test_eof; 
	_test_eof17: cs = 17; goto _test_eof; 
	_test_eof18: cs = 18; goto _test_eof; 
	_test_eof19: cs = 19; goto _test_eof; 
	_test_eof20: cs = 20; goto _test_eof; 
	_test_eof21: cs = 21; goto _test_eof; 
	_test_eof22: cs = 22; goto _test_eof; 
	_test_eof23: cs = 23; goto _test_eof; 
	_test_eof24: cs = 24; goto _test_eof; 
	_test_eof25: cs = 25; goto _test_eof; 
	_test_eof26: cs = 26; goto _test_eof; 
	_test_eof27: cs = 27; goto _test_eof; 
	_test_eof28: cs = 28; goto _test_eof; 
	_test_eof29: cs = 29; goto _test_eof; 
	_test_eof30: cs = 30; goto _test_eof; 
	_test_eof31: cs = 31; goto _test_eof; 
	_test_eof32: cs = 32; goto _test_eof; 
	_test_eof33: cs = 33; goto _test_eof; 
	_test_eof34: cs = 34; goto _test_eof; 
	_test_eof35: cs = 35; goto _test_eof; 
	_test_eof36: cs = 36; goto _test_eof; 
	_test_eof37: cs = 37; goto _test_eof; 
	_test_eof38: cs = 38; goto _test_eof; 
	_test_eof39: cs = 39; goto _test_eof; 
	_test_eof40: cs = 40; goto _test_eof; 
	_test_eof41: cs = 41; goto _test_eof; 
	_test_eof42: cs = 42; goto _test_eof; 
	_test_eof43: cs = 43; goto _test_eof; 
	_test_eof44: cs = 44; goto _test_eof; 
	_test_eof45: cs = 45; goto _test_eof; 
	_test_eof46: cs = 46; goto _test_eof; 
	_test_eof47: cs = 47; goto _test_eof; 
	_test_eof48: cs = 48; goto _test_eof; 
	_test_eof49: cs = 49; goto _test_eof; 
	_test_eof50: cs = 50; goto _test_eof; 
	_test_eof233: cs = 233; goto _test_eof; 
	_test_eof234: cs = 234; goto _test_eof; 
	_test_eof235: cs = 235; goto _test_eof; 
	_test_eof236: cs = 236; goto _test_eof; 
	_test_eof51: cs = 51; goto _test_eof; 
	_test_eof52: cs = 52; goto _test_eof; 
	_test_eof53: cs = 53; goto _test_eof; 
	_test_eof54: cs = 54; goto _test_eof; 
	_test_eof55: cs = 55; goto _test_eof; 
	_test_eof56: cs = 56; goto _test_eof; 
	_test_eof57: cs = 57; goto _test_eof; 
	_test_eof58: cs = 58; goto _test_eof; 
	_test_eof59: cs = 59; goto _test_eof; 
	_test_eof60: cs = 60; goto _test_eof; 
	_test_eof61: cs = 61; goto _test_eof; 
	_test_eof62: cs = 62; goto _test_eof; 
	_test_eof237: cs = 237; goto _test_eof; 
	_test_eof238: cs = 238; goto _test_eof; 
	_test_eof239: cs = 239; goto _test_eof; 
	_test_eof63: cs = 63; goto _test_eof; 
	_test_eof64: cs = 64; goto _test_eof; 
	_test_eof65: cs = 65; goto _test_eof; 
	_test_eof66: cs = 66; goto _test_eof; 
	_test_eof67: cs = 67; goto _test_eof; 
	_test_eof68: cs = 68; goto _test_eof; 
	_test_eof240: cs = 240; goto _test_eof; 
	_test_eof69: cs = 69; goto _test_eof; 
	_test_eof241: cs = 241; goto _test_eof; 
	_test_eof242: cs = 242; goto _test_eof; 
	_test_eof243: cs = 243; goto _test_eof; 
	_test_eof244: cs = 244; goto _test_eof; 
	_test_eof245: cs = 245; goto _test_eof; 
	_test_eof70: cs = 70; goto _test_eof; 
	_test_eof246: cs = 246; goto _test_eof; 
	_test_eof247: cs = 247; goto _test_eof; 
	_test_eof248: cs = 248; goto _test_eof; 
	_test_eof249: cs = 249; goto _test_eof; 
	_test_eof250: cs = 250; goto _test_eof; 
	_test_eof251: cs = 251; goto _test_eof; 
	_test_eof252: cs = 252; goto _test_eof; 
	_test_eof253: cs = 253; goto _test_eof; 
	_test_eof254: cs = 254; goto _test_eof; 
	_test_eof255: cs = 255; goto _test_eof; 
	_test_eof256: cs = 256; goto _test_eof; 
	_test_eof257: cs = 257; goto _test_eof; 
	_test_eof71: cs = 71; goto _test_eof; 
	_test_eof258: cs = 258; goto _test_eof; 
	_test_eof259: cs = 259; goto _test_eof; 
	_test_eof260: cs = 260; goto _test_eof; 
	_test_eof261: cs = 261; goto _test_eof; 
	_test_eof262: cs = 262; goto _test_eof; 
	_test_eof263: cs = 263; goto _test_eof; 
	_test_eof264: cs = 264; goto _test_eof; 
	_test_eof265: cs = 265; goto _test_eof; 
	_test_eof266: cs = 266; goto _test_eof; 
	_test_eof267: cs = 267; goto _test_eof; 
	_test_eof268: cs = 268; goto _test_eof; 
	_test_eof269: cs = 269; goto _test_eof; 
	_test_eof72: cs = 72; goto _test_eof; 
	_test_eof270: cs = 270; goto _test_eof; 
	_test_eof271: cs = 271; goto _test_eof; 
	_test_eof272: cs = 272; goto _test_eof; 
	_test_eof273: cs = 273; goto _test_eof; 
	_test_eof274: cs = 274; goto _test_eof; 
	_test_eof275: cs = 275; goto _test_eof; 
	_test_eof276: cs = 276; goto _test_eof; 
	_test_eof277: cs = 277; goto _test_eof; 
	_test_eof278: cs = 278; goto _test_eof; 
	_test_eof279: cs = 279; goto _test_eof; 
	_test_eof280: cs = 280; goto _test_eof; 
	_test_eof281: cs = 281; goto _test_eof; 
	_test_eof73: cs = 73; goto _test_eof; 
	_test_eof282: cs = 282; goto _test_eof; 
	_test_eof283: cs = 283; goto _test_eof; 
	_test_eof284: cs = 284; goto _test_eof; 
	_test_eof285: cs = 285; goto _test_eof; 
	_test_eof286: cs = 286; goto _test_eof; 
	_test_eof287: cs = 287; goto _test_eof; 
	_test_eof288: cs = 288; goto _test_eof; 
	_test_eof289: cs = 289; goto _test_eof; 
	_test_eof290: cs = 290; goto _test_eof; 
	_test_eof291: cs = 291; goto _test_eof; 
	_test_eof292: cs = 292; goto _test_eof; 
	_test_eof293: cs = 293; goto _test_eof; 
	_test_eof74: cs = 74; goto _test_eof; 
	_test_eof294: cs = 294; goto _test_eof; 
	_test_eof295: cs = 295; goto _test_eof; 
	_test_eof296: cs = 296; goto _test_eof; 
	_test_eof297: cs = 297; goto _test_eof; 
	_test_eof298: cs = 298; goto _test_eof; 
	_test_eof299: cs = 299; goto _test_eof; 
	_test_eof300: cs = 300; goto _test_eof; 
	_test_eof75: cs = 75; goto _test_eof; 
	_test_eof76: cs = 76; goto _test_eof; 
	_test_eof77: cs = 77; goto _test_eof; 
	_test_eof78: cs = 78; goto _test_eof; 
	_test_eof79: cs = 79; goto _test_eof; 
	_test_eof80: cs = 80; goto _test_eof; 
	_test_eof81: cs = 81; goto _test_eof; 
	_test_eof301: cs = 301; goto _test_eof; 
	_test_eof302: cs = 302; goto _test_eof; 
	_test_eof303: cs = 303; goto _test_eof; 
	_test_eof304: cs = 304; goto _test_eof; 
	_test_eof305: cs = 305; goto _test_eof; 
	_test_eof82: cs = 82; goto _test_eof; 
	_test_eof306: cs = 306; goto _test_eof; 
	_test_eof307: cs = 307; goto _test_eof; 
	_test_eof308: cs = 308; goto _test_eof; 
	_test_eof309: cs = 309; goto _test_eof; 
	_test_eof310: cs = 310; goto _test_eof; 
	_test_eof311: cs = 311; goto _test_eof; 
	_test_eof312: cs = 312; goto _test_eof; 
	_test_eof83: cs = 83; goto _test_eof; 
	_test_eof84: cs = 84; goto _test_eof; 
	_test_eof85: cs = 85; goto _test_eof; 
	_test_eof86: cs = 86; goto _test_eof; 
	_test_eof87: cs = 87; goto _test_eof; 
	_test_eof88: cs = 88; goto _test_eof; 
	_test_eof89: cs = 89; goto _test_eof; 
	_test_eof90: cs = 90; goto _test_eof; 
	_test_eof91: cs = 91; goto _test_eof; 
	_test_eof92: cs = 92; goto _test_eof; 
	_test_eof93: cs = 93; goto _test_eof; 
	_test_eof94: cs = 94; goto _test_eof; 
	_test_eof95: cs = 95; goto _test_eof; 
	_test_eof96: cs = 96; goto _test_eof; 
	_test_eof97: cs = 97; goto _test_eof; 
	_test_eof98: cs = 98; goto _test_eof; 
	_test_eof99: cs = 99; goto _test_eof; 
	_test_eof100: cs = 100; goto _test_eof; 
	_test_eof101: cs = 101; goto _test_eof; 
	_test_eof102: cs = 102; goto _test_eof; 
	_test_eof103: cs = 103; goto _test_eof; 
	_test_eof104: cs = 104; goto _test_eof; 
	_test_eof105: cs = 105; goto _test_eof; 
	_test_eof106: cs = 106; goto _test_eof; 
	_test_eof107: cs = 107; goto _test_eof; 
	_test_eof108: cs = 108; goto _test_eof; 
	_test_eof109: cs = 109; goto _test_eof; 
	_test_eof110: cs = 110; goto _test_eof; 
	_test_eof111: cs = 111; goto _test_eof; 
	_test_eof112: cs = 112; goto _test_eof; 
	_test_eof113: cs = 113; goto _test_eof; 
	_test_eof114: cs = 114; goto _test_eof; 
	_test_eof115: cs = 115; goto _test_eof; 
	_test_eof116: cs = 116; goto _test_eof; 
	_test_eof117: cs = 117; goto _test_eof; 
	_test_eof118: cs = 118; goto _test_eof; 
	_test_eof119: cs = 119; goto _test_eof; 
	_test_eof120: cs = 120; goto _test_eof; 
	_test_eof121: cs = 121; goto _test_eof; 
	_test_eof122: cs = 122; goto _test_eof; 
	_test_eof123: cs = 123; goto _test_eof; 
	_test_eof124: cs = 124; goto _test_eof; 
	_test_eof125: cs = 125; goto _test_eof; 
	_test_eof126: cs = 126; goto _test_eof; 
	_test_eof127: cs = 127; goto _test_eof; 
	_test_eof128: cs = 128; goto _test_eof; 
	_test_eof129: cs = 129; goto _test_eof; 
	_test_eof130: cs = 130; goto _test_eof; 
	_test_eof131: cs = 131; goto _test_eof; 
	_test_eof132: cs = 132; goto _test_eof; 
	_test_eof133: cs = 133; goto _test_eof; 
	_test_eof134: cs = 134; goto _test_eof; 
	_test_eof135: cs = 135; goto _test_eof; 
	_test_eof136: cs = 136; goto _test_eof; 
	_test_eof137: cs = 137; goto _test_eof; 
	_test_eof138: cs = 138; goto _test_eof; 
	_test_eof139: cs = 139; goto _test_eof; 
	_test_eof140: cs = 140; goto _test_eof; 
	_test_eof141: cs = 141; goto _test_eof; 
	_test_eof142: cs = 142; goto _test_eof; 
	_test_eof143: cs = 143; goto _test_eof; 
	_test_eof144: cs = 144; goto _test_eof; 
	_test_eof145: cs = 145; goto _test_eof; 
	_test_eof146: cs = 146; goto _test_eof; 
	_test_eof147: cs = 147; goto _test_eof; 
	_test_eof148: cs = 148; goto _test_eof; 
	_test_eof149: cs = 149; goto _test_eof; 
	_test_eof150: cs = 150; goto _test_eof; 
	_test_eof151: cs = 151; goto _test_eof; 
	_test_eof152: cs = 152; goto _test_eof; 
	_test_eof153: cs = 153; goto _test_eof; 
	_test_eof154: cs = 154; goto _test_eof; 
	_test_eof155: cs = 155; goto _test_eof; 
	_test_eof156: cs = 156; goto _test_eof; 
	_test_eof157: cs = 157; goto _test_eof; 
	_test_eof158: cs = 158; goto _test_eof; 
	_test_eof159: cs = 159; goto _test_eof; 
	_test_eof160: cs = 160; goto _test_eof; 
	_test_eof161: cs = 161; goto _test_eof; 
	_test_eof162: cs = 162; goto _test_eof; 
	_test_eof163: cs = 163; goto _test_eof; 
	_test_eof164: cs = 164; goto _test_eof; 
	_test_eof165: cs = 165; goto _test_eof; 
	_test_eof166: cs = 166; goto _test_eof; 
	_test_eof167: cs = 167; goto _test_eof; 
	_test_eof168: cs = 168; goto _test_eof; 
	_test_eof169: cs = 169; goto _test_eof; 
	_test_eof170: cs = 170; goto _test_eof; 
	_test_eof171: cs = 171; goto _test_eof; 
	_test_eof172: cs = 172; goto _test_eof; 
	_test_eof173: cs = 173; goto _test_eof; 
	_test_eof174: cs = 174; goto _test_eof; 
	_test_eof175: cs = 175; goto _test_eof; 
	_test_eof176: cs = 176; goto _test_eof; 
	_test_eof177: cs = 177; goto _test_eof; 
	_test_eof178: cs = 178; goto _test_eof; 
	_test_eof179: cs = 179; goto _test_eof; 
	_test_eof180: cs = 180; goto _test_eof; 
	_test_eof181: cs = 181; goto _test_eof; 
	_test_eof182: cs = 182; goto _test_eof; 
	_test_eof183: cs = 183; goto _test_eof; 
	_test_eof184: cs = 184; goto _test_eof; 
	_test_eof185: cs = 185; goto _test_eof; 
	_test_eof186: cs = 186; goto _test_eof; 
	_test_eof187: cs = 187; goto _test_eof; 
	_test_eof188: cs = 188; goto _test_eof; 
	_test_eof189: cs = 189; goto _test_eof; 
	_test_eof190: cs = 190; goto _test_eof; 
	_test_eof191: cs = 191; goto _test_eof; 
	_test_eof192: cs = 192; goto _test_eof; 
	_test_eof193: cs = 193; goto _test_eof; 
	_test_eof194: cs = 194; goto _test_eof; 
	_test_eof195: cs = 195; goto _test_eof; 
	_test_eof196: cs = 196; goto _test_eof; 
	_test_eof197: cs = 197; goto _test_eof; 
	_test_eof198: cs = 198; goto _test_eof; 
	_test_eof199: cs = 199; goto _test_eof; 
	_test_eof200: cs = 200; goto _test_eof; 
	_test_eof201: cs = 201; goto _test_eof; 
	_test_eof202: cs = 202; goto _test_eof; 
	_test_eof203: cs = 203; goto _test_eof; 
	_test_eof204: cs = 204; goto _test_eof; 
	_test_eof205: cs = 205; goto _test_eof; 
	_test_eof206: cs = 206; goto _test_eof; 
	_test_eof207: cs = 207; goto _test_eof; 
	_test_eof208: cs = 208; goto _test_eof; 
	_test_eof209: cs = 209; goto _test_eof; 
	_test_eof210: cs = 210; goto _test_eof; 
	_test_eof211: cs = 211; goto _test_eof; 
	_test_eof212: cs = 212; goto _test_eof; 
	_test_eof213: cs = 213; goto _test_eof; 
	_test_eof214: cs = 214; goto _test_eof; 
	_test_eof215: cs = 215; goto _test_eof; 
	_test_eof216: cs = 216; goto _test_eof; 
	_test_eof217: cs = 217; goto _test_eof; 
	_test_eof218: cs = 218; goto _test_eof; 
	_test_eof219: cs = 219; goto _test_eof; 
	_test_eof220: cs = 220; goto _test_eof; 
	_test_eof221: cs = 221; goto _test_eof; 
	_test_eof222: cs = 222; goto _test_eof; 
	_test_eof223: cs = 223; goto _test_eof; 
	_test_eof224: cs = 224; goto _test_eof; 
	_test_eof225: cs = 225; goto _test_eof; 
	_test_eof226: cs = 226; goto _test_eof; 
	_test_eof227: cs = 227; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 63 "ip_utils.rl"

  if(len != p-str)
    return ip_type_error;
  else
    return ip_type;
}
