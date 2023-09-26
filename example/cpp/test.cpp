#include <iostream>
#include <vector>
#include <functional>

// Define a container to hold test functions
std::vector<std::pair<const char*,cwtc_step>> test_functions;

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

cwtc_step("hello world")
{
  
}



#define HI(step_definition, n) \
    void CONCAT(step,n)(); \

HI("asdf", __COUNTER__)
// Define some test cases using the TEST macro
// TEST(TestCase1) {
//     // Your test code here
//     std::cout << "Test case 1 executed." << std::endl;
// }

// TEST(TestCase2) {
//     // Your test code here
//     std::cout << "Test case 2 executed." << std::endl;
// }

void TestCase3(); 
namespace 
{ 
  struct TestRegisterer_TestCase3 
  { 
    TestRegisterer_TestCase3() 
    { 
      test_functions.push_back(TestCase3); 
    } 
  } 
  g_test_registerer_TestCase3; 
} 
void TestCase3()
{

}

int main() {
    // Run all registered test functions
    for (const auto& test : test_functions) {
        test();
    }

    __FILE__;

    return 0;
}


