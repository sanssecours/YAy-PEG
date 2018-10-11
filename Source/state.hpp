/**
 * @file
 *
 * @brief This file contains the declaration of a class used by the PEGTL
 *        parser to create a key set.
 *
 * @copyright BSD License (see LICENSE.md or https://www.libelektra.org)
 */

#ifndef ELEKTRA_PLUGIN_YAYPEG_STATE_HPP
#define ELEKTRA_PLUGIN_YAYPEG_STATE_HPP

// -- Imports ------------------------------------------------------------------

#include <stack>

#include <kdb.hpp>

// -- Class --------------------------------------------------------------------

namespace yaypeg {

/** The parser uses this class to convert YAML data to a key set. */
class State {

  /** This enum specifies the context of the YAML data that the parser currently
      analyzes. */
  enum Context {
    /** The current input is part of a block sequence. */
    block_in,
    /** The current input is outside of a block sequence. */
    block_out,
    /** The current input is part of a implicit block key. */
    block_key,
    /** The current input is part of a implicit flow key. */
    flow_key,
    /** The current input is part of a value inside a flow collection. */
    flow_in,
    /** The current input is part of a value outside a flow collection. */
    flow_out
  };

  /** This key set stores the converted YAML data. */
  kdb::KeySet keys;

  /** This stack stores a key for each level of the current key name. */
  std::stack<kdb::Key> parents;

  /** This variable stores the context of the currently parsed YAML data. */
  Context context{block_in};

  /**
   * This variable specifies if the last character matched the grammar rule
   * `ns_char`.
   */
  bool lastWasNsChar;

public:
  /**
   * @brief This constructor creates a State using the given parent key.
   *
   * @param parent This argument specifies the parent key of the key set that
   *               this class stores.
   */
  State(kdb::Key const &parent);

  /**
   * @brief This method checks if the parser currently analyzes an implicit
   *        block key.
   *
   * @retval true If the current input is part of an implicit block key
   * @retval false If the current input is not part of an implicit block key
   */
  bool contextBlockKey() const;

  /**
   * @brief This method checks if the parser currently analyzes an implicit
   *        flow key.
   *
   * @retval true If the current input is part of an implicit flow key
   * @retval false If the current input is not part of an implicit flow key
   */
  bool contextFlowKey() const;

  /**
   * @brief This method checks if the parser currently analyzes a value inside
   *        a flow collection.
   *
   * @retval true If the current input is part of a value inside a flow
   *              collection
   * @retval false If the current input is not part of a flow value
   */
  bool contextFlowIn() const;

  /**
   * @brief This method checks if the parser currently analyzes a value outside
   *        a flow collection.
   *
   * @retval true If the current input is part of a value outside a flow
   *              collection
   * @retval false If the current input is not part of a value outside a flow
   *               collection
   */
  bool contextFlowOut() const;

  /**
   * @brief This method checks if the last matched grammar rule was `ns_char`.
   *
   * @retval true If the last matched rule was `ns_char`
   * @retval false If the last matched rule was not `ns_char`
   */
  bool lastRuleWasNsChar() const;

  /**
   * @brief This method changes the state updating the boolean value that
   *        specifies if the last matched grammar rule was `ns_char`.
   *
   * @param value This value has to be `true` if the last matched rule was
   *              `ns_char`, or false otherwise.
   */
  void setLastRuleWasNsChar(bool value);

  /**
   * @brief This method stores the current parent key in the key set.
   *
   * @param text This variable specifies the text that should be used as value
   *             for the stored parent key.
   */
  void appendKey(std::string const &text);

  /**
   * @brief This method returns the converted YAML data.
   *
   * @return A key set representing the parsed YAML data
   */
  kdb::KeySet getKeySet() const;
};

} // namespace yaypeg

#endif // ELEKTRA_PLUGIN_YAYPEG_STATE_HPP
