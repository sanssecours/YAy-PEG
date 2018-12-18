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

#include <deque>
#include <stack>
#include <string>

// -- Class --------------------------------------------------------------------

namespace yaypeg {

/** The parser uses this class to convert YAML data to a key set. */
struct State {
  std::deque<size_t> indentation{0};
  std::string toString() const noexcept;
};

} // namespace yaypeg

#endif // ELEKTRA_PLUGIN_YAYPEG_STATE_HPP
