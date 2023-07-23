
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "uart.h"

unsigned char last_cr = 0;

int fgetc(FILE *stream) {
    char ret = uart_readc();
    if (ret == '\r')
    {
        last_cr = 1;
        ret = '\n';
    } else {

        last_cr = '0';
        if (ret == '\n') {
            if (last_cr)
                ret = fgetc(stream);
        }
    }

    uart_writec(ret);

    return ret;
}

int fputc(int ch, FILE *stream) {
    return uart_writec(ch);
}

int putchar(int c) {
    if (uart_writec(c) < 0)
        return -1;
    else
        return c;
}

int fputs(const char *str, FILE *stream) {
    char c;
    while ((c = *str++)) {
        int r = fputc(c, stream);
        if (r < 0)
            return r;
    }
    return 0;
}

/* gcc needs these to be real fn's not macros it seems */
int putc(int ch, FILE *stream) {
    return fputc(ch, stream);
}

int puts(const char *str) {
    int ret = fputs(str, stdout);
    if (ret >= 0)
        ret = putc('\n', stdout);
    return ret;
}

int mpad(int pad, char padch, const char *p) {
    int l = strlen(p);
    for (int i = l ; i < pad; i++)
    {
        putc(padch, stdout);
    }
    return l;
}

/***************************** 
 * printf routines 
 * 
 * adapted from https://stackoverflow.com/a/41800327/1428249
*/

int vfprintf(FILE *file, char const *fmt, va_list arg) {

    int int_temp;
    long long_temp;
    char char_temp;
    char *string_temp;

    char ch;
    int length = 0;

    char buffer[17];

    while ((ch = *fmt++)) {
        if ( '%' == ch ) {
            // skip any field specifiers
            int fmt_long = 0;
            int pad = -1;
            int pad2 = 0;
            int b4 = 1;
            if (*fmt == '-') {
                b4 = 0;
                fmt++;
            }
            char padch = ' ';
            do {
                ch = *fmt++;
                if (ch == 'l' || ch == 'L') 
                    fmt_long = 1;
                else if (!pad2 & (ch >= '0' && ch <= '9')) {
                    if (pad < 0)
                    {
                        pad = ch - '0';
                        if (ch == '0')
                            padch = '0';
                    } else {
                        pad = (pad * 10) + (ch - '0');
                    }
                } else if (ch == '.') {
                    pad2 = 1;
                }
            } while ((ch >= '0' && ch <= '9') || ch == '.' || ch == 'l' || ch == 'L');

            switch (ch) {
                /* %% - print out a single %    */
                case '%':
                    fputc('%', file);
                    length++;
                    break;

                /* %c: print out a character    */
                case 'c':
                    char_temp = va_arg(arg, int);
                    fputc(char_temp, file);
                    length++;
                    break;

                /* %s: print out a string       */
                case 's':
                    string_temp = va_arg(arg, char *);
                    if (b4)
                        length += mpad(pad, padch, string_temp);
                    const char *p = string_temp;
                    int n = 0;
                    while (*p && (pad == -1 || n < pad ))
                    {
                        fputc(*p++, file);
                        n++;
                    }
                    length += n;
                    if (!b4)
                        length += mpad(pad, padch, string_temp);
                    break;

                /* %d: print out an int         */
                case 'd':
                    if (fmt_long)                    
                    {
                        long_temp = va_arg(arg, long);
                        ltoa(long_temp, buffer, 10);
                    } else {
                        int_temp = va_arg(arg, int);
                        itoa(int_temp, buffer, 10);
                    }
                    if (b4)
                        length += mpad(pad, padch, buffer);
                    fputs(buffer, file);
                    if (!b4)
                        length += mpad(pad, padch, buffer);
                    break;

                /* %x: print out an int in hex  */
                case 'x':
                case 'X':
                    if (fmt_long)                    
                    {
                        long_temp = va_arg(arg, long);
                        ltoa(long_temp, buffer, 16);
                    } else {
                        int_temp = va_arg(arg, int);
                        itoa(int_temp, buffer, 16);
                    }
                    if (ch == 'x') {
                        char *p = buffer;
                        while ((*p = tolower(*p))) p++;
                    }
                    if (b4)
                        length += mpad(pad, padch, buffer);
                    fputs(buffer, file);
                    if (!b4)
                        length += mpad(pad, padch, buffer);
                    break;
                default:
                    fmt--; // step back on mistake, not sure if that's correct but it save crapping out if % is last char of format string
                    break;
            }
        }
        else {
            putc(ch, file);
            length++;
        }
    }
    return length;
}

int printf(char const *fmt, ...) {
    va_list arg;
    int length;

    va_start(arg, fmt);
    length = vfprintf(stdout, fmt, arg);
    va_end(arg);
    return length;
}

int fprintf(FILE *file, char const *fmt, ...) {
    va_list arg;
    int length;

    va_start(arg, fmt);
    length = vfprintf(file, fmt, arg);
    va_end(arg);
    return length;
}


char *fgets(char * s, size_t n, FILE * stream)
{
    register char *p;
    int c;

    if (n <= 0) {
        goto ERROR;
    }

    p = s;

    while (--n) {
        if ((c = fgetc(stream)) == EOF) {
            break;
        }
        if ((*p++ = c) == '\n') {
            break;
        }
    }

    if (p > s) {
        *p = 0;
        return s;
    }

 ERROR:
    return NULL;
}

int getchar(void) { return fgetc(stdin); }
