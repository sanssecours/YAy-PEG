// -- Imports ------------------------------------------------------------------

#include <iostream>
#include <string>

#include <tao/pegtl.hpp>

#include "grammar.hpp"

using std::cerr;
using std::endl;

namespace pegtl = tao::pegtl;

using pegtl::argv_input;
using pegtl::parse;
using tao::pegtl::parse_error;

using yaypeg::action;
using yaypeg::yaml;

// -- Main ---------------------------------------------------------------------

int main(int argc, char *argv[]) {
  if (argc > 1) {
    argv_input<> input{argv, 1};

    try {
      parse<yaml, action>(input);
    } catch (parse_error const &error) {
      cerr << "Unable to parser input: " << error.what() << endl;
    }
  }
}
