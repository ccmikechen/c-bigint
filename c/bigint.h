typedef unsigned char digit;

typedef struct {
  digit* number;
  int length;
  int sign;
} bigint;

void bigint_new(bigint* a);

void bigint_newf(bigint* a, char* num);

bigint* bigint_newc(char* num);

void bigint_copy(bigint* from, bigint* to);

bigint* bigint_copyc(bigint* from);

void bigint_delete(bigint* a);

bigint* bigint_add(bigint* a, bigint* b);

bigint* bigint_sub(bigint* a, bigint* b);

bigint* bigint_mul(bigint* a, bigint* b);

bigint* bigint_div(bigint* a, bigint* b);

bigint* bigint_mod(bigint* a, bigint* b);

bigint* bigint_pow(bigint* a, int n);

bigint* bigint_abs(bigint* a);

void bigint_str(bigint* a, char* s);

char* bigint_strc(bigint* a);

int bigint_compare(bigint* a, bigint* b);

int bigint_equal(bigint* a, bigint* b);
