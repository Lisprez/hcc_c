/* \brief
		utility functions
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>
#include "mm.h"


typedef struct tagLocation {
	const char* _filename;
	unsigned int _line;
	unsigned int _col;
} FLocation;

typedef union tagValue {
	char		_ch;
	int16_t		_wch;
	int32_t		_int;
	uint32_t	_uint;
	int32_t		_long;
	uint32_t	_ulong;
	int64_t		_llong;
	uint64_t	_ullong;
	float		_float;
	double		_double;
	long double	_ldouble;
	const char* _astr;
	union
	{
		const char* _str;
		uint32_t	_chcnt;
	} _wstr;
} FValue;


unsigned int util_str_hash(const char* str, unsigned int len);
char util_char_lower(char c);
char util_char_upper(char c);

const char* util_convert_abs_pathname(const char* pathname);
int util_is_relative_pathname(const char* pathname);

/* convert xxx\bbb\cc\ to xxx/bbb/cc/ */
const char* util_normalize_pathname(const char* pathname);
const char* util_process_bin_dir();
const char* util_process_working_dir();
const char* util_getpath_from_pathname(const char* pathname);
const char* util_make_pathname(const char* path, const char* filename);

#endif /* __UTILS_H__ */
