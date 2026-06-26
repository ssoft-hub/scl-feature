// Example: scl::feature::implicit::indirect — a copy-on-write executor.
//
// Copies of a wrapper share one heap allocation until the first mutating call,
// which clones the value for that copy only.  Read access never clones.  The
// value and its reference counter live in a single intrusive allocation, so the
// executor is one pointer wide.

#include <scl/feature/executor/implicit/indirect.h>
#include <scl/feature/wrapper.h>

#include <iostream>
#include <string>

namespace
{
    // A small reflected value type: `append` mutates, `str` reads.
    struct Text
    {
        std::string data;

        void append(char const * suffix) & { data += suffix; }
        [[nodiscard]]
        std::string const & str() const &
        {
            return data;
        }
    };
} // namespace

// Teach the wrapper which methods of Text to expose.
template <typename Wrapper, typename Executor>
class scl::feature::reflect<Wrapper, Executor, Text>
{
public:
    SCL_REFLECT_TYPE(Wrapper, Executor)

    SCL_REFLECT_METHOD(append)
    SCL_REFLECT_METHOD(str)
};

int main()
{
    using Document = scl::wrapper<Text, scl::feature::implicit::indirect>;

    Document original{Text{"hello"}};
    Document copy = original; // cheap: shares the same allocation, no string copy

    // Mutating the copy clones its value; the original is left untouched.
    copy.append(", world");

    std::cout << "original: " << original.str() << '\n'; // hello
    std::cout << "copy:     " << copy.str() << '\n';     // hello, world

    return original.str() == "hello" && copy.str() == "hello, world" ? 0 : 1;
}
