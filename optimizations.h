/*************************************************/
/*	This file checks if optimizations	*/
/*	(-O2, -O3, -Ofast) are being used	*/
/* Made by Salonia Matteo <saloniamatteo@pm.me> */
/* Part of SMCLib. Licensed under GPLv3		*/
/***********************************************/

#ifndef _OPTS_H
#define _OPTS_H

/* Check if compiled with optimizations */
#if __OPTIMIZE__ == 0
	#warning "Please use optimizations for performance, like '-O3'!"
	#define OPTS "without optimizations"
/* Check if compiled with optimizations for size, not performance */
#elif __OPTIMIZE_SIZE__ == 1
	#warning "Please use optimizations for performance, like '-O3', instead of using '-Os'!"
	#define OPTS "with optimizations for size"
/* We know it's now compiled with optimizations for performance */
#else
	#define OPTS "with optimizations for performance"
#endif

#endif /* _OPTS_H */
