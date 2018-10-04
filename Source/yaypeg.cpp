// -- Imports ------------------------------------------------------------------

#include <iostream>
#include <string>

#include <tao/pegtl.hpp>

#include <kdb.hpp>

#include "convert.hpp"

using std::cerr;
using std::endl;
using std::string;

using tao::pegtl::input_error;
using tao::pegtl::parse_error;

using ckdb::keyNew;
using kdb::Key;
using kdb::KeySet;

using yaypeg::addToKeySet;

// -- Main ---------------------------------------------------------------------

int main(int argc, char *argv[]) {

  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " filename" << endl;
    return EXIT_FAILURE;
  }

  string filename = argv[1];
  KeySet keys;
  Key parent{keyNew("user", KEY_END, "", KEY_VALUE)};

  try {
    addToKeySet(keys, parent, filename);
  } catch (input_error const &error) {
    cerr << "Unable to open input: " << error.what() << endl;
  } catch (parse_error const &error) {
    cerr << "Unable to parse input: " << error.what() << endl;
  }
}
