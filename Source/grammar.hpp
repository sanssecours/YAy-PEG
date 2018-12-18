/**
 * @file
 *
 * @brief This file contains a basic PEGTL YAML grammar.
 *
 * @copyright BSD License (see LICENSE.md or https://www.libelektra.org)
 */

#ifndef ELEKTRA_PLUGIN_YAYPEG_GRAMMAR_HPP
#define ELEKTRA_PLUGIN_YAYPEG_GRAMMAR_HPP

// -- Macros -------------------------------------------------------------------

#define TAO_PEGTL_NAMESPACE yaypeg

#define SPDLOG_TRACE_ON

#if defined(__clang__)
#define LOGF(fmt, ...)                                                         \
  console->trace("{}:{}: " fmt, __FILE__, __LINE__, __VA_ARGS__)
#else
#define LOGF(fmt, ...)
#endif

#if defined(__clang__)
#define LOG(text) console->trace("{}:{}: {}", __FILE__, __LINE__, text)
#else
#define LOG(text)
#endif

// -- Imports ------------------------------------------------------------------

#include <iostream>

#include <tao/pegtl.hpp>
#include <tao/pegtl/analyze.hpp>

#include <kdb.hpp>

#include "state.hpp"

#if defined(__clang__)
#include <spdlog/spdlog.h>
using spdlog::logger;
using std::shared_ptr;

extern shared_ptr<spdlog::logger> console;
#endif

// -- Rules --------------------------------------------------------------------

namespace yaypeg {

using tao::yaypeg::seq;

// ===========
// = Grammar =
// ===========

struct push_indent {
  using analyze_t =
      tao::yaypeg::analysis::generic<tao::yaypeg::analysis::rule_type::ANY>;

  template <tao::yaypeg::apply_mode, tao::yaypeg::rewind_mode,
            template <typename...> class, template <typename...> class,
            typename Input>
  static bool match(Input &input, State &state) {
    size_t indent = 0;
    for (auto current = input.begin();
         input.size(indent + 1) >= indent + 1 && *current == ' '; ++current) {
      ++indent;
    }

    if (state.indentation.empty() || state.indentation.back() != indent) {
      LOGF("Indentations: {}", state.toString());
      state.indentation.push_back(indent);
    }
    return true;
  }
};

struct yaml : seq<push_indent> {};

// ===========
// = Actions =
// ===========

template <typename Rule> struct base {
  template <typename Input> static void apply(const Input &, State &) {
    LOG("Apply default action");
  }
};

template <typename Rule> struct action : base<Rule> {};

} // namespace yaypeg

#endif // ELEKTRA_PLUGIN_YAYPEG_GRAMMAR_HPP
