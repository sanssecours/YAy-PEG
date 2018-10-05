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

// ===========
// = Grammar =
// ===========

struct plain_scalar : tao::pegtl::identifier {};

struct node
    : tao::pegtl::until<tao::pegtl::eof,
                        tao::pegtl::sor<plain_scalar, tao::pegtl::space>> {};
struct yaml : tao::pegtl::must<node> {};

// ===========
// = Actions =
// ===========

// Disable actions for rules
template <typename Rule> struct action : tao::pegtl::nothing<Rule> {};

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
  static void apply(const Input &input, State &state __attribute__((unused))) {
    std::cout << "Found plain scalar “" << input.string() << "”\n";
  }
};

} // namespace yaypeg

#endif // ELEKTRA_PLUGIN_YAYPEG_GRAMMAR_HPP
