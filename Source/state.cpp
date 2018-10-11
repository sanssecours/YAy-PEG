/**
 * @file
 *
 * @brief This file contains the implementation of a class used by the PEGTL
 *        parser to create a key set.
 *
 * @copyright BSD License (see LICENSE.md or https://www.libelektra.org)
 */

// -- Imports ------------------------------------------------------------------

#include <kdb.hpp>

#include "state.hpp"

using std::string;

using kdb::Key;
using kdb::KeySet;

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
string scalarToText(string const &text) {
  if (text.length() == 0) {
    return text;
  }
  if (*(text.begin()) == '"' || *(text.begin()) == '\'') {
    return text.substr(1, text.length() - 2);
  }
  return text;
}

} // namespace

// -- Class --------------------------------------------------------------------

namespace yaypeg {

/**
 * @brief This constructor creates a State using the given parent key.
 *
 * @param parent This argument specifies the parent key of the key set that
 *               this class stores.
 */
State::State(Key const &parent) { parents.push(parent.dup()); }

/**
 * @brief This method checks if the parser currently analyzes an implicit
 *        block key.
 *
 * @retval true If the current input is part of an implicit block key
 * @retval false If the current input is not part of an implicit block key
 */
bool State::contextBlockKey() const { return context == Context::block_key; }

/**
 * @brief This method checks if the parser currently analyzes an implicit
 *        flow key.
 *
 * @retval true If the current input is part of an implicit flow key
 * @retval false If the current input is not part of an implicit flow key
 */
bool State::contextFlowKey() const { return context == Context::flow_key; }

/**
 * @brief This method checks if the parser currently analyzes a value inside
 *        a flow collection.
 *
 * @retval true If the current input is part of a value inside a flow
 *              collection
 * @retval false If the current input is not part of a flow value
 */
bool State::contextFlowIn() const { return context == Context::flow_in; }

/**
 * @brief This method checks if the parser currently analyzes a value outside
 *        a flow collection.
 *
 * @retval true If the current input is part of a value outside a flow
 *              collection
 * @retval false If the current input is not part of a value outside a flow
 *               collection
 */
bool State::contextFlowOut() const { return context == Context::flow_out; }

/**
 * @brief This method checks if the last matched grammar rule was `ns_char`.
 *
 * @retval true If the last matched rule was `ns_char`
 * @retval false If the last matched rule was not `ns_char`
 */
bool State::lastRuleWasNsChar() const { return lastWasNsChar; }

/**
 * @brief This method changes the state updating the boolean value that
 *        specifies if the last matched grammar rule was `ns_char`.
 *
 * @param value This value has to be `true` if the last matched rule was
 *              `ns_char`, or false otherwise.
 */
void State::setLastRuleWasNsChar(bool value) { lastWasNsChar = value; }

/**
 * @brief This method stores the current parent key in the key set.
 *
 * @param text This variable specifies the text that should be used as value
 *             for the stored parent key.
 */
void State::appendKey(string const &text) {
  Key key = parents.top();
  key.setString(scalarToText(text));
  keys.append(key);
}

/**
 * @brief This method returns the converted YAML data.
 *
 * @return A key set representing the parsed YAML data
 */
KeySet State::getKeySet() const { return keys; };

} // namespace yaypeg
