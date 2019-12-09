/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-08-07 23:38:28
 * @LastEditTime: 2019-12-08 23:24:01
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include <format.h>


static dos_int32 _dos_get_atoi(const dos_char **str)
{
    dos_int32 n;
    for (n = 0; is_digit(**str); (*str)++)
        n = n * 10 + **str - '0';
    return n;
}

static dos_void _dos_buff_put_char(dos_char *buf, dos_size *pos, dos_size max, dos_char c)
{
    if (*pos < max)
        buf[(*pos)] = c;
    (*pos)++;
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
static dos_void _dos_format_int(dos_char *buf, dos_size *len, dos_size maxlen,
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
                _dos_buff_put_char(buf, len, maxlen, altb[j]);
            altb[0] = 0;
        }
        while (npad-- > 0)
            _dos_buff_put_char(buf, len, maxlen, (flags & F_ZEROPAD) ? '0' : ' ');
    }
    for (j = 0; altb[j]; j++)
        _dos_buff_put_char(buf, len, maxlen, altb[j]);

    while (i-- > 0)
        _dos_buff_put_char(buf, len, maxlen, nbuf[i]);

    if (npad > 0 && (flags & F_LEFT))
        while(npad-- > 0)
            _dos_buff_put_char(buf, len, maxlen, pchar);
}

static dos_void _dos_format_char(dos_char *buf, dos_size *pos, dos_size max, dos_char c,
        dos_int32 width, dos_int32 flags)
{
    dos_int32 npad = 0;
    if (width > 0) npad = width - 1;
    if (npad < 0) npad = 0;

    if (npad && ((flags & F_LEFT) == 0))
        while (npad-- > 0)
            _dos_buff_put_char(buf, pos, max, ' ');

    _dos_buff_put_char(buf, pos, max, c);

    if (npad && (flags & F_LEFT))
        while (npad-- > 0)
            _dos_buff_put_char(buf, pos, max, ' ');
}

/**
 * strlen()
 */
static dos_size _dos_strlen(dos_char *s)
{
    dos_size i;
    for (i = 0; *s; i++, s++)
        ;
    return i;
}

static dos_void _dos_format_str(dos_char *buf, dos_size *pos, dos_size max, dos_char *s,
        dos_int32 width, dos_int32 flags)
{
    dos_int32 npad = 0;
    if (width > 0) npad = width - _dos_strlen(s);
    if (npad < 0) npad = 0;

    if (npad && ((flags & F_LEFT) == 0))
        while (npad-- > 0)
            _dos_buff_put_char(buf, pos, max, ' ');

    while (*s)
        _dos_buff_put_char(buf, pos, max, *s++);

    if (npad && (flags & F_LEFT))
        while (npad-- > 0)
            _dos_buff_put_char(buf, pos, max, ' ');
}




/***********************************************************************************************************************/

/**
 * Shrinked down, vsnprintf implementation.
 *  This will not handle floating numbers (yet).
 */
dos_int32 dos_format_nstr(dos_char *buf, dos_size size, const dos_char *fmt, va_list ap)
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
                _dos_buff_put_char(buf, &n, size, c);
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
                width = _dos_get_atoi(&fmt);
            } else {
                fmt--;
                precision = -1;
                state = S_PRECIS;
            }
        } else if (state == S_PRECIS) {
            // Ignored for now, but skip it
            if (c == '.') {
                if (is_digit(*fmt))
                    precision = _dos_get_atoi(&fmt);
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
                _dos_format_int(buf, &n, size, num, base, width, flags);
            } else if (c == 'p') {
                num = (dos_int32) va_arg(ap, dos_void *);
                base = 16;
                flags |= F_SMALL | F_ALTERNATE;
                _dos_format_int(buf, &n, size, num, base, width, flags);
            } else if (c == 's') {
                s = va_arg(ap, dos_char *);
                if (!s)
                    s = "(null)";
                _dos_format_str(buf, &n, size, s, width, flags);
            } else if (c == 'c') {
                c = va_arg(ap, dos_int32);
                _dos_format_char(buf, &n, size, c, width, flags);
            } else if (c == '%') {
                _dos_buff_put_char(buf, &n, size, c);
            } else {
                _dos_buff_put_char(buf, &n, size, '%');
                _dos_buff_put_char(buf, &n, size, c);
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







