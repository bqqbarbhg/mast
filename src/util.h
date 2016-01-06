#include <ctype.h>

static void trimr(char *data)
{
	size_t len = strlen(data);
	int i;
	for (i = (int)len - 1; i >= 0; i--) {
		if (!isspace(data[i]))
			break;
	}
	data[i + 1] = '\0';
}

static const char *skip_prefix(const char *str, const char *prefix)
{
	while (*prefix && *str) {
		if (*str != *prefix)
			break;
		str++; prefix++;
	}
	return str;
}

