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

using std::cout;

namespace pegtl = tao::pegtl;

using pegtl::eof;
using pegtl::identifier;
using pegtl::nothing;
using pegtl::until;

// -- Grammar ------------------------------------------------------------------

namespace yaypeg {

struct plain_scalar : identifier {};

struct yaml : until<plain_scalar, eof> {};

template <typename Rule> struct action : nothing<Rule> {};

template <> struct action<plain_scalar> {
  template <typename Input> static void apply(const Input &input) {
    cout << "Found plain scalar “" << input.string() << "”";
  }
};

} // namespace yaypeg

#endif // ELEKTRA_PLUGIN_YAYPEG_GRAMMAR_HPP
