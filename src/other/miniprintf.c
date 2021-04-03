#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdarg.h>

int test(char *fmt, ...)
{
    va_list ap;
    char *s, c, *p;
    int d;
    printf("%s : %s\n", __FUNCTION__, fmt);
    va_start(ap, fmt);
    for(p = fmt; *p != '\0'; p++)
    {
        if(*p != '%')
        {
            putchar(*p);
            continue;
        }
        switch (*++p) {
            case 's':              /* string */
                s = va_arg(ap, char *);
                printf("%s", s);
                break;
            case 'd':              /* int */
                d = va_arg(ap, int);
                printf("%d", d);
                break;
            case 'c':              /* char */
                /* need a cast here since va_arg only
                 * takes fully promoted types */
                c = (char) va_arg(ap, int);
                printf("%c", c);
                break;
        }
    }
   va_end(ap);
}

int main()
{
    test("mohit_singh %d %c %s %d", 2, 'c', "string", 32);
    return 0;
}


