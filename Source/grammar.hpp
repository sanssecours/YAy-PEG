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

using tao::yaypeg::at;
using tao::yaypeg::bol;
using tao::yaypeg::eof;
using tao::yaypeg::failure;
using tao::yaypeg::must;
using tao::yaypeg::not_at;
using tao::yaypeg::nothing;
using tao::yaypeg::opt;
using tao::yaypeg::plus;
using tao::yaypeg::rep;
using tao::yaypeg::seq;
using tao::yaypeg::sor;
using tao::yaypeg::star;
using tao::yaypeg::success;
using tao::yaypeg::until;
using tao::yaypeg::utf8::one;
using tao::yaypeg::utf8::ranges;

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

struct pop_indent : success {};

struct yaml : seq<push_indent> {};

// ===========
// = Actions =
// ===========

/** This structure contains the default action for rules. */
template <typename Rule> struct base {
  /**
   * @brief PEGTL will call this function after it matched a rule that has no
   *        explicit action.
   *
   * @param input This parameter stores the input matched by the grammar rule.
   * @param state This parameter stores the current state of the parser.
   */
  template <typename Input> static void apply(const Input &, State &state) {
    LOG("Apply default action");
    state.setLastRuleWasNsChar(false);
  }
};
template <typename Rule> struct action : base<Rule> {};

template <> struct action<pop_indent> {
  static void apply0(State &state) { state.indentation.pop_back(); }
};

} // namespace yaypeg

#endif // ELEKTRA_PLUGIN_YAYPEG_GRAMMAR_HPP
