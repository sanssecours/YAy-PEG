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

struct plain_scalar : tao::pegtl::identifier {};

struct yaml : tao::pegtl::until<plain_scalar, tao::pegtl::eof> {};

template <typename Rule> struct action : tao::pegtl::nothing<Rule> {};

template <> struct action<plain_scalar> {
  template <typename Input> static void apply(const Input &input) {
    std::cout << "Found plain scalar “" << input.string() << "”\n";
  }
};

} // namespace yaypeg

#endif // ELEKTRA_PLUGIN_YAYPEG_GRAMMAR_HPP