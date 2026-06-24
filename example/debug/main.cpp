#include <scl/feature/inplace/debug.h>
#include <scl/feature/wrapper.h>

#include <iostream>
#include <string>

// Demonstrates scl::feature::inplace::debug — a diagnostic executor that
// logs every operation (ctor, value, execute, guard, unguard) to std::cout.
// All output below is interleaved with the log lines produced by the executor.

int main()
{
    ::std::cout << "--- construct ---\n";
    ::scl::wrapper<int, ::scl::feature::inplace::debug> w{42};

    ::std::cout << "\n--- construct from string ---\n";
    ::scl::wrapper<::std::string, ::scl::feature::inplace::debug> ws{"hello"};

    ::std::cout << "\n--- copy construct ---\n";
    ::scl::wrapper<int, ::scl::feature::inplace::debug> w2{w};

    return 0;
}
