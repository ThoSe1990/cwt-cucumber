
# Copilot Instructions

When generating code, suggesting changes, or reviewing pull requests, follow the conventions described in `.github/STYLE_CONSISTENCY.md`.

Review priorities:

1. Architectural consistency
2. Reuse of existing abstractions
3. Naming consistency
4. API consistency
5. Ownership and lifetime consistency
6. Error handling consistency

Do not suggest changes solely related to:

* Formatting
* Whitespace
* Include ordering
* Brace placement
* Issues already enforced by clang-format
* Issues already enforced by clang-tidy

When reviewing code:

* Prefer existing repository patterns over introducing new patterns.
* Look for existing implementations before suggesting new abstractions.
* Flag architectural boundary violations.
* Flag inconsistent domain terminology.
* Explain why a suggestion aligns with existing repository conventions.

When unsure, prefer consistency with neighboring code over general C++ best practices.
