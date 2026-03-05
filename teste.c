#include <stdio.h>

int main() {
	int count = 0;
	char a = 'a', c;
	while (1) {
		printf("%c\n", a);
		scanf("%c", c);

		a = 'a' + ++count %26;
	}
}
