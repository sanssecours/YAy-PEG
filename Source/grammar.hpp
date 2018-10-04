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

#include <tao/pegtl.hpp>

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
   */
  template <typename Input> static void apply(const Input &input) {
    std::cout << "Found plain scalar “" << input.string() << "”\n";
  }
};

} // namespace yaypeg

#endif // ELEKTRA_PLUGIN_YAYPEG_GRAMMAR_HPP
