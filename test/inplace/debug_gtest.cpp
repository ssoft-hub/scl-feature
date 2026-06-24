#include <gtest/gtest.h>

#include <scl/feature/detail/executor_access.h>
#include <scl/feature/inplace/debug.h>
#include <scl/feature/wrapper.h>

#include <sstream>
#include <string>

using namespace ::scl;

namespace fd = ::scl::feature::detail;

namespace
{
    /// RAII helper: redirects std::cout to an ostringstream for the lifetime of
    /// the object, then restores the original buffer on destruction.
    class CoutCapture
    {
    public:
        CoutCapture()
            : m_old{::std::cout.rdbuf(m_oss.rdbuf())}
        {}

        ~CoutCapture() { ::std::cout.rdbuf(m_old); }

        [[nodiscard]]
        ::std::string str() const
        {
            return m_oss.str();
        }

    private:
        ::std::ostringstream m_oss;
        ::std::streambuf * const m_old;
    };

    using W = fd::wrapper<int, feature::inplace::debug>;
    using E = W::executor_type;
} // namespace

TEST(DebugExecutor, ConstructorLogs)
{
    CoutCapture cap;
    W w{42};
    EXPECT_NE(cap.str().find("[scl::debug] ctor"), ::std::string::npos);
}

TEST(DebugExecutor, ValueLogs)
{
    W w{7};

    CoutCapture cap;
    [[maybe_unused]]
    int const & v = E::value(fd::executor_access::get(w));
    EXPECT_NE(cap.str().find("[scl::debug] value"), ::std::string::npos);
}

TEST(DebugExecutor, ExecuteLogs)
{
    W w{5};

    CoutCapture cap;
    E::execute(fd::executor_access::get(w), []() {});
    EXPECT_NE(cap.str().find("[scl::debug] execute"), ::std::string::npos);
}

TEST(DebugExecutor, GuardLogs)
{
    W w{0};

    CoutCapture cap;
    E::guard(fd::executor_access::get(w));
    EXPECT_NE(cap.str().find("[scl::debug] guard"), ::std::string::npos);
}

TEST(DebugExecutor, UnguardLogs)
{
    W w{0};

    CoutCapture cap;
    E::unguard(fd::executor_access::get(w));
    EXPECT_NE(cap.str().find("[scl::debug] unguard"), ::std::string::npos);
}

TEST(DebugExecutor, CorrectValueReturned)
{
    W w{99};

    CoutCapture cap;
    int const & v = E::value(fd::executor_access::get(w));
    EXPECT_EQ(v, 99);
}

TEST(DebugExecutor, ExecuteReturnsResult)
{
    W w{0};

    CoutCapture cap;
    int const result = E::execute(fd::executor_access::get(w), []() { return 42; });
    EXPECT_EQ(result, 42);
}

TEST(DebugExecutor, CopyConstructorLogs)
{
    W w1{10};

    CoutCapture cap;
    feature::inplace::debug<int> copy{fd::executor_access::get(w1)};
    EXPECT_NE(cap.str().find("[scl::debug] ctor"), ::std::string::npos);
}
