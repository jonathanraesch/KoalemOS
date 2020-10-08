#include "libc/ctype.h"


int isalnum(int ch) {
	if((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
		return 1;
	}
	return 0;
}

int isdigit(int ch) {
	if(ch >= '0' && ch <= '9') {
		return 1;
	}
	return 0;
}
