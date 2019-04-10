#ifndef CL_TUILS_H
#define CL_UTILS_H

#define ARRAY_COUNT(a) (sizeof((a))/(sizeof((a[0]))))
#define INVALID_PATH assert(NULL)

extern inline void _BITCLEAR_U8(uint8_t *a,
			     int32_t n)
{
	assert(n < sizeof(*a) * 8);
	*a &= ~(1 << n);
}

extern inline void _BITCLEAR_U16(uint16_t *a,
			      int32_t n)
{
	assert(n < sizeof(*a) * 8);
	*a &= ~(1 << n);
}

extern inline void _BITCLEAR_U32(uint32_t *a,
			      int32_t n)
{
	assert(n < sizeof(*a) * 8);
	*a &= ~(1 << n);
}

extern inline void _BITCLEAR_U64(uint64_t *a,
			     int32_t n)
{
	assert(n < sizeof(*a) * 8);
	*a &= ~(1 << n);
}

extern inline void _BITCLEAR_S8(int8_t *a,
			     int32_t n)
{
	assert(n < sizeof(*a) * 8);
	*a &= ~(1 << n);
}

extern inline void _BITCLEAR_S16(int16_t *a,
			      int32_t n)
{
	assert(n < sizeof(*a) * 8);
	*a &= ~(1 << n);
}

extern inline void _BITCLEAR_S32(int32_t *a,
			      int32_t n)
{
	assert(n < sizeof(*a) * 8);
	*a &= ~(1 << n);
}

extern inline void _BITCLEAR_S64(int64_t *a,
			      int32_t n)
{
	assert(n < sizeof(*a) * 8);
	*a &= ~(1 << n);
}

#define BITCLEAR(a, n) _Generic((a), \
uint8_t : _BITCLEAR_U8, \
uint16_t : _BITCLEAR_U16, \
uint32_t : _BITCLEAR_U32, \
uint64_t : _BITCLEAR_U64, \
int8_t : _BITCLEAR_S8, \
int16_t : _BITCLEAR_S16, \
int32_t : _BITCLEAR_S32, \
int64_t : _BITCLEAR_S64)  ((&a), (n))

extern inline void _BITSET_U8(uint8_t *a,
			     int32_t n)
{
	assert(n < sizeof(*a) * 8);
	*a |= 1 << n;
}

extern inline void _BITSET_U16(uint16_t *a,
			      int32_t n)
{
	assert(n < sizeof(*a) * 8);
	*a |= 1 << n;
}

extern inline void _BITSET_U32(uint32_t *a,
			      int32_t n)
{
	assert(n < sizeof(*a) * 8);
	*a |= 1 << n;
}

extern inline void _BITSET_U64(uint64_t *a,
			     int32_t n)
{
	assert(n < sizeof(*a) * 8);
	*a |= 1 << n;
}

extern inline void _BITSET_S8(int8_t *a,
			     int32_t n)
{
	assert(n < sizeof(*a) * 8);
	*a |= 1 << n;
}

extern inline void _BITSET_S16(int16_t *a,
			      int32_t n)
{
	assert(n < sizeof(*a) * 8);
	*a |= 1 << n;
}

extern inline void _BITSET_S32(int32_t *a,
			      int32_t n)
{
	assert(n < sizeof(*a) * 8);
	*a |= 1 << n;
}

extern inline void _BITSET_S64(int64_t *a,
			      int32_t n)
{
	assert(n < sizeof(*a) * 8);
	*a |= 1 << n;
}

#define BITSET(a, n) _Generic((a), \
uint8_t : _BITSET_U8, \
uint16_t : _BITSET_U16, \
uint32_t : _BITSET_U32, \
uint64_t : _BITSET_U64, \
int8_t : _BITSET_S8, \
int16_t : _BITSET_S16, \
int32_t : _BITSET_S32, \
int64_t : _BITSET_S64, \
default : INVALID_PATH)  ((&a), (n))

extern inline bool _BITCHECK_U8(uint8_t a,
			     int32_t n)
{
	assert(n < sizeof(a) * 8);
	return(a & (1 << n));
}

extern inline bool _BITCHECK_U16(uint16_t a,
			      int32_t n)
{
	assert(n < sizeof(a) * 8);
	return(a & (1 << n));
}

extern inline bool _BITCHECK_U32(uint32_t a,
			      int32_t n)
{
	assert(n < sizeof(a) * 8);
	return(a & (1 << n));
}

extern inline bool _BITCHECK_U64(uint64_t a,
			     int32_t n)
{
	assert(n < sizeof(a) * 8);
	return(a & (1 << n));
}

extern inline bool _BITCHECK_S8(int8_t a,
			     int32_t n)
{
	assert(n < sizeof(a) * 8);
	return(a & (1 << n));
}

extern inline bool _BITCHECK_S16(int16_t a,
			      int32_t n)
{
	assert(n < sizeof(a) * 8);
	return(a & (1 << n));
}

extern inline bool _BITCHECK_S32(int32_t a,
			      int32_t n)
{
	assert(n < sizeof(a) * 8);
	return(a & (1 << n));
}

extern inline bool _BITCHECK_S64(int64_t a,
			      int32_t n)
{
	assert(n < sizeof(a) * 8);
	return(a & (1 << n));
}

#define BITCHECK(a, n) _Generic((a), \
uint8_t : _BITCHECK_U8, \
uint16_t : _BITCHECK_U16, \
uint32_t : _BITCHECK_U32, \
uint64_t : _BITCHECK_U64, \
int8_t : _BITCHECK_S8, \
int16_t : _BITCHECK_S16, \
int32_t : _BITCHECK_S32, \
int64_t : _BITCHECK_S64)  ((a), (n))

#endif