/**
 * @file
 *
 * @brief This file contains the implementation of a basic listener.
 *
 * @copyright BSD License (see LICENSE.md or https://www.libelektra.org)
 */

// -- Imports ------------------------------------------------------------------

#include "listener.hpp"

using std::string;

using kdb::Key;

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

/**
 * @brief This constructor creates a Listener using the given parent key.
 *
 * @param parent This argument specifies the parent key of the key set this
 *               listener produces.
 */
Listener::Listener(Key const &parent) { parents.push(parent); }

/**
 * @brief This function will be called after the walker exits a value node.
 *
 * @param text This variable contains the text stored in the value.
 */
void Listener::exitValue(string const &text) {
  Key key = parents.top();
  key.setString(scalarToText(text));
  keys.append(key);
}

/**
 * @brief This function will be called after the walker exits a key node.
 *
 * @param text This variable contains the text of the key.
 */
void Listener::exitKey(string const &text) {
  // Entering a mapping such as `part: …` means that we need to add `part` to
  // the key name
  Key child{parents.top().getName(), KEY_END};
  child.addBaseName(scalarToText(text));
  parents.push(child);
}

/**
 * @brief This function will be called after the walker exits the node for a
 *        key-value pair.
 */
void Listener::exitPair() {
  // Returning from a mapping such as `part: …` means that we need need to
  // remove the key for `part` from the stack.
  parents.pop();
}

/**
 * @brief This method returns the key set of the listener.
 *
 * @return A key set created by the walker by calling methods of this class
 **/
kdb::KeySet Listener::getKeySet() const { return keys; }
