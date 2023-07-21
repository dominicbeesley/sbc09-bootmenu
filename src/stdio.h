
#ifndef __STDIO_H__
#define __STDIO_H__

typedef int size_t;


typedef void * va_list;
typedef void FILE;

FILE *stdout = 0;
FILE *stdin = 0;

#define __va_rounded_size(TYPE)  \
  ( ( (sizeof (TYPE) + sizeof (int) - 1) / sizeof (int) ) * sizeof (int)  )

#define va_start(AP, LASTARG)                       \
 (AP = ((char *) &(LASTARG)) + __va_rounded_size(LASTARG))

#define va_arg(AP, TYPE)                        \
  (AP += __va_rounded_size (TYPE),                  \
  *((TYPE *) (AP - __va_rounded_size(TYPE))))


#define va_end(AP)

extern int vfprintf(FILE *file, char const *fmt, va_list arg);
extern int printf(char const *fmt, ...);
extern int fprintf(FILE *file, char const *fmt, ...);

extern int fputc(int c, FILE *stream);
extern int fputs(const char *str, FILE *stream);

extern int fgetc(FILE *stream);
extern char *fgets(char *buf, size_t num, FILE *stream);

extern int putc(int c, FILE *stream);
extern int puts(const char *str);

extern int getchar(void);

#define EOF -1

#endif