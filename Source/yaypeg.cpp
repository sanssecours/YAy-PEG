// Copyright (c) 2014-2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/PEGTL/

#include <iostream>
#include <string>

#include <tao/pegtl.hpp>

namespace pegtl = tao::pegtl;

namespace hello {
// Parsing rule that matches a literal "Hello, ".

struct prefix : pegtl::string<'H', 'e', 'l', 'l', 'o', ',', ' '> {};

// Parsing rule that matches a non-empty sequence of
// alphabetic ascii-characters with greedy-matching.

struct name : pegtl::plus<pegtl::alpha> {};

// Parsing rule that matches a sequence of the 'prefix'
// rule, the 'name' rule, a literal "!", and 'eof'
// (end-of-file/input), and that throws an exception
// on failure.

struct grammar : pegtl::must<prefix, name, pegtl::one<'!'>, pegtl::eof> {};

// Class template for user-defined actions that does
// nothing by default.

template <typename Rule> struct action : pegtl::nothing<Rule> {};

// Specialisation of the user-defined action to do
// something when the 'name' rule succeeds; is called
// with the portion of the input that matched the rule.

template <> struct action<name> {
  template <typename Input> static void apply(const Input &in, std::string &v) {
    v = in.string();
  }
};

} // namespace hello

int main(int argc, char *argv[]) {
  if (argc > 1) {
    // Start a parsing run of argv[1] with the string
    // variable 'name' as additional argument to the
    // action; then print what the action put there.

    std::string name;

    pegtl::argv_input<> in(argv, 1);
    pegtl::parse<hello::grammar, hello::action>(in, name);

    std::cout << "Good bye, " << name << "!" << std::endl;
  }
}
