#include <lib.h>

const int CYCLE = 1e8;
const char *s = "IDLE!";

int main() {
	while (1) {
		debugf(s);
		for (int i = 0; i < CYCLE; i++);
	}
	return 0;
}
