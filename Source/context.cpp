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

#include <numeric>

// -- Class --------------------------------------------------------------------

namespace yaypeg {

using std::accumulate;
using std::move;
using std::next;
using std::string;
using std::to_string;

string Context::toString() const noexcept {
  string result =
      indentation.size() <= 0
          ? ""
          : accumulate(next(indentation.begin()), indentation.end(),
                       to_string(indentation[0]), // start with first element
                       [](string text, size_t indent) {
                         return move(text) + ", " + to_string(indent);
                       });
  return "[" + result + "]";
}

} // namespace yaypeg
