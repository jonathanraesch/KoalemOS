#pragma once


#define EDOM	1
#define EILSEQ	2
#define ERANGE	3

#define EINVAL	22

extern int __errno;
#define errno __errno
