#pragma once
#include <ctime>

#include <chrono>
#include <functional>

#include "base.hpp"


namespace wh
{
    inline namespace benchmark
    {
        template<typename TimePoint>
        class time_meter
        {
        public:
            using time_point = TimePoint;

        private:
            std::function<time_point()> get_now_;
            std::function<double(time_point, time_point)> get_sec_;

            time_point begin_;
            time_point stop_;
            bool stopped_;

        public:
            template<typename GetNow, typename GetSec>
            time_meter(GetNow&& get_now, GetSec&& get_sec) : get_now_(std::forward<GetNow>(get_now)), get_sec_(std::forward<GetSec>(get_sec)), begin_(get_now_()), stopped_(false) { }

            double seconds() const
            {
                if (stopped_)
                    return get_sec_(begin_, stop_);
                return get_sec_(begin_, get_now_());
            }

            void restart()
            {
                stopped_ = false;
                begin_ = get_now_();
            }

            void stop()
            {
                if (stopped_)
                    return;
                stop_ = get_now_();
                stopped_ = true;
            }

            void start()
            {
                if (!stopped_)
                    return;
                stopped_ = false;
                begin_ += get_now_() - stop_;
            }
        };

        auto create_tm()
        {
            using tm_type = time_meter<std::chrono::high_resolution_clock::time_point>;

            static constexpr auto get_sec = [](tm_type::time_point p1, tm_type::time_point p2)
            {
                return static_cast<double>((p2 - p1).count()) / std::chrono::high_resolution_clock::period::den;
            };

            return tm_type(std::chrono::high_resolution_clock::now, get_sec);
        }

        auto create_proc_tm()
        {
            using tm_type = time_meter<clock_t>;

            static const auto get_now = []()
            {
                return clock();
            };

            static const auto get_sec = [](tm_type::time_point p1, tm_type::time_point p2)
            {
                return static_cast<double>(p2 - p1) / CLOCKS_PER_SEC;
            };

            return tm_type(get_now, get_sec);
        }

        template<typename F, typename... Args>
        double benchmark(F&& f, Args&&... args)
        {
            auto tm = create_tm();
            std::invoke(f, std::forward<Args>(args)...);
            return tm.seconds();
        }

        template<typename F, typename... Args>
        double proc_benchmark(F&& f, Args&&... args)
        {
            auto tm = create_proc_tm();
            std::invoke(f, std::forward<Args>(args)...);
            return tm.seconds();
        }
    }
    // inline namespace benchmark
}
// namespace wh






