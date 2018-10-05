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

using kdb::Key;
using kdb::KeySet;

// -- Class --------------------------------------------------------------------

namespace yaypeg {

/**
 * @brief This constructor creates a State using the given parent key.
 *
 * @param parent This argument specifies the parent key of the key set that
 *               this class stores.
 */
State::State(Key const &parent) { parents.push(parent.dup()); }

/**
 * @brief This method returns the converted YAML data.
 *
 * @return A key set representing the parsed YAML data
 */
KeySet State::getKeySet() const { return keys; };

} // namespace yaypeg
