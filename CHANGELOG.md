# Changelog

## [Unreleased]

## [2.4] 2024-11-14

### Added 
- `cwt_cucumber` object for embedding CWT Cucumber and executing tests ([49](https://github.com/ThoSe1990/cwt-cucumber/pull/49))
- `--steps-catalog [optional: file]` to print the implemented steps as readable text to stdout or a given file (this does not write to '.feature' files) ([50](https://github.com/ThoSe1990/cwt-cucumber/pull/50))
- `--steps-catalog-json [optional: file]` to print the implemented steps as json to stdout or a given file (this does not write to '.feature' files) ([50](https://github.com/ThoSe1990/cwt-cucumber/pull/50)) ([54](https://github.com/ThoSe1990/cwt-cucumber/pull/54))
- Custom Parameter Types ([56](https://github.com/ThoSe1990/cwt-cucumber/pull/56))
  - Define custom types with `CUSTOM_PARAMETER(function-name, "{your type}", "regex pattern", "description") { your callback implementation }` 
  - In your callback implement how to consume capture groups
  - Access capture groups with `CUKE_PARAM_ARG(index)`, where index starts at 1 and goes from left to write
- Renamed example target `box` to `example` ([56](https://github.com/ThoSe1990/cwt-cucumber/pull/56))

## [2.3.1] 2024-10-23

### Added 
- Alternative words in steps ([45](https://github.com/ThoSe1990/cwt-cucumber/pull/45))
  - Backslash to toggle between two words (e.g. `is/are` allows `is` and `are` in steps) 
  - Parenthesis to make words or characters optional (e.g. `item(s)` will allow `item` and `items` in steps)


## [2.3.0] 2024-10-23

### Added
- New `CWT_CUCUMBER_STEP_DELAY` environment variable to delay step execution ([32](https://github.com/ThoSe1990/cwt-cucumber/pull/32))
- Expressions `{word}` and `{}` (anonymous) implemented. Note: both are treated as `std::string` ([36](https://github.com/ThoSe1990/cwt-cucumber/pull/36))
- Skip function: `cuke::skip_scenario`. Call This function in the before hook in order to skip a scenario ([37](https://github.com/ThoSe1990/cwt-cucumber/pull/37))

### Fixed
- Tags aren't ineherited correctly, when using tag expression ([35](https://github.com/ThoSe1990/cwt-cucumber/pull/35))
- Integer and floating point types overwrite `{word}` and `{}` in examples table ([40](https://github.com/ThoSe1990/cwt-cucumber/pull/40))

### Changed 
- cuke::value::to_string uses now std::format instead of std::to_string ([42](https://github.com/ThoSe1990/cwt-cucumber/pull/42))
- Unquotet strings are onlye for anonymous `{}` possible. Every `{string}` value has to use quotes ([42](https://github.com/ThoSe1990/cwt-cucumber/pull/42))
- Type missmatches (e.g. no quotes on `{string}`) lead to an undefined step ([42](https://github.com/ThoSe1990/cwt-cucumber/pull/42))
- Stdout print in `Scenario Outline`: Values are within the step. Table prefix removed ([42](https://github.com/ThoSe1990/cwt-cucumber/pull/42))
- Internals: ([42](https://github.com/ThoSe1990/cwt-cucumber/pull/42))
  - Value representation is now a string: the interface in `cuke::value` is replaced with a simple string representation.
  - Step matching: Introduced regex. Each step is converted into a regex pattern
  - Scenario Outline: All steps do a search and replace for their variables. 


## [2.2.0] 2024-10-16

### Added 
- CMake packaging ([19](https://github.com/ThoSe1990/cwt-cucumber/pull/19))
- Optional custom error messages for step asserts ([28](https://github.com/ThoSe1990/cwt-cucumber/pull/28))

### Fixed 
- Before all hook call at the end of test run ([26](https://github.com/ThoSe1990/cwt-cucumber/pull/26)) 

## [2.1.0] 2024-10-16

### Added 
- Step definitions with ordinal numbers ([23](https://github.com/ThoSe1990/cwt-cucumber/pull/23))
- Added hooks `BEFORE_ALL`, `AFTER_ALL`, executed once before and after the entire test run ([24](https://github.com/ThoSe1990/cwt-cucumber/pull/24))

## [2.0.0] 2024-08-19

- Reimplementan of the parser to provide the abstract syntax tree 
- cwt-cucumber traverses now through the AST to execute all tests 

## [1.2.2] 2024-03-12

### Added
- Run and compile time are measured and printed to final results 
- Command line option `-h` and `--help` print a help screen (if needed in own main use `cuke::internal::print_help(int argc, const char* argv[])`)

## [1.2.1] 2024-02-26

### Changed
- Empty `Feature` does not result in a compilation error 

### Fixed
- Tags are inherited incorrectly

## [1.2.0] 2024-02-22

### Added
- Doc Strings: Backticks (`) as delimiter
- Macro for doc string access `CUKE_DOC_STRING()`
- Support for Datables and `cuke::table` for  
  - Raw access
  - Rows hash access
  - Key/Value pairs or hash pairs
- Macro to create `cuke::table` in step definition `CUKE_TABLE()`
- `cuke::value` added `to_string()` member function
- Quotes for string values in tables aren't required anymore

### Fixed
- Detect too many / too less parenthesis in tag expression 

## [1.1.0] 2024-02-12

### Added
- Cucumber Localization. Languages implemented: 
  - English (default)
  - German
  - Spanish

### Removed
- No C API anymore (project is re-implmented in C++20)
- Moved docs to it's own repository

## [1.0.0] 2023-11-10

- Initial release
