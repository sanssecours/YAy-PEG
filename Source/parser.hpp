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

#include <functional>
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

// -- Functions ----------------------------------------------------------------

namespace {

/**
 * @brief This function converts a YAML scalar to a string.
 *
 * @param text This string contains a YAML scalar (including quote
 *             characters).
 *
 * @return A string without leading and trailing quote characters
 */
std::string scalarToText(std::string const &text) {
  if (text.length() == 0) {
    return text;
  }
  if (*(text.begin()) == '"' || *(text.begin()) == '\'') {
    return text.substr(1, text.length() - 2);
  }
  return text;
}

} // namespace

// -- Rules & Actions ----------------------------------------------------------

namespace yaypeg {

using tao::TAO_PEGTL_NAMESPACE::at;
using tao::TAO_PEGTL_NAMESPACE::blank;
using tao::TAO_PEGTL_NAMESPACE::bol;
using tao::TAO_PEGTL_NAMESPACE::digit;
using tao::TAO_PEGTL_NAMESPACE::eof;
using tao::TAO_PEGTL_NAMESPACE::eolf;
using tao::TAO_PEGTL_NAMESPACE::failure;
using tao::TAO_PEGTL_NAMESPACE::identifier;
using tao::TAO_PEGTL_NAMESPACE::minus;
using tao::TAO_PEGTL_NAMESPACE::must;
using tao::TAO_PEGTL_NAMESPACE::not_at;
using tao::TAO_PEGTL_NAMESPACE::nothing;
using tao::TAO_PEGTL_NAMESPACE::opt;
using tao::TAO_PEGTL_NAMESPACE::plus;
using tao::TAO_PEGTL_NAMESPACE::rep;
using tao::TAO_PEGTL_NAMESPACE::seq;
using tao::TAO_PEGTL_NAMESPACE::sor;
using tao::TAO_PEGTL_NAMESPACE::star;
using tao::TAO_PEGTL_NAMESPACE::success;
using tao::TAO_PEGTL_NAMESPACE::until;
using tao::TAO_PEGTL_NAMESPACE::xdigit;
using tao::TAO_PEGTL_NAMESPACE::utf8::one;
using tao::TAO_PEGTL_NAMESPACE::utf8::range;
using tao::TAO_PEGTL_NAMESPACE::utf8::ranges;

// ==========================
// = Parser Context Updates =
// ==========================

template <typename Rule> struct base {
  template <typename Input> static void apply(const Input &, State &state) {
    state.lastWasNsChar = false;
  }
};
template <typename Rule> struct action : base<Rule> {};

struct ns_char;
template <> struct action<ns_char> {
  template <typename Input> static void apply(const Input &, State &state) {
    state.lastWasNsChar = true;
  }
};

struct push_indent {
  using analyze_t = tao::TAO_PEGTL_NAMESPACE::analysis::generic<
      tao::TAO_PEGTL_NAMESPACE::analysis::rule_type::ANY>;

  template <tao::TAO_PEGTL_NAMESPACE::apply_mode,
            tao::TAO_PEGTL_NAMESPACE::rewind_mode, template <typename...> class,
            template <typename...> class, typename Input>
  static bool match(Input &input, State &state) {
    size_t indent = 0;
    while (input.peek_char(indent) == ' ') {
      ++indent;
    }
    state.indentation.push_back(indent);
    LOGF("State: {}", state.toString());
    return true;
  }
};

struct pop_indent : success {};
template <> struct action<pop_indent> {
  template <typename Input> static void apply(const Input &, State &state) {
    if (state.indentation.empty()) {
      return;
    }
    state.indentation.pop_back();
    LOGF("State: {}", state.toString());
  }
};

template <State::Context Context> struct push_context {
  using analyze_t = tao::TAO_PEGTL_NAMESPACE::analysis::generic<
      tao::TAO_PEGTL_NAMESPACE::analysis::rule_type::ANY>;

  template <tao::TAO_PEGTL_NAMESPACE::apply_mode,
            tao::TAO_PEGTL_NAMESPACE::rewind_mode, template <typename...> class,
            template <typename...> class, typename Input>
  static bool match(Input &, State &state) {
    state.context.push(Context);
    return true;
  }
};

struct pop_context : success {};
template <> struct action<pop_context> {
  template <typename Input> static void apply(const Input &, State &state) {
    state.context.pop();
  }
};

template <typename UpdateStateRule, typename RevertStateRule, typename... Rules>
struct with_updated_state
    : seq<UpdateStateRule, sor<seq<Rules...>, seq<RevertStateRule, failure>>,
          RevertStateRule> {};

template <typename... Rules>
struct with_updated_indent
    : with_updated_state<push_indent, pop_indent, Rules...> {};

template <State::Context Context, typename... Rules>
struct with_updated_context
    : with_updated_state<push_context<Context>, pop_context, Rules...> {};

// =========================
// = Parser Context Checks =
// =========================

template <typename Comparator, bool DefaultValue = false> struct indent {
  using analyze_t = tao::TAO_PEGTL_NAMESPACE::analysis::generic<
      tao::TAO_PEGTL_NAMESPACE::analysis::rule_type::ANY>;

  template <tao::TAO_PEGTL_NAMESPACE::apply_mode,
            tao::TAO_PEGTL_NAMESPACE::rewind_mode, template <typename...> class,
            template <typename...> class, typename Input>
  static bool match(Input &, State &state) {
    size_t levels = state.indentation.size();
    if (levels <= 1) {
      return DefaultValue;
    }
    return Comparator{}(state.indentation[levels - 1],
                        state.indentation[levels - 2]);
  }
};

struct less_indent : indent<std::less<size_t>> {};
struct same_indent : indent<std::equal_to<size_t>> {};
struct more_indent : indent<std::greater<size_t>, true> {};

// ===========
// = Grammar =
// ===========

// [1]
struct c_printable
    : sor<one<'\t', '\n', '\r', 0x85>,
          ranges<' ', 0x7E, 0xA0, 0xD7FF, 0xE000, 0xFFFD, 0x10000, 0x10FFFF>> {
};
// [2]
struct nb_json : sor<one<0x9>, range<0x20, 0x10FFFF>> {};

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

// [29]
struct b_as_line_feed : b_break {};
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
// [35]
struct ns_dec_digit : digit {};
// [36]
struct ns_hex_digit : xdigit {};

// [42]
struct ns_esc_null : one<'0'> {};
// [43]
struct ns_esc_bell : one<'a'> {};
// [44]
struct ns_esc_backspace : one<'b'> {};
// [45]
struct ns_esc_horizontal_tab : one<'t', 0x9> {};
// [46]
struct ns_esc_line_feed : one<'n'> {};
// [47]
struct ns_esc_vertical_tab : one<'v'> {};
// [48]
struct ns_esc_form_feed : one<'f'> {};
// [49]
struct ns_esc_carriage_return : one<'r'> {};
// [50]
struct ns_esc_escape : one<'e'> {};
// [51]
struct ns_esc_space : one<0x20> {};
// [52]
struct ns_esc_double_quote : one<'"'> {};
// [53]
struct ns_esc_slash : one<'/'> {};
// [54]
struct ns_esc_backslash : one<'\\'> {};
// [55]
struct ns_esc_next_line : one<'N'> {};
// [56]
struct ns_esc_non_breaking_space : one<'_'> {};
// [57]
struct ns_esc_line_separator : one<'L'> {};
// [58]
struct ns_esc_paragraph_separator : one<'P'> {};
// [59]
struct ns_esc_8_bit : seq<one<'x'>, rep<2, ns_hex_digit>> {};
// [60]
struct ns_esc_16_bit : seq<one<'u'>, rep<4, ns_hex_digit>> {};
// [61]
struct ns_esc_32_bit : seq<one<'U'>, rep<8, ns_hex_digit>> {};

// [62]
struct c_ns_esc_char
    : seq<one<'\\'>,
          sor<ns_esc_null, ns_esc_bell, ns_esc_backspace, ns_esc_horizontal_tab,
              ns_esc_line_feed, ns_esc_vertical_tab, ns_esc_form_feed,
              ns_esc_carriage_return, ns_esc_escape, ns_esc_space,
              ns_esc_double_quote, ns_esc_slash, ns_esc_backslash,
              ns_esc_next_line, ns_esc_non_breaking_space,
              ns_esc_line_separator, ns_esc_paragraph_separator, ns_esc_8_bit,
              ns_esc_16_bit, ns_esc_32_bit>> {};

// [63]
struct s_indent {
  using analyze_t = tao::TAO_PEGTL_NAMESPACE::analysis::generic<
      tao::TAO_PEGTL_NAMESPACE::analysis::rule_type::ANY>;

  template <tao::TAO_PEGTL_NAMESPACE::apply_mode,
            tao::TAO_PEGTL_NAMESPACE::rewind_mode, template <typename...> class,
            template <typename...> class, typename Input>
  static bool match(Input &input, State &state) {
    size_t indent = state.indentation.back();
    size_t spaces = 0;
    while (input.peek_char(spaces) == ' ') {
      spaces++;
    }
    if (indent != spaces) {
      LOGF("Expected {} spaces, but found {} spaces", indent, spaces);
      return false;
    }
    input.bump(state.indentation.back());
    LOGF("Consumed {} spaces", state.indentation.back());
    return true;
  }
};

// [66]
struct s_separate_in_line : sor<plus<s_white>, bol> {};
// [68]
struct s_block_line_prefix : s_indent {};
// [69]
struct s_flow_line_prefix : seq<s_indent, opt<s_separate_in_line>> {};
// [67]
struct s_line_prefix {
  using analyze_t = tao::TAO_PEGTL_NAMESPACE::analysis::generic<
      tao::TAO_PEGTL_NAMESPACE::analysis::rule_type::ANY>;

  template <tao::TAO_PEGTL_NAMESPACE::apply_mode ApplyMode,
            tao::TAO_PEGTL_NAMESPACE::rewind_mode RewindMode,
            template <typename...> class Action,
            template <typename...> class Control, typename Input>
  static bool match(Input &input, State &state) {
    if (state.context.top() == State::Context::BLOCK_OUT ||
        state.context.top() == State::Context::BLOCK_IN) {
      return s_block_line_prefix::match<ApplyMode, RewindMode, Action, Control>(
          input, state);
    }
    return s_flow_line_prefix::match<ApplyMode, RewindMode, Action, Control>(
        input, state);
  }
};

// [70]
struct s_indent_smaller : with_updated_indent<less_indent, s_indent> {};
struct l_empty : seq<sor<s_line_prefix, s_indent_smaller>, b_as_line_feed> {};
// [71]
struct b_l_trimmed : seq<b_non_content, plus<l_empty>> {};
// [72]
struct b_as_space : b_break {};
// [73]
struct b_l_folded : sor<b_l_trimmed, b_as_space> {};
// [74]
struct s_flow_folded
    : seq<opt<s_separate_in_line>,
          with_updated_context<State::Context::FLOW_IN, b_l_folded>,
          s_flow_line_prefix> {};

// [107]
struct nb_double_char : sor<c_ns_esc_char, minus<nb_json, one<'\\', '"'>>> {};
// [108]
struct ns_double_char : minus<nb_double_char, s_white> {};
// [109]
struct nb_double_text;
struct c_double_quoted : seq<one<'"'>, nb_double_text, one<'"'>> {};

// [111]
struct nb_double_one_line : star<nb_double_char> {};
// [116]
struct nb_ns_double_in_line;
struct s_double_next_line;
struct nb_double_multi_line
    : seq<nb_ns_double_in_line, sor<s_double_next_line, star<s_white>>> {};
// [110]
struct nb_double_text {
  using analyze_t = tao::TAO_PEGTL_NAMESPACE::analysis::generic<
      tao::TAO_PEGTL_NAMESPACE::analysis::rule_type::ANY>;

  template <tao::TAO_PEGTL_NAMESPACE::apply_mode ApplyMode,
            tao::TAO_PEGTL_NAMESPACE::rewind_mode RewindMode,
            template <typename...> class Action,
            template <typename...> class Control, typename Input>
  static bool match(Input &input, State &state) {
    if (state.context.top() == State::Context::FLOW_OUT ||
        state.context.top() == State::Context::FLOW_IN) {
      return nb_double_multi_line::match<ApplyMode, RewindMode, Action,
                                         Control>(input, state);
    }
    return nb_double_one_line::match<ApplyMode, RewindMode, Action, Control>(
        input, state);
  }
};

// [112]
struct s_double_escaped
    : seq<star<s_white>, one<'\\'>, b_non_content,
          with_updated_context<State::Context::FLOW_IN, star<l_empty>>,
          s_flow_line_prefix> {};
// [113]
struct s_double_break : sor<s_double_escaped, s_flow_folded> {};
// [114]
struct nb_ns_double_in_line : star<star<s_white>, ns_double_char> {};
// [115]
struct s_double_next_line
    : seq<s_double_break, opt<ns_double_char, nb_ns_double_in_line,
                              sor<s_double_next_line, star<s_white>>>> {};

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
  using analyze_t = tao::TAO_PEGTL_NAMESPACE::analysis::generic<
      tao::TAO_PEGTL_NAMESPACE::analysis::rule_type::ANY>;

  template <tao::TAO_PEGTL_NAMESPACE::apply_mode ApplyMode,
            tao::TAO_PEGTL_NAMESPACE::rewind_mode RewindMode,
            template <typename...> class Action,
            template <typename...> class Control, typename Input>
  static bool match(Input &input, State &state) {
    if (state.context.top() == State::Context::FLOW_OUT ||
        state.context.top() == State::Context::BLOCK_KEY) {
      return ns_plain_safe_out::match<ApplyMode, RewindMode, Action, Control>(
          input, state);
    }
    return ns_plain_safe_in::match<ApplyMode, RewindMode, Action, Control>(
        input, state);
  }
};

// [130]
struct last_was_ns_plain_safe {
  using analyze_t = tao::TAO_PEGTL_NAMESPACE::analysis::generic<
      tao::TAO_PEGTL_NAMESPACE::analysis::rule_type::ANY>;

  template <tao::TAO_PEGTL_NAMESPACE::apply_mode ApplyMode,
            tao::TAO_PEGTL_NAMESPACE::rewind_mode RewindMode,
            template <typename...> class Action,
            template <typename...> class Control, typename Input>
  static bool match(Input &, State &state) {
    return state.lastWasNsChar;
  }
};
struct ns_plain_char : sor<seq<not_at<one<':', '#'>>, ns_plain_safe>,
                           seq<last_was_ns_plain_safe, one<'#'>>,
                           seq<one<':'>, at<ns_plain_safe>>> {};

// [132]
struct nb_ns_plain_in_line : star<seq<star<s_white>>, ns_plain_char> {};
// [133]
struct ns_plain_one_line : seq<ns_plain_first, nb_ns_plain_in_line> {};
// [134]
struct s_ns_plain_next_line
    : seq<s_flow_folded, ns_plain_char, nb_ns_plain_in_line> {};
// [135]
struct ns_plain_multi_line
    : seq<ns_plain_one_line, star<s_ns_plain_next_line>> {};

// [131]
struct ns_plain {
  using analyze_t = tao::TAO_PEGTL_NAMESPACE::analysis::generic<
      tao::TAO_PEGTL_NAMESPACE::analysis::rule_type::ANY>;

  template <tao::TAO_PEGTL_NAMESPACE::apply_mode ApplyMode,
            tao::TAO_PEGTL_NAMESPACE::rewind_mode RewindMode,
            template <typename...> class Action,
            template <typename...> class Control, typename Input>
  static bool match(Input &input, State &state) {
    if (state.context.top() == State::Context::FLOW_OUT ||
        state.context.top() == State::Context::FLOW_IN) {
      return ns_plain_multi_line::match<ApplyMode, RewindMode, Action, Control>(
          input, state);
    }
    return ns_plain_one_line::match<ApplyMode, RewindMode, Action, Control>(
        input, state);
  }
};

struct plain_scalar : ns_plain {};
struct double_quoted_scalar : c_double_quoted {};

struct child;

struct scalar : sor<plain_scalar, double_quoted_scalar> {};
struct key : scalar {};
struct key_value_indicator : seq<key, star<blank>, one<':'>> {};
struct value : scalar {};
struct pair
    : seq<key_value_indicator, sor<seq<blank, value, eolf>, seq<eolf, child>>> {
};
struct map : with_updated_indent<more_indent, plus<s_indent, pair>> {};
struct indented_scalar : with_updated_indent<more_indent, s_indent, value> {};

struct child : sor<map, indented_scalar> {};
struct yaml : child {};

// ================
// = Data Updates =
// ================

template <> struct action<key> : base<key> {
  template <typename Input>
  static void apply(const Input &input, State &state) {
    state.key = scalarToText(input.string());
    LOGF("Possible Key: “{}”", state.key);
  }
};

template <> struct action<key_value_indicator> : base<key_value_indicator> {
  template <typename Input> static void apply(const Input &, State &state) {
    kdb::Key child{state.parents.top().getName(), KEY_END};
    child.addBaseName(state.key);
    state.parents.push(child);

    LOGF("🔑: “{}”", state.key);
  }
};

template <> struct action<value> : base<value> {
  template <typename Input>
  static void apply(const Input &input, State &state) {
    kdb::Key key = state.parents.top();
    key.setString(scalarToText(input.string()));
    state.keys.append(key);
  }
};

template <> struct action<pair> : base<pair> {
  template <typename Input> static void apply(const Input &, State &state) {
    state.parents.pop();
  }
};

} // namespace yaypeg

#endif // ELEKTRA_PLUGIN_YAYPEG_GRAMMAR_HPP
