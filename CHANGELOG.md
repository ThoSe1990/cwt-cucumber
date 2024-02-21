# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).


## [Unreleased]

## [1.1.0] 2024-02-21

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
