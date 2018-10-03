// -- Imports ------------------------------------------------------------------

#include <iostream>
#include <string>

#include <tao/pegtl.hpp>

#include "grammar.hpp"

namespace pegtl = tao::pegtl;

using pegtl::argv_input;
using pegtl::parse;

using yaypeg::action;
using yaypeg::yaml;

// -- Main ---------------------------------------------------------------------

int main(int argc, char *argv[]) {
  if (argc > 1) {
    argv_input<> input{argv, 1};
    parse<yaml, action>(input);
  }
}
