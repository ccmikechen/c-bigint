#include <stdio.h>

#define BIGINT long long

BIGINT power_mod(n, m, p) {
	// n ^ m mod p
	BIGINT sum = n;
	BIGINT t = m;
	while (--t > 0) {
		sum = (sum * (n % p)) % p; 
	}
	return sum;
}

BIGINT calc(x, y) {
	// 2x^5 + 4x^3 + 7 mod y
	BIGINT sum_a = ((2 % y) * power_mod(x, 5, y)) % y;
	BIGINT sum_b = ((4 % y) * power_mod(x, 3, y)) % y;
	BIGINT sum_c = 7 % y;
	printf("sum_a = %d, sum_b = %d\n", sum_a, sum_b);
	return (sum_a + sum_b + sum_c) % y; 
	
}

int main() {
	BIGINT x, y;
	while (scanf("%ld %ld", &x, &y) == 2) {
		BIGINT answer = calc(x, y);
		printf("Answer: %ld\n", answer);	
	}
	
	return 0;
}
