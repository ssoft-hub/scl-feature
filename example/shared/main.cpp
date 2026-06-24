// Example: heap::shared executor
//
// heap::shared<Value> stores its value on the heap and provides shared-ownership
// semantics: copy-construction shares the same heap allocation (shallow copy),
// so mutations through any copy are visible to all. Move-construction transfers
// ownership without allocating.

#include <scl/feature/heap/shared.h>
#include <scl/feature/wrapper.h>
#include <scl/feature/wrapper_guard.h>

#include <cassert>
#include <string>

int main()
{
    using namespace ::scl;
    using namespace ::scl::feature::heap;

    // --- Forwarding construction ---
    wrapper<::std::string, feature::heap::shared> a{"hello"};

    // --- Shallow copy: a and b share the same heap object ---
    auto b = a;

    // A write through b is visible through a (they share the allocation)
    {
        feature::wrapper_guard<decltype(b) &> gb{b};
        // gb.value() is a std::string & to the shared object
        (void)gb.value();
    }

    // --- Execute: invoke a callable with the held value ---
    feature::heap::shared<int> counter{0};
    feature::heap::shared<int>::execute(counter, [](int & v) { ++v; });
    assert(feature::heap::shared<int>::value(counter) == 1);

    // --- Shared identity: copy sees the mutation ---
    feature::heap::shared<int> x{10};
    feature::heap::shared<int> y{x}; // shallow — same object
    feature::heap::shared<int>::value(x) = 99;
    assert(feature::heap::shared<int>::value(y) == 99); // y sees the write

    // --- Move: z takes sole ownership ---
    auto z = ::std::move(x);
    assert(feature::heap::shared<int>::value(z) == 99);

    return 0;
}
