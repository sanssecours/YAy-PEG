/**
 * @file
 *
 * @brief This file contains a function to convert a YAML file to a key set.
 *
 * @copyright BSD License (see LICENSE.md or https://www.libelektra.org)
 */

// -- Imports ------------------------------------------------------------------

#include "convert.hpp"
#include "grammar.hpp"
#include "state.hpp"

#include <tao/pegtl/analyze.hpp>
#include <tao/pegtl/contrib/tracer.hpp>

using std::string;

using CppKey = kdb::Key;
using CppKeySet = kdb::KeySet;

namespace yaypeg {

#define TAO_PEGTL_NAMESPACE yaypeg

using tao::yaypeg::analyze;
using tao::yaypeg::file_input;
using tao::yaypeg::parse;

// -- Function -----------------------------------------------------------------

/**
 * @brief This function converts the given YAML file to keys and adds the
 *        result to `keySet`.
 *
 * @param keySet The function adds the converted keys to this variable.
 * @param parent The function uses this parent key of `keySet` to emit error
 *               information.
 * @param filename This parameter stores the path of the YAML file this
 *                 function converts.
 *
 * @retval -1 if there was a error converting the YAML file
 * @retval  0 if parsing was successful and the function did not change the
 *            given keyset
 * @retval  1 if parsing was successful and the function did change `keySet`
 */
int addToKeySet(CppKeySet &keySet, CppKey &parent, string const &filename) {
  using std::cerr;
  using std::endl;
  using tao::yaypeg::parse_error;
  using tao::yaypeg::tracer;

  State state{parent};

  // Check grammar for problematic code
  analyze<yaml>();

  file_input<> input{filename};

  try {
    parse<yaml, action, tracer>(input, state);
  } catch (parse_error const &error) {
    cerr << error.what() << endl;
    return -1;
  }

  CppKeySet keys = state.getKeySet();

  int status = (keys.size() <= 0) ? 0 : 1;

  keySet.append(keys);

  return status;
}

} // namespace yaypeg
