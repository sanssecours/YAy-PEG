/**
 * @file
 *
 * @brief This file contains the implementation of a basic listener.
 *
 * @copyright BSD License (see LICENSE.md or https://www.libelektra.org)
 */

// -- Imports ------------------------------------------------------------------

#include "listener.hpp"

using kdb::Key;

// -- Class --------------------------------------------------------------------

/**
 * @brief This constructor creates a Listener using the given parent key.
 *
 * @param parent This argument specifies the parent key of the key set this
 *               listener produces.
 */
Listener::Listener(Key const &parent) { parents.push(parent); }

/**
 * @brief This method returns the key set of the listener.
 *
 * @return A key set created by the walker by calling methods of this class
 **/
kdb::KeySet Listener::getKeySet() const { return keys; }
