#include <scl/feature/thread_safe/mutex.h>
#include <scl/feature/wrapper.h>
#include <scl/feature/wrapper_guard.h>

#include <iostream>
#include <thread>

int main()
{
    // A counter guarded by a std::mutex executor.
    scl::wrapper<int, scl::feature::thread_safe::mutex> counter{0};

    auto increment = [&counter](int n) {
        for (int i = 0; i < n; ++i)
        {
            scl::feature::wrapper_guard<decltype(counter) &> g{counter};
            ++g.value();
        }
    };

    ::std::thread t1{increment, 1000};
    ::std::thread t2{increment, 1000};
    t1.join();
    t2.join();

    scl::feature::wrapper_guard<decltype(counter) &> g{counter};
    ::std::cout << "counter = " << g.value() << '\n'; // expected: 2000
}
