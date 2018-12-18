/**
 * @file
 *
 * @brief This file contains the implementation of a struct used by the
 *        parser to store contextual data.
 *
 * @copyright BSD License (see LICENSE.md or https://www.libelektra.org)
 */

// -- Imports ------------------------------------------------------------------

#include "context.hpp"

// -- Class --------------------------------------------------------------------

namespace yaypeg {

std::string Context::toString() const noexcept {
  std::string result = "[ ";
  for (auto const &indent : indentation) {
    result += std::to_string(indent) + " ";
  }
  return result + " ]";
}

} // namespace yaypeg
