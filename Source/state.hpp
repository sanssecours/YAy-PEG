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
#include <stack>
#include <string>

#include <kdb.hpp>

// -- Class --------------------------------------------------------------------

namespace yaypeg {

struct State {
  enum class Context {
    BLOCK_IN,
    BLOCK_OUT,
    BLOCK_KEY,
    FLOW_KEY,
    FLOW_IN,
    FLOW_OUT
  };

  std::stack<Context> context{{Context::BLOCK_IN}};
  std::deque<size_t> indentation;

  char lastWasNsChar = false;

  std::string key;
  std::stack<kdb::Key> parents;
  kdb::KeySet keys;

  State(kdb::Key const &parent);
  std::string toString() const noexcept;
};

} // namespace yaypeg

#endif // ELEKTRA_PLUGIN_YAYPEG_CONTEXT_HPP
