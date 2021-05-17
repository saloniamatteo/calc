/* See LICENSE file for copyright and license details.
 *
 * This Library checks if optimizations, such as -O1, -O2, -O3,
 * or -Ofast are being used, defining "OPTS" at pre-processing
 * time.
 *
 * Made by Salonia Matteo <saloniamatteo@pm.me>
 * Part of SMCLib.
 *
 */

#ifndef _OPTS_H
#define _OPTS_H

/* No optimizations are being used */
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
