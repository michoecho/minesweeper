#include "helper_routines.h"
#include <ctype.h>

bool isNumber(char *string) {
	if (!string || !*string) return false;
	while (*string) {
		if (!isdigit(*string)) return false;
		++string;
	}
	return true;
}

