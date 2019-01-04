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

#include "context.hpp"

#if defined(__clang__)
#include <spdlog/spdlog.h>
using spdlog::logger;
using std::shared_ptr;

extern shared_ptr<spdlog::logger> console;
#endif

// -- Rules --------------------------------------------------------------------

namespace yaypeg {

using tao::TAO_PEGTL_NAMESPACE::eolf;
using tao::TAO_PEGTL_NAMESPACE::identifier;
using tao::TAO_PEGTL_NAMESPACE::one;
using tao::TAO_PEGTL_NAMESPACE::plus;
using tao::TAO_PEGTL_NAMESPACE::seq;
using tao::TAO_PEGTL_NAMESPACE::sor;
using tao::TAO_PEGTL_NAMESPACE::space;
using tao::TAO_PEGTL_NAMESPACE::success;

// ===========
// = Grammar =
// ===========

struct push_indent {
  using analyze_t = tao::TAO_PEGTL_NAMESPACE::analysis::generic<
      tao::TAO_PEGTL_NAMESPACE::analysis::rule_type::ANY>;

  template <tao::TAO_PEGTL_NAMESPACE::apply_mode,
            tao::TAO_PEGTL_NAMESPACE::rewind_mode, template <typename...> class,
            template <typename...> class, typename Input>
  static bool match(Input &input, Context &context) {
    size_t indent = 0;
    while (input.peek_char(indent) == ' ') {
      ++indent;
    }
    context.indentation.push_back(indent);
    LOGF("Context: {}", context.toString());
    return true;
  }
};

template <typename Comparator, bool DefaultValue = false> struct indent {
  using analyze_t = tao::TAO_PEGTL_NAMESPACE::analysis::generic<
      tao::TAO_PEGTL_NAMESPACE::analysis::rule_type::ANY>;

  template <tao::TAO_PEGTL_NAMESPACE::apply_mode,
            tao::TAO_PEGTL_NAMESPACE::rewind_mode, template <typename...> class,
            template <typename...> class, typename Input>
  static bool match(Input &, Context &context) {
    size_t levels = context.indentation.size();
    if (levels <= 1) {
      return DefaultValue;
    }
    return Comparator{}(context.indentation[levels - 1],
                        context.indentation[levels - 2]);
  }
};

struct consume_indent {
  using analyze_t = tao::TAO_PEGTL_NAMESPACE::analysis::generic<
      tao::TAO_PEGTL_NAMESPACE::analysis::rule_type::ANY>;

  template <tao::TAO_PEGTL_NAMESPACE::apply_mode,
            tao::TAO_PEGTL_NAMESPACE::rewind_mode, template <typename...> class,
            template <typename...> class, typename Input>
  static bool match(Input &input, Context &context) {
    size_t indent = context.indentation.back();
    size_t spaces = 0;
    while (input.peek_char(spaces) == ' ') {
      spaces++;
    }
    if (indent != spaces) {
      LOGF("Expected {} spaces, but found {} spaces", indent, spaces);
      return false;
    }
    input.bump(context.indentation.back());
    LOGF("Consumed {} spaces", context.indentation.back());
    return true;
  }
};

struct pop_indent : success {};

template <typename... Rules>
struct with_updated_indent
    : seq<push_indent, sor<seq<Rules...>, pop_indent>, pop_indent> {};

struct child;

struct more_indent : indent<std::greater<size_t>, true> {};
struct same_indent : indent<std::equal_to<size_t>> {};

struct scalar : identifier {};
struct key : scalar {};
struct key_value_indicator : seq<key, one<':'>> {};
struct value : scalar {};
struct pair
    : seq<key_value_indicator, sor<seq<space, value, eolf>, seq<eolf, child>>> {
};
struct map : with_updated_indent<more_indent, plus<consume_indent, pair>> {};

struct child : sor<map, value> {};
struct yaml : child {};

// ===========
// = Actions =
// ===========

template <typename Rule> struct base {
  template <typename Input> static void apply(const Input &, Context &) {
    LOG("Apply default action");
  }
};

template <typename Rule> struct action : base<Rule> {};

template <> struct action<pop_indent> : base<pop_indent> {
  template <typename Input> static void apply(const Input &, Context &context) {
    if (context.indentation.empty()) {
      return;
    }
    context.indentation.pop_back();
    LOGF("Context: {}", context.toString());
  }
};

template <> struct action<key> : base<key> {
  template <typename Input>
  static void apply(const Input &input, Context &context) {
    context.key = input.string();
    LOGF("Possible Key: “{}”", context.key);
  }
};

template <> struct action<key_value_indicator> : base<key_value_indicator> {
  template <typename Input> static void apply(const Input &, Context &context) {
    kdb::Key child{context.parents.top().getName(), KEY_END};
    child.addBaseName(context.key);
    context.parents.push(child);

    LOGF("🔑: “{}”", context.key);
  }
};

template <> struct action<value> : base<value> {
  template <typename Input>
  static void apply(const Input &input, Context &context) {
    kdb::Key key = context.parents.top();
    key.setString(input.string());
    context.keys.append(key);
  }
};

template <> struct action<pair> : base<pair> {
  template <typename Input> static void apply(const Input &, Context &context) {
    context.parents.pop();
  }
};

template <> struct action<child> : base<child> {
  template <typename Input> static void apply(const Input &input, Context &) {
    LOGF("🧒🏾: “{}”", input.string());
  }
};

} // namespace yaypeg

#endif // ELEKTRA_PLUGIN_YAYPEG_GRAMMAR_HPP
