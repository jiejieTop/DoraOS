#include <format.h>

static dos_int32 _Dos_Get_AtoI(const dos_char **str)
{
    dos_int32 n;
    for (n = 0; is_digit(**str); (*str)++)
        n = n * 10 + **str - '0';
    return n;
}

static dos_void _Dos_Buff_Put_Char(dos_char *buf, dos_size *pos, dos_size max, dos_char c)
{
    if(buf)
    {
        if (*pos < max)
            buf[(*pos)] = c;
        (*pos)++;
    }
    else
    {
        if(c != '\0')
            (void)outputchar(c);
    }
}


/**
 * Formats an integer number
 *  buf - buffer to print into
 *  len - current position in buffer
 *  maxlen - last valid position in buf
 *  num - number to print
 *  base - it's base
 *  width - how many spaces this should have; padding
 *  flags - above F flags
 */
static dos_void _Dos_Format_Int(dos_char *buf, dos_size *len, dos_size maxlen,
        dos_int64 num, dos_int32 base, dos_int32 width, dos_int32 flags)
{
    dos_char nbuf[64], sign = 0;
    dos_char altb[8]; // small buf for sign and #
    dos_int32 n = num;
    dos_int32 npad;           // number of pads
    dos_char pchar = ' ';   // padding character
    dos_char *digits = "0123456789ABCDEF";
    dos_char *ldigits = "0123456789abcdef";
    dos_int32 i, j;

    if (base < 2 || base > 16)
        return;
    if (flags & F_SMALL) digits = ldigits;
    if (flags & F_LEFT) flags &= ~F_ZEROPAD;

    if ((flags & F_SIGNED) && num < 0) {
        n = -num;
        sign = '-';
    } else if (flags & F_PLUS) {
        sign = '+';
    } else if (flags & F_SPACE)
        sign = ' ';

    i = 0;
    do {
        nbuf[i++] = digits[n % base];
        n = n / base;
    } while (n > 0);
   
    j = 0;
    if (sign) altb[j++] = sign;
    if (flags & F_ALTERNATE) {
        if (base == 8 || base == 16) {
            altb[j++] = '0';
            if (base == 16)
                altb[j++] = (flags & F_SMALL) ? 'x' : 'X';
        }
    }
    altb[j] = 0;

    npad = width > i + j ? width - i - j : 0;

    if (width > i + j)
        npad = width - i - j;

    if (npad > 0 && ((flags & F_LEFT) == 0)) {
        if (flags & F_ZEROPAD) {
            for (j = 0; altb[j]; j++)
                _Dos_Buff_Put_Char(buf, len, maxlen, altb[j]);
            altb[0] = 0;
        }
        while (npad-- > 0)
            _Dos_Buff_Put_Char(buf, len, maxlen, (flags & F_ZEROPAD) ? '0' : ' ');
    }
    for (j = 0; altb[j]; j++)
        _Dos_Buff_Put_Char(buf, len, maxlen, altb[j]);

    while (i-- > 0)
        _Dos_Buff_Put_Char(buf, len, maxlen, nbuf[i]);

    if (npad > 0 && (flags & F_LEFT))
        while(npad-- > 0)
            _Dos_Buff_Put_Char(buf, len, maxlen, pchar);
}

static dos_void _Dos_Format_Char(dos_char *buf, dos_size *pos, dos_size max, dos_char c,
        dos_int32 width, dos_int32 flags)
{
    dos_int32 npad = 0;
    if (width > 0) npad = width - 1;
    if (npad < 0) npad = 0;

    if (npad && ((flags & F_LEFT) == 0))
        while (npad-- > 0)
            _Dos_Buff_Put_Char(buf, pos, max, ' ');

    _Dos_Buff_Put_Char(buf, pos, max, c);

    if (npad && (flags & F_LEFT))
        while (npad-- > 0)
            _Dos_Buff_Put_Char(buf, pos, max, ' ');
}

/**
 * strlen()
 */
static dos_size _Dos_StrLen(dos_char *s)
{
    dos_size i;
    for (i = 0; *s; i++, s++)
        ;
    return i;
}

static dos_void _Dos_Format_Str(dos_char *buf, dos_size *pos, dos_size max, dos_char *s,
        dos_int32 width, dos_int32 flags)
{
    dos_int32 npad = 0;
    if (width > 0) npad = width - _Dos_StrLen(s);
    if (npad < 0) npad = 0;

    if (npad && ((flags & F_LEFT) == 0))
        while (npad-- > 0)
            _Dos_Buff_Put_Char(buf, pos, max, ' ');

    while (*s)
        _Dos_Buff_Put_Char(buf, pos, max, *s++);

    if (npad && (flags & F_LEFT))
        while (npad-- > 0)
            _Dos_Buff_Put_Char(buf, pos, max, ' ');
}




/***********************************************************************************************************************/


dos_int32 Dos_FormatOut(const dos_char *format, ...)
{
	va_list args;

	va_start( args, format );
	return Dos_FormatNStr(DOS_NULL, 0, format, args);
}


/**
 * Shrinked down, vsnprintf implementation.
 *  This will not handle floating numbers (yet).
 */
dos_int32 Dos_FormatNStr(dos_char *buf, dos_size size, const dos_char *fmt, va_list ap)
{
    dos_size n = 0;
    dos_char c, *s;
    dos_char state = 0;
    dos_int64 num;
    dos_int32 base;
    dos_int32 flags, width, precision, lflags;

    if (!buf) size = 0;

    for (;;) {
        c = *fmt++;
        if (state == S_DEFAULT) {
            if (c == '%') {
                state = S_FLAGS;
                flags = 0;
            } else {
                _Dos_Buff_Put_Char(buf, &n, size, c);
            }
        } else if (state == S_FLAGS) {
            switch (c) {
                case '#': flags |= F_ALTERNATE; break;
                case '0': flags |= F_ZEROPAD; break;
                case '-': flags |= F_LEFT; break;
                case ' ': flags |= F_SPACE; break;
                case '+': flags |= F_PLUS; break;
                case '\'':
                case 'I' : break; // not yet used
                default: fmt--; width = 0; state = S_WIDTH;
            }
        } else if (state == S_WIDTH) {
            if (c == '*') {
                width = va_arg(ap, dos_int32);
                if (width < 0) {
                    width = -width;
                    flags |= F_LEFT;
                }
            } else if (is_digit(c) && c > '0') {
                fmt--;
                width = _Dos_Get_AtoI(&fmt);
            } else {
                fmt--;
                precision = -1;
                state = S_PRECIS;
            }
        } else if (state == S_PRECIS) {
            // Ignored for now, but skip it
            if (c == '.') {
                if (is_digit(*fmt))
                    precision = _Dos_Get_AtoI(&fmt);
                else if (*fmt == '*')
                    precision = va_arg(ap, dos_int32);
                precision = precision < 0 ? 0 : precision;
            } else
                fmt--;
            lflags = 0;
            state = S_LENGTH;
        } else if (state == S_LENGTH) {
            switch(c) {
                case 'h': lflags = lflags == L_CHAR ? L_SHORT : L_CHAR; break;
                case 'l': lflags = lflags == L_LONG ? L_LLONG : L_LONG; break;
                case 'L': lflags = L_DOUBLE; break;
                default: fmt--; state = S_CONV;
            }
        } else if (state == S_CONV) {
            if (c == 'd' || c == 'i' || c == 'o' || c == 'b' || c == 'u' 
                    || c == 'x' || c == 'X') {
                if (lflags == L_LONG)
                    num = va_arg(ap, dos_int32);
                else if (lflags & (L_LLONG | L_DOUBLE))
                    num = va_arg(ap, dos_int64);
                else if (c == 'd' || c == 'i')
                    num = va_arg(ap, dos_int32);
                else
                    num = (dos_uint32) va_arg(ap, dos_int32);

                base = 10;
                if (c == 'd' || c == 'i') {
                    flags |= F_SIGNED;
                } else if (c == 'x' || c == 'X') {
                    flags |= c == 'x' ? F_SMALL : 0;
                    base = 16;
                } else if (c == 'o') {
                    base = 8;
                } else if (c == 'b') {
                    base = 2;
                }
                _Dos_Format_Int(buf, &n, size, num, base, width, flags);
            } else if (c == 'p') {
                num = (dos_int32) va_arg(ap, dos_void *);
                base = 16;
                flags |= F_SMALL | F_ALTERNATE;
                _Dos_Format_Int(buf, &n, size, num, base, width, flags);
            } else if (c == 's') {
                s = va_arg(ap, dos_char *);
                if (!s)
                    s = "(null)";
                _Dos_Format_Str(buf, &n, size, s, width, flags);
            } else if (c == 'c') {
                c = va_arg(ap, dos_int32);
                _Dos_Format_Char(buf, &n, size, c, width, flags);
            } else if (c == '%') {
                _Dos_Buff_Put_Char(buf, &n, size, c);
            } else {
                _Dos_Buff_Put_Char(buf, &n, size, '%');
                _Dos_Buff_Put_Char(buf, &n, size, c);
            }
            state = S_DEFAULT;
        }
        if (c == 0)
            break;
    }
    n--;
    if (n < size)
        buf[n] = 0;
    else if (size > 0)
        buf[size - 1] = 0;

    return n;
}







