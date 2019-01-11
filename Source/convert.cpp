/**
 * @file
 *
 * @brief This file contains a function to convert a YAML file to a key set.
 *
 * @copyright BSD License (see LICENSE.md or https://www.libelektra.org)
 */

// -- Imports ------------------------------------------------------------------

#include "convert.hpp"
#include "parser.hpp"
#include "state.hpp"

#define TAO_PEGTL_NAMESPACE yaypeg

#include <tao/pegtl/contrib/parse_tree.hpp>
#include <tao/pegtl/contrib/tracer.hpp>

namespace yaypeg {

using kdb::Key;
using kdb::KeySet;
using std::string;

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
int addToKeySet(KeySet &keySet, Key &, string const &filename) {
  using std::cerr;
  using std::endl;
  using tao::TAO_PEGTL_NAMESPACE::analyze;
  using tao::TAO_PEGTL_NAMESPACE::file_input;
  using tao::TAO_PEGTL_NAMESPACE::parse;
  using tao::TAO_PEGTL_NAMESPACE::parse_error;
  using tao::TAO_PEGTL_NAMESPACE::tracer;

  State state;

  // Check grammar for problematic code
  analyze<yaml>();

  file_input<> input{filename};

  try {
    auto root = parse<yaml, action, tracer>(input, state);
    if (root) {
      std::cout << "\\o/" << std::endl;
    }
  } catch (parse_error const &error) {
    cerr << error.what() << endl;
    return -1;
  }

  KeySet keys;

  int status = (keys.size() <= 0) ? 0 : 1;

  keySet.append(keys);

  return status;
}

} // namespace yaypeg
