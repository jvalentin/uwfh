/*
 * itoa.c
 */

static char *i2a(unsigned i, char *a, unsigned r)
  {
    if (i/r > 0) a = i2a(i/r,a,r);
    *a = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[i%r];
    return a+1;
  }

  char *itoa(int i, char *a, int r)
  {
    if ((r < 2) || (r > 36)) r = 10;
    if (i < 0)
    {
        *a = '-';
        *i2a(-(unsigned)i,a+1,r) = 0;
    }
    else *i2a(i,a,r) = 0;
    return a;
  } 
