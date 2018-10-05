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

#include <stack>

#include <kdb.hpp>

// -- Class --------------------------------------------------------------------

namespace yaypeg {

/** The parser uses this class to convert YAML data to a key set. */
class State {
  /** This key set stores the converted YAML data. */
  kdb::KeySet keys;

  /** This stack stores a key for each level of the current key name. */
  std::stack<kdb::Key> parents;

public:
  /**
   * @brief This constructor creates a State using the given parent key.
   *
   * @param parent This argument specifies the parent key of the key set that
   *               this class stores.
   */
  State(kdb::Key const &parent);

  /**
   * @brief This method stores the current parent key in the key set.
   *
   * @param text This variable specifies the text that should be used as value
   *             for the stored parent key.
   */
  void appendKey(std::string const &text);

  /**
   * @brief This method returns the converted YAML data.
   *
   * @return A key set representing the parsed YAML data
   */
  kdb::KeySet getKeySet() const;
};

} // namespace yaypeg

#endif // ELEKTRA_PLUGIN_YAYPEG_STATE_HPP
