#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static int dtostr(int i, char* s);
static int utostr(unsigned int u, char* s, bool lz, int md);
static int htostr(unsigned int u, char* s, bool lz, int md);

void* memcpy(void* dest, const void* src, size_t n)
{
	for (size_t i = 0; i < n; i++)
	{
		((unsigned char*)dest)[i] = ((unsigned char*)src)[i];
	}

	return dest;
}

void* memset(void* s, int c, size_t n)
{
	for (size_t i = 0; i < n; i++)
	{
		((unsigned char*)s)[i] = c;
	}

	return s;
}

int sprintf(char* str, const char* fmt, ...)
{
	const char* init_str = str;
	char tc;
	char* ts;
	int ti;
	unsigned int tu;

	va_list args;
	va_start(args, fmt);

	char c;
	while (c = *fmt++)
	{
		if (c == '%')
		{
			bool lz = false;
			int md = 0;
			char c2 = *fmt++;

			if (c2 == '0')
			{
				lz = true;
				c2 = *fmt++;
			}

			if (c2 >= '2' && c2 <= '9')
			{
				md = c2 - 0x30;
				c2 = *fmt++;
			}

			switch (c2)
			{
				case '%':
					*str++ = '%';
					break;
				case 'c':
					tc = va_arg(args, int);
					*str++ = tc;
					break;
				case 's':
					ts = va_arg(args, char*);
					strcpy(str, ts);
					str += strlen(ts);
					break;
				case 'd':
					ti = va_arg(args, int);
					str += dtostr(ti, str);
					break;
				case 'u':
					tu = va_arg(args, unsigned int);
					str += utostr(tu, str, lz, md);
					break;
				case 'x':
					tu = va_arg(args, unsigned int);
					str += htostr(tu, str, lz, md);
					break;
				default:
					break;
			}
		}
		else
		{
			*str++ = c;
		}
	}

	va_end(args);

	*str = 0;

	return (str - init_str);
}

char* strcpy(char* dest, const char* src)
{
	while (*src != 0)
	{
		*(dest++) = *(src++);
	}

	*dest = 0;

	return dest;
}

size_t strlen(const char* s)
{
	size_t len = 0;
	while (*(s++) != 0)
	{
		len++;
	}

	return len;
}

int strncmp(const char* s1, const char* s2, size_t n)
{
	size_t i = 0;
	while (i++ != n)
	{
		if (*s1 == 0)
		{
			if (*s2 == 0)
			{
				// Both strings end.
				return 0;
			}
			else
			{
				// s1 ends.
				return -1;
			}
		}
		else if (*s2 == 0)
		{
			// s2 ends.
			return 1;
		}
		else if (*s1 < *s2)
		{
			return -1;
		}
		else if (*s1 > *s2)
		{
			return 1;
		}

		s1++;
		s2++;
	}

	return 0;
}

static int dtostr(int i, char* s)
{
	if (i < 0)
	{
		*s++ = '-';
		return 1 + utostr(-i, s, false, 0);
	}
	else
	{
		return utostr(i, s, false, 0);
	}
}

static int utostr(unsigned int u, char* s, bool lz, int md)
{
	int i = 0;
	char cr[12];
	while (u != 0)
	{
		cr[i++] = u % 10;
		u /= 10;
	}

	int i2 = 0;
	if (i < md)
	{
		for (int j = 0; j < md - i; j++)
		{
			*s++ = (lz ? '0' : ' ');
			i2++;
		}
	}

	for (int j = i - 1; j >= 0; j--)
	{
		*s++ = cr[j] + 0x30;
	}

	if (i == 0 && i2 == 0)
	{
		*s = '0';
		i = 1;
	}

	return i + i2;
}

static int htostr(unsigned int u, char* s, bool lz, int md)
{
	int i = 0;
	char cr[12];
	while (u != 0)
	{
		cr[i++] = (u & 0x0f);
		u >>= 4;
	}

	int i2 = 0;
	if (i < md)
	{
		for (int j = 0; j < md - i; j++)
		{
			*s++ = (lz ? '0' : ' ');
			i2++;
		}
	}

	for (int j = i - 1; j >= 0; j--)
	{
		if (cr[j] >= 10)
		{
			*s++ = cr[j] + 0x57;
		}
		else
		{
			*s++ = cr[j] + 0x30;
		}
	}

	if (i == 0 && i2 == 0)
	{
		*s = '0';
		i = 1;
	}

	return i + i2;
}
