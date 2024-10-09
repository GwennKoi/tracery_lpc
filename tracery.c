// tracery.c
// LPC Port of Kate "GalaxyKate" Compton's Tracery javascript library
// This is a grammar replacement library. See https://tracery.io for details
// Kryssa@SWMud.org, October 2024

#include <tracery.h>

// The grammar definition
mapping grammar;

// For actions that push or pop rules dynamically.  Resets on flatten();
mapping temporaryRules;

// Internal node counter. Resets on flatten();
int node_count = 0;

void create(string input_grammar) {
  // initialize variables
  createGrammar(input_grammar);
  node_count = 0;
  temporaryRules = ([]);
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

// Internal parse start for a string.
private string parse_string(string input) {
  return processNode(parseNode(createStartNode(input)));
}

//I'm lazy.
private void debug(string text) {
#ifdef TRACERY_DEBUG
  write(text);
#endif
}

// Parses a node into child nodes
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
    // Parses removing a rule "[myVariable:POP]"
    if (sscanf(action, "%s:POP", action)) {
      map_delete(temporaryRules, action);
    } 
    // Parses adding a rule "[myVariable:someRule]" 
    else if (sscanf(action, "%s:%s", action, rule)) {
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
      return pluralizeFirstWord(text);
    case "ed":
      return pastTense(text);
    case "spaceBefore":
      return addSpaceBefore(text);
    case "spaceAfter":
      return addSpaceAfter(text);
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

// Utility: If the text has length, add a space at the end.
private string addSpaceBefore(string text) {
  if(sizeof(text) > 0) {
    return " "+text;
  }
  return text;
}

// Utility: If the text has length, add a space at the end.
private string addSpaceAfter(string text) {
  if(sizeof(text) > 0) {
    return text+" ";
  }
  return text;
}

// Utility: Split off the first word and pluralize it
private string pluralizeFirstWord(string text) {
  int space; 
  if(!stringp(text) || !sizeof(text)) {
    return text;
  }
  
  space = strsrch(text, " ");
  if(space == -1) {
    return pluralize(text);
  }
  return sprintf("%s%s", pluralize(text[0..space-1]), text[space..]);
}

// Utility: Add an 'ed' to word.
string pastTense(string text) {
  string result;
  if(!stringp(text) || !sizeof(text)) {
    return text;
  }
  result = text[0..<2];
  switch (lower_case(text)[<1]) {
    case 's':
      result += "ed";
      break;
    case 'e':
      result += "d";
      break;
    case 'h':
    result += "ed";
      break;
    case 'x':
      result += "ed";
      break;
    case 'y':
    if (!is_vowel(lower_case(result)[<2])) {
        result = result[0..<2] + "ied";
      }
      else {
        result += "d";
      }
      break;
    default:
    result += "ed";
  }
  return result;
}

// Utility: Create a parent node to start from
private TNODE createStartNode(string input) {
  TNODE node = new(TNODE);
  node.parent = 0;
  node.id = node_count++;
  node.type = TNODE_RAW;
  node.symbol = input || "";
  node.children = ({ });

  return node;
}

// Utility: Child node creation
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
