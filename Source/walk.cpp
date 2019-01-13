/**
 * @file
 *
 * @brief This file contains a tree walker function.
 *
 * @copyright BSD License (see LICENSE.md or https://www.libelektra.org)
 */

// -- Imports ------------------------------------------------------------------

#include <iostream>

#include "listener.hpp"
#include "walk.hpp"

using std::cout;
using std::endl;
using std::string;

using tao::TAO_PEGTL_NAMESPACE::parse_tree::node;

// -- Functions ----------------------------------------------------------------

namespace {

/**
 * @brief This function returns the string representation of a tree node.
 *
 * @param node This argument stores the tree node that this function converts to
 *             a string.
 * @param indent This variable stores the string representation of the current
 *               depth of the node.
 *
 * @return A string representation of the given node
 */
string toString(node const &node, string const indent = "") {
  string representation;

  representation += node.is_root() ? "Root" : indent + node.name();
  if (!node.is_root() && node.has_content()) {
    representation += ": “" + node.content() + "”";
  }

  if (!node.children.empty()) {
    for (auto &child : node.children) {
      representation += "\n" + toString(*child, indent + "  ");
    }
  }
  return representation;
}

} // namespace

/**
 * @brief This function walks a syntax tree calling methods of the given
 *        listener.
 *
 * @param listener This argument specifies the listener which this function
 *                 uses to convert the syntax tree to a key set.
 * @param root This variable stores the root of the tree this function
 *             visits.
 */
void walk(Listener &listener, node const &node) {
  if (node.is_root()) {
    cout << toString(node) << endl;
  }

  if (!node.children.empty()) {
    for (auto &child : node.children) {
      walk(listener, *child);
    }
  }
}
