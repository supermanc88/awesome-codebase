/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_LINKAGE_H
#define _ASM_X86_LINKAGE_H

#include "stringify.h"

#undef notrace
#define notrace __attribute__((no_instrument_function))

#define __ASSEMBLY__
/*#define CONFIG_X86_64*/

#ifdef CONFIG_X86_32
#define asmlinkage CPP_ASMLINKAGE __attribute__((regparm(0)))
#endif /* CONFIG_X86_32 */

#ifdef __ASSEMBLY__

#if defined(CONFIG_X86_64) || defined(CONFIG_X86_ALIGNMENT_16)
#define __ALIGN		.p2align 4, 0x90
#define __ALIGN_STR	__stringify(__ALIGN)
#endif

#if defined(CONFIG_RETHUNK) && !defined(__DISABLE_EXPORTS) && !defined(BUILD_VDSO)
#define RET	jmp __x86_return_thunk
#else /* CONFIG_RETPOLINE */
#ifdef CONFIG_SLS
#define RET	ret; int3
#else
#define RET	ret
#endif
#endif /* CONFIG_RETPOLINE */

#else /* __ASSEMBLY__ */

#if defined(CONFIG_RETHUNK) && !defined(__DISABLE_EXPORTS) && !defined(BUILD_VDSO)
#define ASM_RET	"jmp __x86_return_thunk\n\t"
#else /* CONFIG_RETPOLINE */
#ifdef CONFIG_SLS
#define ASM_RET	"ret; int3\n\t"
#else
#define ASM_RET	"ret\n\t"
#endif
#endif /* CONFIG_RETPOLINE */

#endif /* __ASSEMBLY__ */

#define SYM_L_GLOBAL(name)			.globl name
#define SYM_L_WEAK(name)			.weak name
#define SYM_L_LOCAL(name)			/* nothing */
#define ASM_NL		 ;

#define SYM_ENTRY(name, linkage, align...)		\
	linkage(name) ASM_NL				\
	align ASM_NL					\
	name:

#define SYM_START(name, linkage, align...)		\
	SYM_ENTRY(name, linkage, align)

#define ALIGN __ALIGN
#define SYM_A_ALIGN				ALIGN
#define SYM_FUNC_START(name)				\
	SYM_START(name, SYM_L_GLOBAL, SYM_A_ALIGN)


#define SYM_FUNC_START_LOCAL(name)			\
	SYM_START(name, SYM_L_LOCAL, SYM_A_ALIGN)

#define SYM_END(name, sym_type)				\
	.type name sym_type ASM_NL			\
	.size name, .-name

#define SYM_T_FUNC				2

#define SYM_FUNC_END(name)				\
	SYM_END(name, SYM_T_FUNC)


#endif /* _ASM_X86_LINKAGE_H */

