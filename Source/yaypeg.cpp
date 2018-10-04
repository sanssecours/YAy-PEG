// -- Imports ------------------------------------------------------------------

#include <iostream>
#include <string>

#include <tao/pegtl.hpp>

#include "grammar.hpp"

using std::cerr;
using std::endl;

namespace pegtl = tao::pegtl;

using pegtl::file_input;
using pegtl::parse;
using tao::pegtl::input_error;
using tao::pegtl::parse_error;

using yaypeg::action;
using yaypeg::yaml;

// -- Main ---------------------------------------------------------------------

int main(int argc, char *argv[]) {

  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " filename" << endl;
    return EXIT_FAILURE;
  }

  try {
    file_input<> input{argv[1]};
    parse<yaml, action>(input);
  } catch (input_error const &error) {
    cerr << "Unable to open input: " << error.what() << endl;
  } catch (parse_error const &error) {
    cerr << "Unable to parse input: " << error.what() << endl;
  }
}
