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
using tao::pegtl::not_at;
using tao::pegtl::nothing;
using tao::pegtl::plus;
using tao::pegtl::seq;
using tao::pegtl::sor;
using tao::pegtl::space;
using tao::pegtl::until;
using tao::pegtl::utf8::one;
using tao::pegtl::utf8::ranges;

// ===========
// = Grammar =
// ===========

// [1]
struct c_printable
    : sor<one<'\t', '\n', '\r', 0x85>,
          ranges<' ', 0x7E, 0xA0, 0xD7FF, 0xE000, 0xFFFD, 0x10000, 0x10FFFF>> {
};
// [3]
struct c_byte_order_mark : one<0xFEFF> {};

// [24]
struct b_line_feed : one<'\n'> {};
// [25]
struct b_carriage_return : one<'\r'> {};
// [26]
struct b_char : sor<b_line_feed, b_carriage_return> {};
// [27]
struct nb_char : seq<c_printable, not_at<sor<b_char, c_byte_order_mark>>> {};

// [31]
struct s_space : one<' '> {};
// [32]
struct s_tab : one<'\t'> {};
// [33]
struct s_white : sor<s_space, s_tab> {};
// [34]
struct ns_char : seq<nb_char> {};

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
