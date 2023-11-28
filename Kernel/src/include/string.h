#ifndef _STRING_INC_H
#define _STRING_INC_H

#include <stdint.h>
#include <types.h>


int strlen(const char *s) {
    int len = 0;
    while (*s++)
        len++;
    return len;
}

char *search_token(char *string, char *delim)
{
  static char *remember = NULL;
  int string_length = 0;
  int i= 0;
  int search_hit=0;

  if(delim == NULL)
    return NULL;

  if((string == NULL) && (remember == NULL))
    return NULL;

  if(string == NULL)
    string = remember;

  string_length = strlen(string) + 1;

  for(i=0;i<string_length;i++)
  {
    if(string[i] == delim[0])
    {
      search_hit = 1 ;
      break;
    }
  }

  if(search_hit != 1)
  {
    remember = NULL;
    return string;
  }

  string[i] = '\0';

  if((string+i+1) != NULL)
    remember = string + i + 1;
  else
    remember = NULL;

  return string;
}

void strsplit(char* return_list[], char str[], char* delim)
{
  unsigned count = 0;
  char *token;

  token = search_token(str,delim);

  while(token != NULL)
  {
    return_list[count] = token;
    token = search_token(NULL, delim);
    count++;
  }
}


uint32_t digit_count(int num)
{
  uint32_t count = 0;
  
  if(num == 0)
    return 1;
  
  while(num > 0)
  {
    count++;
    num = num/10;
  }
  
  return count;
}

int atoi(char *str)
{
  int res = 0;

  for (int i = 0; str[i] != '\0'; ++i)
  {
    if (str[i]> '9' || str[i]<'0')
      return -1;

    res = res*10 + str[i] - '0';
  }

  return res;
}

void itoa2(int num, char *number)
{
  int dgcount = digit_count(num);
  int index = dgcount - 1;
  char x;
  
  if(num == 0 && dgcount == 1)
  {
    number[0] = '0';
    number[1] = '\0';
  }
  else
  {
    while(num != 0)
    {
      x = num % 10;
      number[index] = x + '0';
      index--;
      num = num / 10;
    }
    
    number[dgcount] = '\0';
  }
}

void *memset(void *dst, char c, uint32 n) {
    char *temp = dst;
    for (; n != 0; n--) *temp++ = c;
    return dst;
}

void *memcpy(void *dst, const void *src, uint32 n) {
    char *ret = dst;
    char *p = dst;
    const char *q = src;
    while (n--)
        *p++ = *q++;
    return ret;
}

int memcmp(uint8 *s1, uint8 *s2, uint32 n) {
    while (n--) {
        if (*s1 != *s2)
            return 0;
        s1++;
        s2++;
    }
    return 1;
}

int strcmp(const char *s1, char *s2) {
    int i = 0;

    while ((s1[i] == s2[i])) {
        if (s2[i++] == 0)
            return 0;
    }
    return 1;
}

int strcpy(char *dst, const char *src) {
    int i = 0;
    while ((*dst++ = *src++) != 0)
        i++;
    return i;
}

void strcat(char *dest, const char *src) {
    char *end = (char *)dest + strlen(dest);
    memcpy((void *)end, (void *)src, strlen(src));
    end = end + strlen(src);
    *end = '\0';
}


int isspace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}

int isalpha(char c) {
    return (((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')));
}

char upper(char c) {
    if ((c >= 'a') && (c <= 'z'))
        return (c - 32);
    return c;
}

char lower(char c) {
    if ((c >= 'A') && (c <= 'Z'))
        return (c + 32);
    return c;
}

void itoa(char *buf, int base, int d) {
    char *p = buf;
    char *p1, *p2;
    unsigned long ud = d;
    int divisor = 10;

    /* If %d is specified and D is minus, put ‘-’ in the head. */
    if (base == 'd' && d < 0) {
        *p++ = '-';
        buf++;
        ud = -d;
    } else if (base == 'x')
        divisor = 16;

    /* Divide UD by DIVISOR until UD == 0. */
    do {
        int remainder = ud % divisor;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
    } while (ud /= divisor);

    /* Terminate BUF. */
    *p = 0;

    /* Reverse BUF. */
    p1 = buf;
    p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
}

#endif