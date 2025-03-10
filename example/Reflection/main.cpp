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
    template <typename Self_, typename SelfHolder_>
    class ValueReflectionMixIn<Self_, SelfHolder_, MyData>
    {
    public:
        SCL_REFLECT_METHOD( getName )
        SCL_REFLECT_METHOD( setName )
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
