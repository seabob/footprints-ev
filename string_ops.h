#ifndef __STRING_OPS_H__
#define __STRING_OPS_H__

static inline unsigned char asc2hex(char c)
{
	if(c >= 'A' && c < 'Z')	return c-'A'+10;
	else if(c >= 'a' && c < 'z') return c-'a'+10;
	return c-'0';
}

static inline unsigned char str2hex(char m, char n)
{
	return (asc2hex(m)<<4 | asc2hex(n))	;
}

static inline unsigned char have_escope(char* src)
{
	if(strstr(src,"AA") || strstr(src,"aa"))
		return 1;
	return 0;
}

static inline unsigned char check_source(char * src)
{
	if(!src)
		return 1;
	
}
#endif
