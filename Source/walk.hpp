/**
 * @file
 *
 * @brief This file contains the declaration of a tree walker function.
 *
 * @copyright BSD License (see LICENSE.md or https://www.libelektra.org)
 */

#ifndef ELEKTRA_PLUGIN_YAYPEG_WALK_HPP
#define ELEKTRA_PLUGIN_YAYPEG_WALK_HPP

// -- Imports ------------------------------------------------------------------

#include "listener.hpp"

#define TAO_PEGTL_NAMESPACE yaypeg

#include <tao/pegtl/contrib/parse_tree.hpp>

// -- Function -----------------------------------------------------------------

/**
 * @brief This function walks a syntax tree calling methods of the given
 *        listener.
 *
 * @param listener This argument specifies the listener which this function
 *                 uses to convert the syntax tree to a key set.
 * @param root This variable stores the root of the tree this function visits.
 */
void walk(Listener &listener __attribute__((unused)),
          tao::TAO_PEGTL_NAMESPACE::parse_tree::node const &root
          __attribute__((unused)));

#endif // ELEKTRA_PLUGIN_YAYPEG_WALK_HPP
