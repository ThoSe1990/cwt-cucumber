#include <iostream>
#include <vector>
#include <functional>

// Define a container to hold test functions
std::vector<std::pair<const char*,cwtc_step_t>> test_functions;

#define CONCAT_INTERNAL(a, b) a ## b
#define CONCAT(a, b) CONCAT_INTERNAL(a, b)

// Define a macro to declare test cases and register them
#define INTERNAL_STEP(step_definition, n) \
    void CONCAT(step,n)(); \
    namespace { \
        struct CONCAT(cwt_step,n) { \
            CONCAT(cwt_step,n)() { \
             test_functions.push_back() \
            } \
        } CONCAT(g_cwt_step,n); \
    } \
    void CONCAT(step,n)()

#define cwtc_step(step) INTERNAL_STEP(step, __COUNTER__)





int main() {
    // Run all registered test functions
    for (const auto& test : test_functions) {
        test();
    }

    __FILE__;

    return 0;
}


