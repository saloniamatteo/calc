/* See LICENSE file for copyright and license details.
 *
 * This Library checks if GCC, MSVC, or CLang is being used to
 * compile the program.
 *
 * Made by Salonia Matteo <saloniamatteo@pm.me>
 * Part of SMCLib.
 *
 */

#ifndef _COMPILER_H
#define _COMPILER_H

/* Check CLang */
#ifdef __clang__
#define CC "CLang"
	
/* Check GCC */
#elif defined __GNUC__
#define CC "GCC"
		
/* Check MSVC */
#elif defined _MSC_VER
#define CC "MSVC"
#endif

#endif /* _COMPILER_H */
