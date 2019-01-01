/**
 * @file
 *
 * @brief This file contains the definition of a struct used by the
 *        parser to store contextual data.
 *
 * @copyright BSD License (see LICENSE.md or https://www.libelektra.org)
 */

#ifndef ELEKTRA_PLUGIN_YAYPEG_CONTEXT_HPP
#define ELEKTRA_PLUGIN_YAYPEG_CONTEXT_HPP

// -- Imports ------------------------------------------------------------------

#include <deque>
#include <string>

// -- Class --------------------------------------------------------------------

namespace yaypeg {

struct Context {
  std::deque<size_t> indentation;
  std::string toString() const noexcept;
};

} // namespace yaypeg

#endif // ELEKTRA_PLUGIN_YAYPEG_CONTEXT_HPP
