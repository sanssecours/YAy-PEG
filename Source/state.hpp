/**
 * @file
 *
 * @brief This file contains the declaration of a class used by the PEGTL
 *        parser to create a key set.
 *
 * @copyright BSD License (see LICENSE.md or https://www.libelektra.org)
 */

#ifndef ELEKTRA_PLUGIN_YAYPEG_STATE_HPP
#define ELEKTRA_PLUGIN_YAYPEG_STATE_HPP

// -- Imports ------------------------------------------------------------------

#include <kdb.hpp>

// -- Class --------------------------------------------------------------------

namespace yaypeg {

/** The parser uses this class to convert YAML data to a key set. */
class State {
  /** This key set stores the converted YAML data. */
  kdb::KeySet keys;

public:
  /**
   * @brief This method returns the converted YAML data.
   *
   * @return A key set representing the parsed YAML data
   */
  kdb::KeySet getKeySet();
};

} // namespace yaypeg

#endif // ELEKTRA_PLUGIN_YAYPEG_STATE_HPP
