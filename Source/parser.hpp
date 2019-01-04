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

// -- Rules --------------------------------------------------------------------

namespace yaypeg {

using tao::TAO_PEGTL_NAMESPACE::at;
using tao::TAO_PEGTL_NAMESPACE::blank;
using tao::TAO_PEGTL_NAMESPACE::bol;
using tao::TAO_PEGTL_NAMESPACE::eof;
using tao::TAO_PEGTL_NAMESPACE::eolf;
using tao::TAO_PEGTL_NAMESPACE::failure;
using tao::TAO_PEGTL_NAMESPACE::identifier;
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
using tao::TAO_PEGTL_NAMESPACE::utf8::one;
using tao::TAO_PEGTL_NAMESPACE::utf8::ranges;

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

struct pop_indent : success {};

template <typename... Rules>
struct with_updated_indent
    : seq<push_indent, sor<seq<Rules...>, seq<pop_indent, failure>>,
          pop_indent> {};

struct child;

struct more_indent : indent<std::greater<size_t>, true> {};
struct same_indent : indent<std::equal_to<size_t>> {};

struct scalar : identifier {};
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

// ===========
// = Actions =
// ===========

template <typename Rule> struct base {
  template <typename Input> static void apply(const Input &, State &) {
    LOG("Apply default action");
  }
};

template <typename Rule> struct action : base<Rule> {};

template <> struct action<pop_indent> : base<pop_indent> {
  template <typename Input> static void apply(const Input &, State &state) {
    if (state.indentation.empty()) {
      return;
    }
    state.indentation.pop_back();
    LOGF("State: {}", state.toString());
  }
};

template <> struct action<key> : base<key> {
  template <typename Input>
  static void apply(const Input &input, State &state) {
    state.key = input.string();
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
    key.setString(input.string());
    state.keys.append(key);
  }
};

template <> struct action<pair> : base<pair> {
  template <typename Input> static void apply(const Input &, State &state) {
    state.parents.pop();
  }
};

template <> struct action<child> : base<child> {
  template <typename Input>
  static void apply(const Input &input __attribute__((unused)), State &) {
    LOGF("🧒🏾: “{}”", input.string());
  }
};

} // namespace yaypeg

#endif // ELEKTRA_PLUGIN_YAYPEG_GRAMMAR_HPP
