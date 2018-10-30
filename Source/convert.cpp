/**
 * @file
 *
 * @brief This file contains a function to convert a YAML file to a key set.
 *
 * @copyright BSD License (see LICENSE.md or https://www.libelektra.org)
 */

// -- Macros -------------------------------------------------------------------

#define TAO_PEGTL_NAMESPACE yaypeg

// -- Imports ------------------------------------------------------------------

#include "convert.hpp"
#include "grammar.hpp"
#include "state.hpp"

#include <tao/pegtl/analyze.hpp>

using std::string;

using tao::yaypeg::analyze;
using tao::yaypeg::file_input;
using tao::yaypeg::parse;

using CppKey = kdb::Key;
using CppKeySet = kdb::KeySet;

namespace yaypeg {

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
 * @retval  0 if parsing was successful and the function did not change the
 *            given keyset
 * @retval  1 if parsing was successful and the function did change `keySet`
 */
int addToKeySet(CppKeySet &keySet, CppKey &parent, string const &filename) {
  State state{parent};

  // Check grammar for problematic code
  analyze< yaml >();

  file_input<> input{filename};
  parse<yaml, action>(input, state);

  CppKeySet keys = state.getKeySet();

  int status = (keys.size() <= 0) ? 0 : 1;

  keySet.append(keys);

  return status;
}

} // namespace yaypeg
