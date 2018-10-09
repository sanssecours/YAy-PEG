/**
 * @file
 *
 * @brief This file contains a basic PEGTL YAML grammar.
 *
 * @copyright BSD License (see LICENSE.md or https://www.libelektra.org)
 */

#ifndef ELEKTRA_PLUGIN_YAYPEG_GRAMMAR_HPP
#define ELEKTRA_PLUGIN_YAYPEG_GRAMMAR_HPP

// -- Imports ------------------------------------------------------------------

#include <iostream>

#include <tao/pegtl.hpp>

#include <kdb.hpp>

#include "state.hpp"

// -- Grammar ------------------------------------------------------------------

namespace yaypeg {

using tao::pegtl::eof;
using tao::pegtl::identifier;
using tao::pegtl::must;
using tao::pegtl::nothing;
using tao::pegtl::sor;
using tao::pegtl::space;
using tao::pegtl::until;

// ===========
// = Grammar =
// ===========

struct plain_scalar : identifier {};

struct node : until<eof, sor<plain_scalar, space>> {};
struct yaml : must<node> {};

// ===========
// = Actions =
// ===========

// Disable actions for rules
template <typename Rule> struct action : nothing<Rule> {};

/** This struct contains an action for plain scalars. */
template <> struct action<plain_scalar> {

  /**
   * @brief PEGTL will call this function after it matched a plain scalar.
   *
   * @param input This parameter stores the input matched by the grammar rule.
   * @param state This parameter stores data the parser uses to create a key
   *              set from the YAML data.
   */
  template <typename Input>
  static void apply(const Input &input, State &state) {
    state.appendKey(input.string());
  }
};

} // namespace yaypeg

#endif // ELEKTRA_PLUGIN_YAYPEG_GRAMMAR_HPP
