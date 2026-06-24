#include <scl/feature/inplace/strong_typedef.h>
#include <scl/feature/wrapper.h>

#include <iostream>

// ---------------------------------------------------------------------------
// Domain types
// ---------------------------------------------------------------------------

// Tags must be forward-declared outside the template argument list.
// C++ does not allow defining a new type inside a template argument.
struct UserIdTag
{};
struct OrderIdTag
{};

// Each tag value uniquely identifies the strong-typedef domain.
using UserId = scl::wrapper<int, scl::feature::inplace::strong_typedef<UserIdTag{}>::template type>;
using OrderId = scl::wrapper<int, scl::feature::inplace::strong_typedef<OrderIdTag{}>::template type>;

// Alternatively, use the convenience alias:
// using UserId  = scl::feature::inplace::make_strong_typedef<UserIdTag{}, int>;
// using OrderId = scl::feature::inplace::make_strong_typedef<OrderIdTag{}, int>;

// ---------------------------------------------------------------------------
// Functions that take domain-specific IDs — cannot be accidentally mixed
// ---------------------------------------------------------------------------

static void process_user(UserId uid)
{
    std::cout
        << "Processing user  "
        << scl::feature::inplace::strong_typedef<UserIdTag{}>::type<int>::value(
               scl::feature::detail::executor_access::get(uid))
        << '\n';
}

static void process_order(OrderId oid)
{
    std::cout
        << "Processing order "
        << scl::feature::inplace::strong_typedef<OrderIdTag{}>::type<int>::value(
               scl::feature::detail::executor_access::get(oid))
        << '\n';
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main()
{
    UserId uid{42};
    OrderId oid{7};

    process_user(uid);
    process_order(oid);

    // The following line does NOT compile — UserId and OrderId are unrelated types:
    //   process_user(oid);   // error: no matching function
    //   process_order(uid);  // error: no matching function

    // Compile-time proof of distinct types:
    static_assert(!std::is_same_v<UserId, OrderId>, "UserId and OrderId must be distinct types");

    std::cout << "Type safety verified at compile time.\n";
    return 0;
}
