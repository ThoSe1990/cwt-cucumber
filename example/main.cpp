
#include "cwt_cucumber/parser.hpp"

int main() {
    using namespace cwt::cucumber;

    std::string input = 
    "Collection First Collection\n"
    "Item Some arbitrary string\n"
    "Item string after variable\n"
    "Item Another arbitrary string\n"
    "Collection with variables Second Collection\n"
    "Item More arbitrary strings\n"
    "Item Yet another string with <variable_2>\n";


    Lexer lexer;
    auto tokens = lexer.lex(input);

    Parser parser;
    auto ast = parser.parse(tokens);

    int collectionCount = 0;
    for (const auto& collection : ast) {
        std::cout << "Collection " << ++collectionCount << " with name: " << collection.name << std::endl;
        int itemCount = 0;
        for (const auto& item : collection.items) {
            std::cout << "  Item " << ++itemCount << " with value: " << item.value << std::endl;
            for (const auto& variable : item.variables) {
                std::cout << "    - Variable: " << variable.name << std::endl;
            }
        }
    }
    return 0;
}
