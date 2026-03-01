#include <iostream>
#include <string>

#include <scl/feature.h>

using namespace ::scl::feature;
using namespace ::std;

// Unworkable case
// using Filename = string;
// using Url      = string;

// Strong typedef case
using Filename = wrapper<string, Inplace::Typedef<class Filename_tag> >;
using Url = wrapper<string, Inplace::Typedef<class Url_tag> >;

auto read(Filename filename) { /*read from disk*/ return filename; }

auto read(Url url) { /*read from internet*/ return url; }

auto test()
{
    auto filename = Filename{"foobar.txt"};
    auto url = Url{"http://foobar.com/"};

    cout << "From disk [" << filename << "]: " << read(filename) << "\n"; //::std::endl;
    cout << "From web  [" << url << "]: " << read(url) << "\n";           //::std::endl;
}

int main(int, char **)
{
    test();
    return 0;
}
