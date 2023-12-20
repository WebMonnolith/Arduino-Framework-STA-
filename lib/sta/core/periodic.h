#ifndef _STA_PERIODIC_
#define _STA_PERIODIC_

#include "sta.h"

BEGIN_NP_BLOCK

template <typename TL, typename TR>
_STAXXEXPORT
constexpr auto circ_add(
    const TL lhs,
    const TR rhs,
    const decltype(lhs + rhs) rmin = 0,
    const decltype(lhs + rhs) rmax = 360
    ) _STAXX_NOEXCEPT -> decltype(lhs + rhs)
{
    auto result = lhs + rhs;
    auto range = rmax - rmin;
    while (result >= rmax) result -= range;
    while (result < rmin) result += range;
    return result;

}

template <typename TL, typename TR>
_STAXXEXPORT
constexpr auto circ_sub(
    const TL lhs,
    const TR rhs,
    const decltype(lhs + rhs) rmin = 0,
    const decltype(lhs + rhs) rmax = 360
    ) _STAXX_NOEXCEPT -> decltype(lhs + rhs)
{
    auto result = lhs - rhs;
    auto range = rmax - rmin;
    while (result >= rmax) result -= range;
    while (result < rmin) result += range;
    return result;

}

template <typename TL, typename TR>
_STAXXEXPORT
constexpr auto circ_mul(
    const TL lhs,
    const TR rhs,
    const decltype(lhs + rhs) rmin = 0,
    const decltype(lhs + rhs) rmax = 360
    ) _STAXX_NOEXCEPT -> decltype(lhs + rhs)
{
    auto result = lhs * rhs;
    auto range = rmax - rmin;
    while (result >= rmax) result -= range;
    while (result < rmin) result += range;
    return result;

}

template <typename TL, typename TR>
_STAXXEXPORT
constexpr auto circ_div(
    const TL lhs,
    const TR rhs,
    const decltype(lhs + rhs) rmin = 0,
    const decltype(lhs + rhs) rmax = 360
    ) _STAXX_NOEXCEPT -> decltype(lhs + rhs)
{
    auto result = lhs / rhs;
    auto range = rmax - rmin;
    while (result >= rmax) result -= range;
    while (result < rmin) result += range;
    return result;

}

template <typename TL, typename TR>
_STAXXEXPORT
constexpr auto circ_sdist(
    const TL lhs,
    const TR rhs,
    const decltype(lhs + rhs) rmin = 0,
    const decltype(lhs + rhs) rmax = 360
    ) _STAXX_NOEXCEPT -> decltype(lhs + rhs)
{
    auto fx = circ_sub(rhs, lhs, rmin, rmax);
    auto fy = circ_sub(lhs, rhs, rmin, rmax);
    return min(fx, fy);
}

template <typename TL, typename TR>
_STAXXEXPORT
constexpr auto circ_sdiff(
    const TL lhs,
    const TR rhs,
    const decltype(lhs + rhs) rmin = 0,
    const decltype(lhs + rhs) rmax = 360
    ) _STAXX_NOEXCEPT -> decltype(lhs + rhs)
{
    auto fx = circ_sub(rhs, lhs, rmin, rmax);
    auto fy = circ_sub(lhs, rhs, rmin, rmax);
    if (fy > fx)
        return fx;
    return -fy;
}

END_NP_BLOCK

#endif