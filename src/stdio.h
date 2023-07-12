
#ifndef __STDIO_H__
#define __STDIO_H__

typedef void * va_list;
typedef void * FILE;

FILE *stdout;
FILE *stdin;

#define __va_rounded_size(TYPE)  \
  ( ( (sizeof (TYPE) + sizeof (int) - 1) / sizeof (int) ) * sizeof (int)  )

#define va_start(AP, LASTARG)                       \
 (AP = ((char *) &(LASTARG)))

#define va_arg(AP, TYPE)                        \
  (AP -= __va_rounded_size (TYPE),                  \
  *((TYPE *) (AP)))


#define va_end(AP)

extern int vfprintf(FILE *file, char const *fmt, va_list arg);
extern int printf(char const *fmt, ...);
extern int fprintf(FILE *file, char const *fmt, ...);

extern int fputc(int c, FILE *stream);
extern int fputs(const char *str, FILE *stream);

#define putc(c, f) fputc(c, f)
#define puts(c, f) fputs(c, f)

#endif