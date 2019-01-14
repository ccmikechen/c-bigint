#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bigint.h"

#define CHAR_ZERO 48
#define MAX(X, Y) (X > Y ? X : Y)

static int bigint_is_zero(bigint* a) {
  int i;
  for (i = 0; i < a->length; i++) {
    if (a->number[i] > 0) {
      return 0;
    }
  }
  return 1;
}

static bigint* bigint_mul10n(bigint* a, int n) {
  if (bigint_is_zero(a)) {
    return bigint_copyc(a);
  }
  bigint* out = (bigint*)malloc(sizeof(bigint));
  int len = a->length + n;
  out->length = len;
  out->sign = a->sign;
  out->number = (digit*)malloc(len);
  int i;
  for (i = 0; i < a->length; i++) {
    out->number[i] = a->number[i];
  }
  for (; i < len; i++) {
    out->number[i] = 0;
  }
  return out;
}

void bigint_new(bigint* a) {
  a->number = (digit*)malloc(1);
  a->number[0] = 0;
  a->length = 1;
  a->sign = 1;
}

void bigint_newf(bigint* a, char* num) {
  int len = strlen(num);
  int p = 0;
  a->sign = 1;
  if (num[0] == '-') {
    a->sign = -1;
    len--;
    p++;
  }

  int i;
  for (i = 0; i < len; i++) {
    if (num[i + p] > CHAR_ZERO) {
      break;
    }
  }
  if (i == len) {
    a->number = (digit*)malloc(1);
    a->number[0] = 0;
    a->length = 1;
    return;
  }

  a->length = len - i;
  a->number = (digit*)malloc(len - i);
  int j;
  for (j = 0; i < len; i++, j++) {
    a->number[j] = num[i + p] - CHAR_ZERO;
  }
}

bigint* bigint_newc(char* num) {
  bigint* out = (bigint*)malloc(sizeof(bigint));
  bigint_newf(out, num);
  return out;
}

void bigint_copy(bigint* from, bigint* to) {
  if (bigint_is_zero(from)) {
    bigint_newf(to, "0");
    return;
  }
  to->sign = from->sign;
  to->length = from->length;
  to->number = (digit*)malloc(from->length);
  int i = 0;
  while (i < from->length) {
    to->number[i] = from->number[i];
    i++;
  }
  while (to->number[0] == 0) {
    to->number++;
    to->length--;
  }
}

bigint* bigint_copyc(bigint* from) {
  bigint* to = (bigint*)malloc(sizeof(bigint));
  bigint_copy(from, to);
  return to;
}

void bigint_delete(bigint* a) {
  free(a->number);
  free(a);
}

bigint* bigint_add(bigint* a, bigint* b) {
  if (a->sign > b->sign) {
    bigint* tmp = bigint_copyc(b);
    tmp->sign = 1;
    bigint* result = bigint_sub(a, tmp);
    bigint_delete(tmp);
    return result;
  }
  if (a->sign < b->sign) {
    bigint* tmp = bigint_copyc(a);
    tmp->sign = 1;
    bigint* result = bigint_sub(b, tmp);
    bigint_delete(tmp);
    return result;
  }

  int len = MAX(a->length, b->length) + 1;
  bigint* out = (bigint*)malloc(sizeof(bigint));
  out->length = len;
  out->sign = a->sign;
  out->number = (digit*)malloc(len);
  
  int i;
  char t = 0;
  for (i = 1; i <= len; i++) {
    t /= 10;
    if (i <= a->length) {
      t += a->number[a->length - i];
    }
    if (i <= b->length) {
      t += b->number[b->length - i];
    }
    out->number[len - i] = t % 10;
  }
  if (out->length > 1 && out->number[0] == 0) {
    out->number++;
    out->length--;
  }
  return out;
}

bigint* bigint_sub(bigint* a, bigint* b) {
  if (a->sign > b->sign) {
    bigint* tmp = bigint_copyc(b);
    tmp->sign = 1;
    bigint* result = bigint_add(a, tmp);
    bigint_delete(tmp);
    return result;
  }
  if (a->sign < b->sign) {
    bigint* tmp = bigint_copyc(b);
    tmp->sign = -1;
    bigint* result = bigint_add(a, tmp);
    bigint_delete(tmp);
    return result;
  }
  if ((bigint_compare(a, b) * a->sign) == -1) {
    bigint* out = bigint_sub(b, a);
    out->sign = -1;
    return out;
  }
  
  int len = a->length;
  bigint* out = (bigint*)malloc(sizeof(bigint));
  out->length = len;
  out->sign = a->sign;
  out->number = (digit*)malloc(len);

  int i;  
  char t = 0;
  for (i = 1; i <= len; i++) {
    t = t < 0 ? -1 : 0;
    if (i <= a->length) {
      t += a->number[a->length - i];
    }
    if (i <= b->length) {
      t -= b->number[b->length - i];
    }
    out->number[len - i] = (t + 10) % 10;
  }
  bigint* result = bigint_copyc(out);
  bigint_delete(out);
  return result;
}


bigint* bigint_mul(bigint* a, bigint* b) {
  if (a->length < b->length) {
    return bigint_mul(b, a);
  }
  bigint* out = bigint_newc("0");
  int i;
  for (i = 0; i < b->length; i++) {
    int n = b->number[b->length - i - 1];
    
    bigint* tmp = bigint_newc("0");
    while (n > 0) {
      tmp = bigint_add(tmp, a);
      n--;
    }
    tmp = bigint_mul10n(tmp, i);
    out = bigint_add(out, tmp);
    bigint_delete(tmp);
  }
  out->sign = a->sign * b->sign;
  
  return out;
}



static void bigint_div_and_mod(bigint* a, bigint* b, bigint* ans, bigint* mod) {
  if (bigint_is_zero(b)) {
    fprintf(stderr, "bigint error: Divide by zero\n");
    exit(1);
  }
  bigint* c = bigint_abs(b);
  bigint* tmp = bigint_newc("0");
  bigint* out = (bigint*)malloc(sizeof(bigint));
  out->number = (digit*)malloc(a->length);
  out->length = a->length;
  int i;
  char j;
  for (i = 0; i < a->length; i++) {
    tmp = bigint_mul10n(tmp, 1);
    tmp->number[tmp->length - 1] = a->number[i];
    j = 0;
    while (bigint_compare(tmp, c) >= 0) {
      tmp = bigint_sub(tmp, c);
      j++;
    }
    out->number[i] = j;
  }
  
  out->sign = a->sign * b->sign;
  tmp->sign = a->sign;
  bigint_copy(out, ans);
  bigint_copy(tmp, mod);
}

bigint* bigint_div(bigint* a, bigint* b) {
  bigint* ans = (bigint*)malloc(sizeof(bigint));
  bigint* mod = (bigint*)malloc(sizeof(bigint));
  bigint_div_and_mod(a, b, ans, mod);
  return ans;
}

bigint* bigint_mod(bigint* a, bigint* b) {
  bigint* ans = (bigint*)malloc(sizeof(bigint));
  bigint* mod = (bigint*)malloc(sizeof(bigint));
  bigint_div_and_mod(a, b, ans, mod);
  return mod;
}

bigint* bigint_pow(bigint* a, int n) {
  bigint* out = bigint_newc("1");
  while (n > 0) {
    out = bigint_mul(out, a);
    n--;
  }
  return out;
}

bigint* bigint_abs(bigint* a) {
  bigint* out = bigint_copyc(a);
  if (out->sign == -1) {
    out->sign = 1;
  }
  return out;
}

void bigint_str(bigint* a, char* s) {  
  char* out = (char*)malloc(a->length + 1);
  int i;
  for (i = 0; i < a->length; i++) {
    out[i] = a->number[i] + CHAR_ZERO;
  }
  out[i] = '\0';
  if (a->sign == -1 && !(a->length == 1 && a->number[0] == 0)) {
    strcpy(s, "-");
  }
  strcat(s, out);
}

char* bigint_strc(bigint* a) {
  int len = a->length;
  if (a->sign == -1) {
    len++;
  }
  char* s = (char*)malloc(len);
  bigint_str(a, s);
  return s;
}

int bigint_compare(bigint* a, bigint* b) {
  if (a->length == 1 && a->number[0] == 0 &&
      b->length == 1 && b->number[0] == 0) {
    return 0;
  }
  if (a->sign > b->sign) {
    return 1;
  }
  if (a->sign < b->sign) {
    return -1;
  }
  if (a->length > b->length) {
    return a->sign;
  }
  if (a->length < b->length) {
    return -a->sign;
  }
  int i;
  for (i = 0; i < a->length; i++) {
    if (a->number[i] > b->number[i]) {
      return a->sign;
    }
    if (a->number[i] < b->number[i]) {
      return -a->sign;
    }
  }
  return 0;
}

int bigint_equal(bigint* a, bigint* b) {
  return bigint_compare(a, b) == 0;
}
