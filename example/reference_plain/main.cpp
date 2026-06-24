#include <scl/feature/executors.h>
#include <scl/feature/wrapper.h>
#include <scl/feature/wrapper_guard.h>

#include <cassert>
#include <iostream>
#include <string>

int main()
{
    // --- Basic observation ---
    ::std::string s = "hello";
    ::scl::wrapper<::std::string, ::scl::feature::reference::plain> ref{s};

    {
        ::scl::feature::wrapper_guard<decltype(ref) &> g{ref};
        g.value() += " world";
    }

    assert(s == "hello world");
    ::std::cout << "s = " << s << "\n"; // s = hello world

    // --- Shallow copy: both observe the same object ---
    int x = 10;
    ::scl::wrapper<int, ::scl::feature::reference::plain> ra{x};
    auto rb = ra; // copy — same pointer

    {
        ::scl::feature::wrapper_guard<decltype(ra) &> ga{ra};
        ga.value() = 99;
    }

    {
        ::scl::feature::wrapper_guard<decltype(rb) &> gb{rb};
        assert(gb.value() == 99);                             // rb sees the mutation through ra
        ::std::cout << "rb.value() = " << gb.value() << "\n"; // 99
    }

    assert(x == 99);
    ::std::cout << "x = " << x << "\n"; // x = 99

    // --- Compile-time: rvalue binding is deleted ---
    // The line below must NOT compile:
    // ::scl::wrapper<int, ::scl::feature::reference::plain> bad{42};

    return 0;
}
