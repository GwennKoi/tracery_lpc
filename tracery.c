// tracery.c
// LPC Port of Kate "GalaxyKate" Compton's Tracery javascript library
// This is a grammar replacement library. See https://tracery.io for details
// Kryssa@SWMud.org, October 2024


// I'm going to split these definitions out into their own .h file.
// for... reasons?
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
//End of .h file

// The grammar definition
mapping grammar;

// For actions that push or pop rules dynamically.  Resets on flatten();
mapping temporaryRules;
// Internal node counter. Resets on flatten();
int node_count = 0;

// Adding in this test grammar just to beta it. Will be removed in the final.
mapping test_grammar = ([
  "origin": ({
    "#sentence_start#, [badman:#bad_official.capitalizeAll.a#]#badman# was "+
      "#action#. Little did anyone know, #corruption# #consequence_good#. "+
      "#badman.capitalize# learned that crime pays."
    ,"#sentence_start#, #consequence_bad# "+
      "[badman:#bad_official.capitalizeAll.a#]#badman# #action#. No more "+
      "#fruit.s# for #badman#."
  }),
  "fruit": ({"apple", "banana", "coconut"}),
  "sentence_start": ({"#setting_start#","#time_start#"}),
  "setting_start": ({
    "In the heart of the city",
    "Behind closed doors at the government office",
    "In the shadowy halls of the ministry",
    "Within the walls of the bureaucracy"
  }),
  "time_start": ({
    "Seemingly burning the midnight oil",
    "By the dark of night",
    "On the weekend, free from prying eyes",
    "In broad daylight",
    "During a leisurely lunch",
    "While enjoying another state dinner",
    "At second breakfast"
  }),
  "bad_adjective": ({
    "corrupt",
    "slimy",
    "sly",
    "crooked",
    "cunning",
    "shrewd",
    "craven",
    "money-hungry"
  }),
  "bad_official": ({
    "#bad_adjective# #bureaucrat#"
  }),
  "bureaucrat": ({
    "official",
    "minister",
    "representative",
    "bureaucrat",
    "administrator",
    "councillor",
    "moff",
    "governor",
    "senator",
    "ambassador",
    "diplomat",
    "clerk"
  }),
  "action": ({
    "signing another suspicious contract",
    "approving funds for a nonexistent project",
    "meeting with a shady lobbyist",
    "stashing away bribes in offshore accounts",
    "diverting public funds for personal gain",
    "making backroom deals with powerful elites"
  }),
  "corruption": ({
    "the plan was falling apart",
    "the media was closing in",
    "the public was becoming aware",
    "the law was beginning to catch up",
    "a whistleblower was gathering evidence",
    "rumors of their dealings were spreading"
  }),
  "consequence_good": ({
    "but they covered their tracks expertly",
    "but they managed to bribe the investigators",
    "but a scapegoat was already in place",
    "but no one dared to confront them",
    "yet the system continued to protect them",
    "though their power only grew stronger"
  }),
  "consequence_bad": ({
    "lady justice peeled back her blindfold to catch",
    "#cops# finally discovered",
    "#cops# arrested",
    "#cops# were able to corroborate stories about",
  }),
  "cops": ({
    "investigators",
    "inquisitors",
    "authorities",
    "police",
    "the constabulary",
  })
]);

//I'm lazy.
void debug(string text) {
#ifdef TRACERY_DEBUG
  write(text);
#endif
}

void create(string input_grammar) {
  // initialize variables
  createGrammar(input_grammar);
  node_count = 0;
  temporaryRules = ([]);
}

void test() {
  createGrammar(test_grammar);
  write("Test: " + flatten("#origin#"));
}

// Constructor to create grammar
void createGrammar(mapping inputGrammar) {
    grammar = inputGrammar || ([]);
}

// Flatten method to process a replacement string
// This is the public entry point.
// DO NOT USE INTERNALLY IN THIS LIBRARY
string flatten(string input) {
  node_count = 0;
  temporaryRules = ([]);
  return parse_string(input);
}

private string parse_string(string input) {
  return processNode(parseNode(createStartNode(input)));
}


private TNODE createStartNode(string input) {
  TNODE node = new(TNODE);
  node.parent = 0;
  node.id = node_count++;
  node.type = TNODE_RAW;
  node.symbol = input || "";
  node.children = ({ });

  return node;
}

private TNODE createNode(TNODE parent, int start, int end, int type) {
  TNODE node = new(TNODE);
  if(parent) {
    if(!sizeof(parent.children)) {
      parent.children = ({ });
    }
    parent.children += ({ node });
  }
  
  node.parent = parent;
  node.id = node_count++;
  node.type = type;
  if(parent) {
    debug(identify(parent.symbol));
    node.symbol = (parent) ? (string)(parent.symbol)[start..end] : "";
  }
  node.children = ({ });
  
  debug(sprintf(
    "Node %d: Type: %d Symbol: %s\n",
    node.id, node.type, node.symbol
  ));
  return node;
}

// Parses a string input into nodes
private TNODE *parseNode(TNODE node) {
  // Logic for parsing the input string and creating nodes
  // Identify #tags# and [actions] and convert them to nodes with
  // appropriate types
  int escaped = 0;
  int depth = 0;
  int inTag = 0;
  int start = 0;
  int lastEscapedChar = 0;
  string escapedSubstring = "";

  // Parse character by character
  for (int i = 0; i < sizeof(node.symbol); i++) {

    // Skip this spot if escaped.
    if (escaped) {
      escaped = 0;
    } 
    // Otherwise...
    else {
      int c = node.symbol[i];

      switch(c) {

      // Enter an action section
      case '[':
        if (depth == 0 && !inTag) {
          if (start < i)
            createNode(node, start, i-1, TNODE_TEXT);
          start = i + 1;
        }
        depth++;
        break;

      case ']':
        depth--;

        // End a bracketed section
        if (depth == 0 && !inTag) {
          createNode(node, start, i-1, TNODE_ACTION);
          start = i + 1;
        }
        break;

      // Hashtag -- Not really.
      // Ignore if not at depth 0, that means we are in a bracket
      case '#':
        if (depth == 0) {
          if (inTag) {
            createNode(node, start, i-1, TNODE_SYMBOL);
            start = i + 1;
          } else {
            if (start < i)
              createNode(node, start, i-1, TNODE_TEXT);
            start = i + 1;
          }
          inTag = inTag^1;
        }
        break;

      case '\\':
        escaped = 1;
        escapedSubstring = escapedSubstring + (string)(node.symbol)[start..i];
        start = i + 1;
        lastEscapedChar = i;
        break;
      }
    }
  }
  if (start < sizeof(node.symbol)) {
    debug("End of line.");
    createNode(node, start, sizeof(node.symbol), TNODE_TEXT);
  }
  if (inTag) {
    debug("Unclosed tag");
  }
  if (depth > 0) {
    debug("Too many [");
  }
  if (depth < 0) {
    debug("Too many ]");
  }
  return node.children;
}

// Recursive function to process a node
private string processNode(TNODE *nodes) {
  string result = "";
  string fragment = "";
  foreach(TNODE node in nodes) {
    // Handle the node type
    switch(node->type) {
      case TNODE_RAW:
        // Raw node: needs parsing and further processing
        fragment = processNode(parseNode(node));
        break;
      case TNODE_TEXT:
        // Plaintext node: just return the symbol
        fragment = node->symbol;
        break;
      case TNODE_SYMBOL:
        // Tag node: replace the symbol using the grammar
        fragment = handleTagNode(node);
        break;
      case TNODE_ACTION:
        // Action node: process the action and return results
        fragment = handleActionNode(node);
        break;
    }

    // Handle modifiers on the result
    if (sizeof(node->modifiers)) {
      foreach (string modifier in node->modifiers) {
        fragment = applyModifier(fragment, modifier);
      }
    }
    result += fragment;
  }
  return result;
}

// Handle a tag node by replacing its symbol using grammar or temporary rules
private string handleTagNode(TNODE node) {
    string key = node.symbol;
    mixed values;
    string replacement;
    string *extensions;

    extensions = explode(key, ".");
    if(sizeof(extensions) > 1) {
      node.modifiers = extensions[1..];
      node.symbol = extensions[0];
      key = node.symbol;
    }

    // First check for temporary rules
    if (temporaryRules[key]) {
      values = temporaryRules[key];
    } else if (grammar[key]) {
      values = grammar[key];
    } else {
      // return key as fallback if no rule exists
      return key;
    }

    // Choose a random replacement from the list
    if(arrayp(values)) {
      replacement = element_of(values);
    } else {
      replacement = values;
    }

    // Process the replacement
    // (it may be another symbol that needs further expansion)
    return parse_string(replacement);
}

// Handle action nodes such as pushing new rules or popping existing ones
private string handleActionNode(TNODE node) {
    string action = node->symbol;
    string rule;
    // Parse action: either "[myVariable:someRule]" or "[myVariable:POP]"
    if (sscanf(action, "%s:POP", action)) {
        map_delete(temporaryRules, action);
    } else if (sscanf(action, "%s:%s", action, rule)) {
        temporaryRules[action] = parse_string(rule);
    }

    // No actual string output for actions, just modify rules
    return "";
}

// Apply modifiers to a string, such as 'a' or 'capitalize'
//TODO: Implement the other modifiers
private string applyModifier(string text, string modifier) {
  switch(modifier) {
    case "a":
      return article(text);
    case "capitalize":
      return capitalize(text);
    case "capitalizeAll":
      return capitalize_all(text);
    case "s":
      return pluralize(text);
    case "firstS":
    case "ed":
    case "replace":
    default:
      // fallback: do nothing
      return text;
  }
}

// Utility: return "a" or "an" based on input word
private string article(string text) {
    string *vowels = explode("aeiouAEIOU","");
    if (member_array(text[0..0], vowels) != -1) {
      return "an " + text;
    } else {
      return "a " + text;
    }
}