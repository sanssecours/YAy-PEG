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
// [28]
struct b_break
    : sor<seq<b_carriage_return, b_line_feed>, b_carriage_return, b_line_feed> {
};

// [30]
struct b_non_content : b_break {};

// [31]
struct s_space : one<' '> {};
// [32]
struct s_tab : one<'\t'> {};
// [33]
struct s_white : sor<s_space, s_tab> {};
// [34]
struct ns_char : seq<not_at<s_white>, nb_char> {};

// [63]
template <size_t repetitions> struct s_indent : rep<repetitions, s_space> {};

template <size_t whitespaces> struct s_indent_plus {
  using analyze_t =
      tao::yaypeg::analysis::generic<tao::yaypeg::analysis::rule_type::ANY>;

  template <tao::yaypeg::apply_mode, tao::yaypeg::rewind_mode,
            template <typename...> class, template <typename...> class,
            typename Input>
  static bool match(Input &input, State &state) {
    if (input.size(whitespaces) <= whitespaces) {
      return false;
    }
    auto end = input.begin() + (whitespaces - state.indentation.back());
    for (auto current = input.begin(); current != end; current++) {
      LOGF("Current: “{}”", *current);
      if (*current != ' ') {
        return false;
      }
    }
    input.bump(whitespaces);
    state.indentation.push_back(whitespaces);
    return true;
  }
};

struct push_indent {
  using analyze_t =
      tao::yaypeg::analysis::generic<tao::yaypeg::analysis::rule_type::ANY>;

  template <tao::yaypeg::apply_mode, tao::yaypeg::rewind_mode,
            template <typename...> class, template <typename...> class,
            typename Input>
  static bool match(Input &input, State &state) {
    // TODO: Check for non-empty line
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

// [66]
struct s_separate_in_line : sor<plus<s_white>, bol> {};

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
  template <tao::yaypeg::apply_mode ApplyMode,
            tao::yaypeg::rewind_mode RewindMode,
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
  template <tao::yaypeg::apply_mode ApplyMode,
            tao::yaypeg::rewind_mode RewindMode,
            template <typename...> class Action,
            template <typename...> class Control, typename Input>
  static bool match(Input &, State &state) {
    return state.lastRuleWasNsChar();
  }
};
struct ns_plain_char : sor<seq<not_at<one<':', '#'>>, ns_plain_safe>,
                           seq<last_was_ns_plain_safe, one<'#'>>,
                           seq<one<':'>, at<ns_plain_safe>>> {};

// [132]
struct nb_ns_plain_in_line : star<seq<star<s_white>>, ns_plain_char> {};
// [133]
struct ns_plain_one_line : seq<ns_plain_first, nb_ns_plain_in_line> {};

struct plain_scalar : plus<ns_char> {};
struct node : until<eof, seq<plain_scalar, opt<b_line_feed>>> {};
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

template <> struct action<pop_indent> {
  static void apply0(State &state) { state.indentation.pop_back(); }
};

/** This struct contains an action for plain scalars. */
template <> struct action<plain_scalar> : base<plain_scalar> {

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
  }
};

} // namespace yaypeg

#endif // ELEKTRA_PLUGIN_YAYPEG_GRAMMAR_HPP
