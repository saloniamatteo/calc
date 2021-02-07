/***************************************************/
/* This file checks if GCC, MSVC or CLang is used */
/* Made by Salonia Matteo <saloniamatteo@pm.me>  */
/* Part of SMCLib. Licensed under GPLv3		*/
/***********************************************/

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

#endif
