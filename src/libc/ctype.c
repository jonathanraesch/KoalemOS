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

int isspace(int ch) {
	if(ch == ' ' || ch == '\f' || ch == '\n' || ch == '\r' || ch == '\t' || ch == '\v') {
		return 1;
	}
	return 0;
}
