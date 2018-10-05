// -- Imports ------------------------------------------------------------------

#include <iostream>
#include <string>

#include <tao/pegtl.hpp>

#include <kdb.hpp>

#include "convert.hpp"

using std::cerr;
using std::cout;
using std::endl;
using std::string;

using tao::pegtl::input_error;
using tao::pegtl::parse_error;

using ckdb::keyNew;
using kdb::Key;
using kdb::KeySet;

using yaypeg::addToKeySet;

// -- Functions ----------------------------------------------------------------

void printOutput(KeySet const &keys) {
  cout << endl << "— Output ————" << endl << endl;
  for (auto key : keys) {
    cout << key.getName() << ":"
         << (key.getStringSize() > 1 ? " " + key.getString() : "") << endl;
  }
}

// -- Main ---------------------------------------------------------------------

int main(int argc, char *argv[]) {

  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " filename" << endl;
    return EXIT_FAILURE;
  }

  string filename = argv[1];
  KeySet keys;
  Key parent{keyNew("user", KEY_END, "", KEY_VALUE)};
  int status = -1;

  try {
    status = addToKeySet(keys, parent, filename);
  } catch (input_error const &error) {
    cerr << "Unable to open input: " << error.what() << endl;
  } catch (parse_error const &error) {
    cerr << "Unable to parse input: " << error.what() << endl;
  }

  printOutput(keys);
  return (status >= 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
