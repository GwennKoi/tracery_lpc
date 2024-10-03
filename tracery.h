// tracery.h
// LPC Port of Kate "GalaxyKate" Compton's Tracery javascript library
// This is a grammar replacement library. See https://tracery.io for details
// Kryssa@SWMud.org, October 2024

#ifndef _TRACERY_H
#define _TRACERY_H

//To turn debugging on and off.
//#define TRACERY_DEBUG 1
#define TNODE        class tracery_node
#define TNODE_RAW    (-1)
#define TNODE_TEXT    (0)
#define TNODE_SYMBOL  (1)
#define TNODE_ACTION  (2)

// A class to represent a node in the grammar tree
TNODE {
  // Individual id of a node for tracking and seeing purposes
  int id;
  // Node type:
  // TNODE_RAW - Unprocessed string
  // TNODE_TEXT - Plaintext
  // TNODE_SYMBOL - A piece of grammar that needs replaced
  // TNODE_ACTION - Adding or removing a temporary grammar rule.
  int type;
  // Parent node
  TNODE parent;
  // Symbol string (e.g., "#animal#", or "bird")
  string symbol;
  // Modifiers (e.g., ["a", "s", "capitalize"])
  string  *modifiers;
  // Children based on the current symbol
  TNODE *children;
}

#endif
