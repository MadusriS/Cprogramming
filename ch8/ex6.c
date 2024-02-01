/*
    Exercise 8.6. The standard library function calloc(n, size) returns a pointer to n objects of size size, with the storage initialised to zero. Write calloc, by calling malloc or by modifying it.
*/

#include <stdlib.h>
#include <string.h>
void *mycalloc(size_t nmemb, size_t size)
{
  void *Result = NULL;

  /* use malloc to get the memory */
  Result = malloc(nmemb * size);

  /* and clear the memory on successful allocation */
  if(NULL != Result)
  {
    memset(Result, 0x00, nmemb * size);/*
memset(Result, 0x00, nmemb * size);: Uses memset to set all bytes in the allocated memory block to zero. This step initializes the memory to zero.
memset syntax:void *memset(void *s, int c, size_t n);
s: Pointer to the memory block to fill.
c: Value to be set. This is passed as an int, but it is typically used as an unsigned char.
n: Number of bytes to be set to the value.

*/
  }

  /* and return the result */
  return Result;
}

/* simple test driver, by RJH */

#include <stdio.h>

int main(void)
{
  int *p = NULL;
  int i = 0;

  p = mycalloc(100, sizeof *p);
  if(NULL == p)
  {
    printf("mycalloc returned NULL.\n");
  }
  else
  {
    for(i = 0; i < 100; i++)
    {
      printf("%08X ", p[i]);/*Prints each integer in hexadecimal format with leading zeros.*/
      if(i % 8 == 7)
      {
	printf("\n");
      /*Prints each integer in hexadecimal format with leading zeros.*/
      }
    }
    printf("\n");
    free(p);/*Prints each integer in hexadecimal format with leading zeros.*/
  }

  return 0;
}
