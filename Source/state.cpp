/**
 * @file
 *
 * @brief This file contains the implementation of a class used by the PEGTL
 *        parser to create a key set.
 *
 * @copyright BSD License (see LICENSE.md or https://www.libelektra.org)
 */

// -- Imports ------------------------------------------------------------------

#include "state.hpp"

// -- Class --------------------------------------------------------------------

namespace yaypeg {

std::string State::toString() const noexcept {
  std::string result = "[ ";
  for (auto const &indent : indentation) {
    result += std::to_string(indent) + " ";
  }
  return result + " ]";
}

} // namespace yaypeg
