# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).


## [Unreleased]

### Added 
- Step definitions with ordinal numbers 

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
