#pragma once

#include <nms/core/base.h>

namespace nms
{

/* wall time */
struct DateTime
{
    u16 year;           // year         [0~65535]
    u16 month;          // month        [0~11]
    u16 day;            // day of month [1~31]

    u16 hour    = 0;    // hour         [0~23]
    u16 minute  = 0;    // minute       [0~59]
    u16 second  = 0;    // second       [0~59]

    constexpr DateTime()
        : year(1990), month(1), day(1)
    {}

    constexpr DateTime(u32 year, u32 month, u32 day)
        : year(u16(year)), month(u16(month)), day(u16(day))
    {}

    constexpr DateTime(u32 year, u32 month, u32 day, u32 hour, u32 minute, u32 second)
        : year(u16(year)), month(u16(month)), day(u16(day))
        , hour(u16(hour)), minute(u16(minute)), second(u16(second))
    {}

#ifdef _M_CEE
    __forceinline DateTime(System::DateTime v)
        : year(v.Year), month(v.Month), day(v.Day)
        , hour(v.Hour), minute(v.Minute), second(v.Second)
    {}
#endif

    NMS_API i64 stamp() const;
    NMS_API static DateTime now();

    friend i64 operator-(const DateTime& a, const DateTime& b) {
        const auto ta = a.stamp();
        const auto tb = b.stamp();
        return ta - tb;
    }
};

/*!
 * process clock
 */
NMS_API f64 clock();

}
