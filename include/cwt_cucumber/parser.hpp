#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <sstream>

namespace cwt::cucumber
{   
    
enum TokenType {
    COLLECTION, COLLECTION_WITH_VARIABLES, ITEM, STRING, VARIABLE, END
};

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
public:
    std::vector<Token> lex(const std::string& input) {
        std::vector<Token> tokens;
        std::istringstream input_stream(input);
        std::string line;
        while (std::getline(input_stream, line)) {
            if (line.find("Collection with variables") != std::string::npos) {
                tokens.push_back({COLLECTION_WITH_VARIABLES, ""});
                tokens.push_back({STRING, line.substr(26)});  // Skip "Collection with variables" to get the collection name
            } else if (line.find("Collection") != std::string::npos) {
                tokens.push_back({COLLECTION, ""});
                tokens.push_back({STRING, line.substr(11)});  // Skip "Collection" to get the collection name
            } else if (line.find("Item") != std::string::npos) {
                tokens.push_back({ITEM, ""});
                std::string item_line = line.substr(5); // Skip "Item" to get the rest of the line
                tokens.push_back({STRING, item_line});
                std::string::size_type variable_pos = item_line.find('<');
                if (variable_pos != std::string::npos) {
                    std::string::size_type variable_end_pos = item_line.find('>', variable_pos);
                    std::string variable = item_line.substr(variable_pos + 1, variable_end_pos - variable_pos - 1);
                    tokens.push_back({VARIABLE, variable});
                    // item_line = item_line.substr(0, variable_pos) + item_line.substr(variable_end_pos + 1);
                }
                // tokens.push_back({STRING, item_line});
            }
        }
        tokens.push_back({END, ""});  // Sentinel to indicate the end of the input
        return tokens;
    }
};

struct VariableNode {
    std::string name;
};

struct ItemNode {
    std::string value;
    std::vector<VariableNode> variables;
};

struct CollectionNode {
    std::string name;
    std::vector<ItemNode> items;
};


class Parser {
public:
    std::vector<CollectionNode> parse(const std::vector<Token>& tokens) {
        std::vector<CollectionNode> ast;

        size_t i = 0;
        while(i < tokens.size()) {
            if(tokens[i].type == COLLECTION || tokens[i].type == COLLECTION_WITH_VARIABLES) {
                bool acceptVariable = (tokens[i].type == COLLECTION_WITH_VARIABLES);

                CollectionNode collection;
                collection.name = tokens[++i].value;
                ++i; // Skip to the next token after the collection name

                while(i < tokens.size() && tokens[i].type != COLLECTION && tokens[i].type != COLLECTION_WITH_VARIABLES && tokens[i].type != END) {
                    if(tokens[i].type == ITEM) {
                        ItemNode item;
                        item.value.clear();
                        
                        // Capture everything after ITEM as the item value until we hit the next ITEM, COLLECTION, COLLECTION_WITH_VARIABLES or END
                        while(++i < tokens.size() && tokens[i].type != ITEM && tokens[i].type != COLLECTION && tokens[i].type != COLLECTION_WITH_VARIABLES && tokens[i].type != END) {
                            if(tokens[i].type == VARIABLE && acceptVariable) {
                                item.variables.push_back({tokens[i].value});
                            }
                            if (tokens[i].type != VARIABLE)
                                item.value += tokens[i].value + " ";
                        }

                        // remove trailing whitespace
                        item.value.erase(item.value.find_last_not_of(" \n\r\t")+1);

                        collection.items.push_back(item);
                    } else {
                        ++i; // Ensure progress in the loop if the current token is not an ITEM
                    }
                }

                ast.push_back(collection);
            } else {
                ++i; // Ensure progress in the loop if the current token is not a COLLECTION
            }
        }

        return ast;
    }
};

} // namespace cucumber
