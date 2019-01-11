/**
 * @file
 *
 * @brief This file contains a tree walker function.
 *
 * @copyright BSD License (see LICENSE.md or https://www.libelektra.org)
 */

// -- Imports ------------------------------------------------------------------

#include "walk.hpp"
#include "listener.hpp"

using tao::TAO_PEGTL_NAMESPACE::parse_tree::node;

// -- Functions ----------------------------------------------------------------

/**
 * @brief This function walks a syntax tree calling methods of the given
 *        listener.
 *
 * @param listener This argument specifies the listener which this function
 *                 uses to convert the syntax tree to a key set.
 * @param root This variable stores the root of the tree this function
 *             visits.
 */
void walk(Listener &listener __attribute__((unused)),
          node const &root __attribute__((unused))) {}
