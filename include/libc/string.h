#pragma once
#include <stddef.h>


char* strcpy(char* restrict dest, const char* restrict src);
char* strncpy(char* restrict dest, const char* restrict src, size_t count);
char* strcat(char* restrict dest, const char* restrict src);

size_t strlen(const char* str);
int strcmp(const char* lhs, const char* rhs);
int strncmp(const char* lhs, const char* rhs, size_t count);
char* strchr(const char* str, int ch);
char* strrchr(const char* str, int ch);
char* strstr(const char* str, const char* substr);

int memcmp(const void* lhs, const void* rhs, size_t count);
void* memset(void* dest, int ch, size_t count);
void* memcpy(void* restrict dest, const void* restrict src, size_t count);
void* memmove(void* dest, const void* src, size_t count);
