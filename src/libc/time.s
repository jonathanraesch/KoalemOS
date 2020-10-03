

%ifdef __klibc__

global clock
clock:
	rdtsc
	lfence
	shl rdx, 32
	or rax, rdx
	ret

%endif
