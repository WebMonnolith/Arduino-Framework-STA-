#ifndef _STA_UTILITY_
#define _STA_UTILITY_

#pragma GCC visibility push(default)

#include "sta.h"

BEGIN_NP_BLOCK

/*
A struct that can be used to generate a delay in the program with some range constraints.
Can be used together with coroutines.
*/
struct _STAXXEXPORT safe_delay {
public:
	inline safe_delay(uint16 time_in_ms, uint16 limit=45) {
		if (time_in_ms > limit)
			return;
		delay(time_in_ms);
	}
};

/*
A class that can be used to generate a delay in the without using the delay() function.
Can be used symbiotically with your entire program.

Basic usage:
sta::safe_delay_point previous = 0UL;
sta::uint64 interval = 1000;
...
sta::safe_delay_point current;
if (current - previous > interval) {
	...
	previous = current;
}
*/
class _STAXXEXPORT safe_delay_point {
public: // CON-/DESTRUCTORS
	inline safe_delay_point(uint64 point=millis()) 
		: _tp(point)
	{}
public: // OPERATORS
	inline operator unsigned long long() const { return this->_tp; }
private: // INTERNAL
	uint64 _tp; 
};

namespace literals {
	safe_delay_point operator "" _sdp(const unsigned long long n) {
        return safe_delay_point(n);
    }
}

inline _STAXXEXPORT
safe_delay_point delta(
	const safe_delay_point& _X1,
	const safe_delay_point& _X2) _STAXX_NOEXCEPT 
{
	return _X1 - _X2;
}

/*
Sets the stating baud rate of each cycle.
*/
inline _STAXXEXPORT 
void begin(uint32 baudrate) 
_STAXX_USE_NOEXCEPT {
#if defined(UBRRH) || defined(UBRR0H)
	Serial.begin(baudrate);
#endif
#if defined(UBRR1H)
	Serial1.begin(baudrate);
#endif
#if defined(UBRR2H)
	Serial2.begin(baudrate);
#endif
#if defined(UBRR3H)
	Serial3.begin(baudrate);
#endif
}

inline _STAXXEXPORT 
void end() 
_STAXX_USE_NOEXCEPT {
#if defined(UBRRH) || defined(UBRR0H)
	Serial.end();
#endif
#if defined(UBRR1H)
	Serial1.end();
#endif
#if defined(UBRR2H)
	Serial2.end();
#endif
#if defined(UBRR3H)
	Serial3.end();
#endif
}

namespace rel_ops {
	template<class T> inline bool operator!=(const T& x, const T& y) {
		return !(x == y);
	}
	
	template<class T> inline bool operator> (const T& x, const T& y) {
		return (y < x);
	}

	template<class T> inline bool operator<=(const T& x, const T& y) {
		return !(y < x);
	}

	template<class T> inline bool operator>=(const T& x, const T& y) {
		return !(x < y);
	}
}

template <class T1, class T2> 
struct _STAXXEXPORT pair {
public:
	typedef T1 first_type;
	typedef T2 second_type;
public:
	T1 first;
	T2 second;
public:
	pair() = default;
	pair(const T1& x, const T2& y) : first(x), second(y) {  }
	template<class U, class V> pair(const pair<U, V>& p) : first(p.first), second(p.second) { }
};

template <class T1, class T2> 
bool operator==(const pair<T1, T2>& x, const pair<T1, T2>& y) {
	using namespace rel_ops;
	return (x.first == y.first && x.second == y.second);
}

template <class T1, class T2> 
bool operator< (const pair<T1, T2>& x, const pair<T1, T2>& y) {
	return x.first < y.first || (!(y.first < x.first) && x.second < y.second);
}

template <class T1, class T2> 
bool operator!=(const pair<T1, T2>& x, const pair<T1, T2>& y) {
	return !(x == y);
}

template <class T1, class T2> 
bool operator> (const pair<T1, T2>& x, const pair<T1, T2>& y) {
	return y < x;
}

template <class T1, class T2> 
bool operator>=(const pair<T1, T2>& x, const pair<T1, T2>& y) {
	return !(x < y);
}

template <class T1, class T2> 
bool operator<=(const pair<T1, T2>& x, const pair<T1, T2>& y) {
	return !(y < x);
}

template <class T1, class T2> pair<T1, T2> 
make_pair(const T1& x, const T2& y) {
	return pair<T1, T2>(x, y);
}

// Stubb out move for compatibility
template<class T>
inline T& move(T& t) _STAXX_NOEXCEPT {
	return t;
}

END_NP_BLOCK

#endif
