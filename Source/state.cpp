/**
 * @file
 *
 * @brief This file contains the implementation of a struct used by the
 *        parser to store contextual data.
 *
 * @copyright BSD License (see LICENSE.md or https://www.libelektra.org)
 */

// -- Imports ------------------------------------------------------------------

#include "state.hpp"

#include <numeric>

// -- Class --------------------------------------------------------------------

namespace yaypeg {

using kdb::Key;

using std::accumulate;
using std::move;
using std::next;
using std::string;
using std::to_string;

// ===========
// = Private =
// ===========

string State::contextToString() const noexcept {
  if (context.empty()) {
    return "EMPTY";
  }
  if (context.top() == Context::BLOCK_IN) {
    return "BLOCK_IN";
  } else if (context.top() == Context::BLOCK_OUT) {
    return "BLOCK_OUT";
  } else if (context.top() == Context::BLOCK_KEY) {
    return "BLOCK_KEY";
  } else if (context.top() == Context::FLOW_KEY) {
    return "FLOW_KEY";
  } else if (context.top() == Context::FLOW_IN) {
    return "FLOW_IN";
  } else if (context.top() == Context::FLOW_OUT) {
    return "FLOW_OUT";
  }
  return "UKNOWN";
}

// ==========
// = Public =
// ==========

string State::toString() const noexcept {
  string indents =
      indentation.size() <= 0
          ? ""
          : accumulate(next(indentation.begin()), indentation.end(),
                       to_string(indentation[0]), // start with first element
                       [](string text, long long indent) {
                         return move(text) + ", " + to_string(indent);
                       });

  return "{" + contextToString() + "}" + " [" + indents + "]";
}

} // namespace yaypeg
