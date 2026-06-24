// Example: heap::indirect executor
//
// heap::indirect<Value> stores its value on the heap and provides full value
// semantics: copy-construction performs a deep copy (new allocation), while
// move-construction transfers pointer ownership without allocating.

#include <scl/feature/heap/indirect.h>
#include <scl/feature/wrapper.h>

#include <cassert>
#include <string>

int main()
{
    using namespace ::scl;
    using namespace ::scl::feature::heap;

    // --- Forwarding construction ---
    wrapper<::std::string, feature::heap::indirect> a{"hello"};

    // --- Deep copy: a and b are independent ---
    auto b = a;

    // --- Move: c takes ownership of b's allocation ---
    auto c = ::std::move(b);

    // --- Execute: invoke a callable with the held value ---
    feature::heap::indirect<int> counter{0};
    feature::heap::indirect<int>::execute(counter, [](int & v) { ++v; });
    assert(feature::heap::indirect<int>::value(counter) == 1);

    return 0;
}
