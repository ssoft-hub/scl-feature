#include <chrono>
#include <iostream>
#include <vector>
#include <ScL/Feature.h>

struct Timer
{
    using Clock = ::std::chrono::steady_clock;
    using TimePoint =  ::std::chrono::time_point<Clock>;
    using TimeDuration = TimePoint::duration;

    TimePoint m_start{};
    TimePoint m_stop{};

    Timer () noexcept { start(); }

    ~Timer () { stop(); }

    void start () noexcept { m_start = Clock::now(); };
    void stop () noexcept { m_stop = Clock::now(); };
    TimeDuration elapsed () noexcept { return m_stop - m_start; }
};

template <typename T_>
Timer foo ()
{
    using Container = T_;
    using Count = ::std::size_t;
    using Index = Count;

    Count constexpr value_count = 100000000;

    Container container;
    container.reserve(value_count);

    Timer timer;

    timer.start();

    for (Index i = 0; i < value_count; ++i)
        container.emplace_back(int(i));

    for (Index i = 0; i < value_count; ++i)
        container[i] += container[value_count - i - 1];

    timer.stop();

    return timer;
}

int main (int, char**)
{
    using namespace ::std;
    using namespace ::ScL::Feature;

    Timer::TimeDuration duration_0{};
    Timer::TimeDuration duration_1{};
    Timer::TimeDuration duration_2{};
    Timer::TimeDuration duration_3{};

    for ( int i = 0; i < 10; ++i )
    {
        duration_0 += foo<vector<int>>().elapsed();
        duration_1 += foo<vector<Wrapper<int>>>().elapsed();
        duration_2 += foo<Wrapper<vector<int>>>().elapsed();
        duration_3 += foo<Wrapper<vector<Wrapper<int>>>>().elapsed();
    }

    ::std::cout << "foo<vector<int>>: " << duration_0.count() << ::std::endl;
    ::std::cout << "foo<vector<Wrapper<int>>>: " << duration_1.count() << ::std::endl;
    ::std::cout << "foo<Wrapper<vector<int>>>: " << duration_2.count() << ::std::endl;
    ::std::cout << "foo<Wrapper<vector<Wrapper<int>>>>: " << duration_3.count() << ::std::endl;
}
