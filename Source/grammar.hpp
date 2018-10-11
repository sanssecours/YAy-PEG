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

#include <kdb.hpp>

#include "state.hpp"

#if defined(__clang__)
#include <spdlog/spdlog.h>
using spdlog::logger;
using std::shared_ptr;

extern shared_ptr<spdlog::logger> console;
#endif

// -- Grammar ------------------------------------------------------------------

namespace yaypeg {

using tao::pegtl::at;
using tao::pegtl::eof;
using tao::pegtl::must;
using tao::pegtl::not_at;
using tao::pegtl::nothing;
using tao::pegtl::opt;
using tao::pegtl::plus;
using tao::pegtl::seq;
using tao::pegtl::sor;
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

// [22]
struct c_indicator : one<'-', '?', ':', ',', '[', ']', '{', '}', '#', '&', '*',
                         '!', ',', '>', '\'', '"', '%', '@', '`'> {};
// [23]
struct c_flow_indicator : one<',', '[', ']', '{', '}'> {};

// [24]
struct b_line_feed : one<'\n'> {};
// [25]
struct b_carriage_return : one<'\r'> {};
// [26]
struct b_char : sor<b_line_feed, b_carriage_return> {};
// [27]
struct nb_char : seq<not_at<sor<b_char, c_byte_order_mark>>, c_printable> {};

// [31]
struct s_space : one<' '> {};
// [32]
struct s_tab : one<'\t'> {};
// [33]
struct s_white : sor<s_space, s_tab> {};
// [34]
struct ns_char : seq<not_at<s_white>, nb_char> {};

// [126]
struct ns_plain_safe;
struct ns_plain_first : sor<seq<not_at<c_indicator>, ns_char>,
                            one<'?', ':', '-'>, ns_plain_safe> {};

// [128]
struct ns_plain_safe_out : ns_char {};
// [129]
struct ns_plain_safe_in : seq<not_at<c_flow_indicator>, ns_char> {};

// [127]
struct ns_plain_safe {
  template <tao::pegtl::apply_mode ApplyMode,
            tao::pegtl::rewind_mode RewindMode,
            template <typename...> class Action,
            template <typename...> class Control, typename Input>
  static bool match(Input &input, State &state) {
    if (state.contextFlowOut() || state.contextBlockKey()) {
      return ns_plain_safe_out::match<ApplyMode, RewindMode, Action, Control>(
          input, state);
    }
    return ns_plain_safe_in::match<ApplyMode, RewindMode, Action, Control>(
        input, state);
  }
};

// [130]
struct last_was_ns_plain_safe {
  template <tao::pegtl::apply_mode ApplyMode,
            tao::pegtl::rewind_mode RewindMode,
            template <typename...> class Action,
            template <typename...> class Control, typename Input>
  static bool match(Input &, State &state) {
    return state.lastRuleWasNsChar();
  }
};
struct ns_plain_char : sor<seq<not_at<one<':', '#'>>, ns_plain_safe>,
                           seq<last_was_ns_plain_safe, one<'#'>>,
                           seq<one<':'>, at<ns_plain_safe>>> {};

struct plain_scalar : plus<ns_char> {};
struct node : until<eof, seq<plain_scalar, opt<b_line_feed>>> {};
struct yaml : must<node> {};

// ===========
// = Actions =
// ===========

/** This structure contains the default action for rules. */
template <typename Rule> struct action {
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

/** This struct contains an action for `ns_char`. */
template <> struct action<ns_char> {

  /**
   * @brief PEGTL will call this function after it matched a plain scalar.
   *
   * @param state This parameter stores the current state of the parser.
   */
  template <typename Input> static void apply(const Input &, State &state) {
    LOG("Current input matched rule `ns_char`");
    state.setLastRuleWasNsChar(true);
  }
};

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
    LOGF("Matched plain scalar with value “{}”", input.string());
    state.appendKey(input.string());
    state.setLastRuleWasNsChar(false);
  }
};

} // namespace yaypeg

#endif // ELEKTRA_PLUGIN_YAYPEG_GRAMMAR_HPP
