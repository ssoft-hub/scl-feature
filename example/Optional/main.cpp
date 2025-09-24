#include <iostream>
#include <string>

#include <ScL/Feature/Tool.h>
#include <ScL/Feature/Wrapper.h>

using namespace ::ScL::Feature;
using OptionalString = Wrapper<std::string, Inplace::Optional>;

OptionalString create(bool b)
{
    if (b)
        return "Godzilla";
    return {};
}

void example()
{
    ::std::cout << "create(true) returned " << create(true).value() << ::std::endl;
    ::std::cout << "create(false) returned " << create(false).valueOr("empty") << ::std::endl;
}

int main(int, char **)
{
    OptionalString str;
    str.emplace("Hello");

    OptionalString str1 = str;
    str.reset();
    str1 += " Optional!";

    OptionalString str2 = ::std::move(str1);
    ::std::cout << str2.value() << ::std::endl;

    example();

    return 0;
}
