#include <iostream>
#include <string>

#include <ScL/Feature.h>

using namespace ::ScL::Feature;
using namespace ::std;

// Unworkable case
//using Filename = string;
//using Url      = string;

// Strong typedef case
using Filename = Wrapper< string, Inplace::Typedef< class Filename_tag > >;
using Url      = Wrapper< string, Inplace::Typedef< class Url_tag > >;

auto read ( Filename filename ) { /*read from disk*/ return filename; }
auto read ( Url url )           { /*read from internet*/ return url;  }

auto test ()
{
  auto filename = Filename{ "foobar.txt" };
  auto url = Url{ "http://foobar.com/" };

  cout << "From disk [" << filename << "]: " << read( filename ) << "\n";
  cout << "From web  [" << url      << "]: " << read( url ) << "\n";
}

int main ( int, char ** )
{
    test();
    return 0;
}
