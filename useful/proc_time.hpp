#pragma once
#include "std_import.hpp"

#ifdef __linux

#include <unistd.h>
#include <sys/times.h>

namespace uf
{
    class proc_time_meter
    {
        static const long per_second_;

        i64 now() const
        {
            tms result;
            times(&result);
            return result.tms_stime + result.tms_utime;
        }

        i64 begin_ = now();
        i64 stop_;
        bool stopped_ = false;

    public:
        long double get() const noexcept
        {
            if (stopped_)
                return static_cast<long double>(stop_ - begin_) / per_second_;
            return static_cast<long double>(now() - begin_) / per_second_;
        }

        long double restart() noexcept
        {
            long double result = get();
            stopped_ = false;
            begin_ = now();
            return result;
        }

        long double stop() noexcept
        {
            long double result = get();
            if (stopped_)
                return result;
            stop_ = now();
            stopped_ = true;
            return result;
        }

        void start() noexcept
        {
            if (!stopped_)
                return;
            stopped_ = false;
            begin_ += now() - stop_;
        }

        bool stopped() const noexcept
        {
            return stopped_;
        }
    };

    const long proc_time_meter::per_second_ = sysconf(_SC_CLK_TCK);
}

#elif _WIN32

// TODO

#else

#error "Unknown OS"

#endif




