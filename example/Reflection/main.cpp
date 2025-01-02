#include <ScL/Feature.h>
#include <cstring>
#include <string_view>

class MyData
{
    using Text = ::std::string;
    using TextView = ::std::string_view;

private:
    Text m_name{};

public:
    TextView getName () const & { return m_name; }
    void setName (TextView name) { m_name = name; }
};

namespace ScL::Feature
{
    template <typename Self_>
    class ValueReflectionMixIn<Self_, MyData>
    {
    public:
        template <typename ... Arguments_>
        decltype(auto) getName (Arguments_ && ... /*arguments*/) const & { return "test"; }

        template <typename ... Arguments_>
        decltype(auto) setName (Arguments_ && ... /*arguments*/) {}
    };
}

template <typename Type_>
void foo ()
{
    Type_ data{};

    data.setName( "TEST" );
    data.getName();
}

int main (int, char**)
{
    using namespace ::ScL::Feature;

    foo<MyData>();
    foo<Wrapper<MyData>>();
}
