#include <stdlib.h>
char * ltoa(long val, char *buf,int base)   
{
  ldiv_t r;                                 /* result of val / base  */
  if (base > 36 || base < 2)          /* no conversion if wrong base */
    {
      *buf = '\0';
      return buf;
    }
  if (val < 0)
    *buf++ = '-';
  r = ldiv (labs(val), base);
  /* output digits of val/base first */
  if (r.quot > 0)
    buf = ltoa ( r.quot, buf, base);
  
  /* output last digit */
  
  *buf++ = "0123456789abcdefghijklmnopqrstuvwxyz"[(int)r.rem];
  *buf   = '\0';
  return buf;
}
