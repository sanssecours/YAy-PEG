// -- Imports ------------------------------------------------------------------

#include <iostream>
#include <string>

#include <tao/pegtl.hpp>

using std::cout;

namespace pegtl = tao::pegtl;

using pegtl::argv_input;
using pegtl::eof;
using pegtl::identifier;
using pegtl::nothing;
using pegtl::parse;
using pegtl::until;

// -- Grammar ------------------------------------------------------------------

namespace yaypeg {

struct plain_scalar : identifier {};

struct yaml : until<plain_scalar, eof> {};

template <typename Rule> struct action : nothing<Rule> {};

template <> struct action<plain_scalar> {
  template <typename Input> static void apply(const Input &input) {
    cout << "Found plain scalar “" << input.string() << "”";
  }
};

} // namespace yaypeg

// -- Main ---------------------------------------------------------------------

int main(int argc, char *argv[]) {
  if (argc > 1) {
    argv_input<> input{argv, 1};
    parse<yaypeg::yaml, yaypeg::action>(input);
  }
}
