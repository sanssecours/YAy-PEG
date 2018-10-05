/**
 * @file
 *
 * @brief This file contains the implementation of a class used by the PEGTL
 *        parser to create a key set.
 *
 * @copyright BSD License (see LICENSE.md or https://www.libelektra.org)
 */

// -- Imports ------------------------------------------------------------------

#include <kdb.hpp>

#include "state.hpp"

using kdb::KeySet;

// -- Class --------------------------------------------------------------------

namespace yaypeg {

/**
 * @brief This method returns the converted YAML data.
 *
 * @return A key set representing the parsed YAML data
 */
KeySet State::getKeySet() { return keys; };

} // namespace yaypeg
