
.section .rodata

.global roboto_mono_ttf
roboto_mono_ttf:
.incbin "RobotoMono-VariableFont_wght.ttf"
_roboto_mono_ttf_end:

.global roboto_mono_ttf_size
roboto_mono_ttf_size:
.quad _roboto_mono_ttf_end - roboto_mono_ttf
