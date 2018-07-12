#ifndef HELPER_ROUTINES_H_
#define HELPER_ROUTINES_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

bool isNumber(char *string);

#define ALLOC(ptr)\
do {\
	ptr = malloc(sizeof(*ptr));\
	if (ptr == NULL) {\
		fprintf(stderr, "[%s:%ul] Allocation failed (%zuB)\n",\
			__FILE__, __LINE__, sizeof(*ptr));\
		abort();\
	}\
} while (0)

#define ALLOC_N(ptr, N)\
do {\
	ptr = malloc(sizeof(*ptr) * N);\
	if (ptr == NULL && N != 0) {\
		fprintf(stderr, "[%s:%ul] Allocation failed (%zuB)\n",\
			__FILE__, __LINE__, sizeof(*ptr));\
		abort();\
	}\
} while (0)

#define CALLOC_N(ptr, N)\
do {\
	ptr = calloc(sizeof(*ptr), N);\
	if (ptr == NULL && N != 0) {\
		fprintf(stderr, "[%s:%ul] Allocation failed (%zuB)\n",\
			__FILE__, __LINE__, sizeof(*ptr));\
		abort();\
	}\
} while (0)

#define REALLOC_N(ptr, N)\
do {\
	ptr = realloc(ptr, sizeof(*ptr) * N);\
	if (ptr == NULL & N != 0) {\
		fprintf(stderr, "[%s:%ul] Allocation failed (%zuB)\n",\
			__FILE__, __LINE__, sizeof(*ptr));\
		abort();\
	}\
} while (0)

#define FREE(ptr)\
do {\
	free(ptr);\
	ptr = NULL;\
} while (0)

#define EXPAND_COLOR(color) color.r, color.g, color.b

#endif
