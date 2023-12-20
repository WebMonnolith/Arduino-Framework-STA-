#ifndef _STA_FIXED_
#define _STA_FIXED_

#include <Arduino.h>
#include "sta.h"

#pragma GCC visibility push(default)
#pragma GCC diagnostic ignored "-Wshift-count-overflow"

BEGIN_NP_BLOCK

template <size_t dp>
constexpr int32 cast2Fixed(const double d)
noexcept {
    return int32(d * double(1 << dp) + (d >= 0 ? 0.5 : -0.5));
}

template <size_t dp>
constexpr double cast2Double(const int32 d)
noexcept {
    return double(d) / double(1 << dp);
}

/// <summary>
/// Simple fixed point calculation class.
/// </summary>
/// <typeparam name="T">TYPE</typeparam>
/// <typeparam name="T2">AMOUNT OF WORKSPACE IN BITS</typeparam>
/// <typeparam name="dp">AMOUNT OF PRECISION IN BITS</typeparam>
template <typename T, typename T2, size_t dp>
class Fixed {
public:
    Fixed() = default;
    Fixed(const double d) {
        this->value = T(cast2Fixed<dp>(d));
    }

    operator double() const {
        return cast2Double<dp>(this->value);
    }

    void set(T value) { this->value = value; }
    T get() const { return this->value; }
private:
    T value = T(0);
private:
    static Fixed form(T v) { Fixed f; f.set(v); return f; }
public:
    Fixed& operator = (const Fixed& f) = default;

    Fixed operator - () const {
        return form(-this->value);
    }

    Fixed operator + (const Fixed& f) const {
        return form(this->value + f.get());
    }

    Fixed& operator += (const Fixed& f) {
        this->value += f.get();
        return *this;
    }

    Fixed operator - (const Fixed& f) const {
        return form(this->value - f.get());
    }

    Fixed& operator -= (const Fixed& f) {
        this->value -= f.get();
        return *this;
    }

    Fixed operator * (const Fixed& f) const {
        return form((T2(this->value) * T2(f.get())) >> dp);
    }

    Fixed& operator *= (const Fixed& f) {
        this->value = (T2(this->value) * T2(f.get())) >> dp;
        return *this;
    }

    Fixed operator / (const Fixed& f) const {
        return form(((T2(this->value) << dp) / T2(f.get())));
    }

    Fixed& operator /= (const Fixed& f) {
        this->value = ((T2(this->value) << dp) / T2(f.get()));
        return *this;
    }

};

template <typename T, typename T2, size_t dp>
using fixed = Fixed<T, T2, dp>;

using fp16_16 = fixed<int32, int64, 16>;
using fp8_8 = fixed<int16, int32, 8>;

namespace literals {
    fp16_16 operator "" _fp16(const long double n) {
        return fp16_16(double(n));
    }

    fp8_8 operator "" _fp8(const long double n) {
        return fp8_8(double(n));
    }
}

END_NP_BLOCK

#endif