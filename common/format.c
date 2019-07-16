#include <format.h>



static void _Dos_FormatChar(dos_char **str, dos_int32 c)
{
	//extern dos_int32 putchar(dos_int32 c);
	
	if (str) {
		**str = (dos_char)c;
		++(*str);
	}
	else
	{ 
//		(void)putchar(c);
	}
}


static dos_int32 _Dos_FormatStr(dos_char **out, const dos_char *string, dos_int32 width, dos_int32 pad)
{
	register dos_int32 pc = 0, padchar = ' ';

	if (width > 0) {
		register dos_int32 len = 0;
		register const dos_char *ptr;
		for (ptr = string; *ptr; ++ptr) ++len;
		if (len >= width) width = 0;
		else width -= len;
		if (pad & PAD_ZERO) padchar = '0';
	}
	if (!(pad & PAD_RIGHT)) {
		for ( ; width > 0; --width) {
			_Dos_FormatChar (out, padchar);
			++pc;
		}
	}
	for ( ; *string ; ++string) {
		_Dos_FormatChar (out, *string);
		++pc;
	}
	for ( ; width > 0; --width) {
		_Dos_FormatChar (out, padchar);
		++pc;
	}

	return pc;
}


static dos_int32 _Dos_FormatInt(dos_char **out, dos_int32 i, dos_int32 b, dos_int32 sg, dos_int32 width, dos_int32 pad, dos_int32 letbase)
{
	dos_char print_buf[FORMAT_BUF_LEN];
	register dos_char *s;
	register dos_int32 t, neg = 0, pc = 0;
	register dos_uint32 u = (dos_uint32)i;

	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return _Dos_FormatStr (out, print_buf, width, pad);
	}

	if (sg && b == 10 && i < 0) {
		neg = 1;
		u = (dos_uint32)-i;
	}

	s = print_buf + FORMAT_BUF_LEN-1;
	*s = '\0';

	while (u) {
		t = (dos_uint32)u % b;
		if( t >= 10 )
			t += letbase - '0' - 10;
		*--s = (dos_char)(t + '0');
		u /= b;
	}

	if (neg) {
		if( width && (pad & PAD_ZERO) ) {
			_Dos_FormatChar (out, '-');
			++pc;
			--width;
		}
		else {
			*--s = '-';
		}
	}

	return pc + _Dos_FormatStr (out, s, width, pad);
}

static dos_int32 _Dos_Format( dos_char **out, const dos_char *format, va_list args )
{
	register dos_int32 width, pad;
	register dos_int32 pc = 0;
	dos_char scr[2];

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			width = pad = 0;
			if (*format == '\0') break;
			if (*format == '%') goto out;
			if (*format == '-') {
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
			for ( ; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			if( *format == 's' ) {
				register dos_char *s = (dos_char *)va_arg( args, dos_int32 );
				pc += _Dos_FormatStr (out, s?s:"(null)", width, pad);
				continue;
			}
			if( *format == 'd' || *format == 'i' ) {
				pc += _Dos_FormatInt (out, va_arg( args, dos_int32 ), 10, 1, width, pad, 'a');
				continue;
			}
			if( *format == 'x' ) {
				pc += _Dos_FormatInt (out, va_arg( args, dos_int32 ), 16, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'X' ) {
				pc += _Dos_FormatInt (out, va_arg( args, dos_int32 ), 16, 0, width, pad, 'A');
				continue;
			}
			if( *format == 'u' ) {
				pc += _Dos_FormatInt (out, va_arg( args, dos_int32 ), 10, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'c' ) {
				/* dos_char are converted to dos_int32 then pushed on the stack */
				scr[0] = (dos_char)va_arg( args, dos_int32 );
				scr[1] = '\0';
				pc += _Dos_FormatStr (out, scr, width, pad);
				continue;
			}
		}
		else {
		out:
			_Dos_FormatChar (out, *format);
			++pc;
		}
	}
	if (out) **out = '\0';
	va_end( args );
	return pc;
}

dos_int32 Dos_Format(const dos_char *format, ...)
{
	va_list args;

	va_start( args, format );
	return _Dos_Format( 0, format, args );
}

dos_int32 Dos_FormatStr(dos_char *out, const dos_char *format, ...)
{
	va_list args;

	va_start( args, format );
	return _Dos_Format( &out, format, args );
}


dos_int32 Dos_FormatNStr( dos_char *buf, dos_uint32 count, const dos_char *format, ... )
{
	va_list args;

	( void ) count;

	va_start( args, format );
	return _Dos_Format( &buf, format, args );
}






