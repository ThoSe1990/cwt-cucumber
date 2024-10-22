# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).


## [Unreleased]

### Added
- New `CWT_CUCUMBER_STEP_DELAY` environment variable to delay step execution ([32](https://github.com/ThoSe1990/cwt-cucumber/pull/32))
- Expressions `{word}` and `{}` (anonymous) implemented. Note: both are treated as `std::string` ([36](https://github.com/ThoSe1990/cwt-cucumber/pull/36))
- Skip function: `cuke::skip_scenario`. Call This function in the before hook in order to skip a scenario ([37](https://github.com/ThoSe1990/cwt-cucumber/pull/37))

### Fixed
- Tags aren't ineherited correctly, when using tag expression ([35](https://github.com/ThoSe1990/cwt-cucumber/pull/35))
- Integer and floating point types overwrite `{word}` and `{}` in examples table ([40](https://github.com/ThoSe1990/cwt-cucumber/pull/40))

### Changed 
- cuke::value::to_string uses now std::format instead of std::to_string ([42](https://github.com/ThoSe1990/cwt-cucumber/pull/42))
- Internals: ([42](https://github.com/ThoSe1990/cwt-cucumber/pull/42))
  - Value representation is now a string: the interface in `cuke::value` is replaced with a simple string representation.
  - Step matching: Introduced regex. Each step is converted into a regex pattern
  - Scenario Outline: All steps do a search and replace for their variables. 
- Unquotet strings are onlye for anonymous `{}` possible. Every `{string}` value has to use quotes ([42](https://github.com/ThoSe1990/cwt-cucumber/pull/42))
- Type missmatches lead to an undefined step ([42](https://github.com/ThoSe1990/cwt-cucumber/pull/42))


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
