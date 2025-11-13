# Contributing to CWT-Cucumber

🎉 Thanks for your interest in contributing to **CWT-Cucumber**!  
This project aims to bring modern, lightweight, *pure C++* BDD testing to life — without legacy baggage.  
Contributions are always welcome, whether you’re fixing a bug, improving docs, or proposing new features.

---

## 🧭 Guiding Principles

- **Keep it simple** – CWT-Cucumber should remain lightweight, dependency-free, and easy to integrate.  
- **Modern C++** – Use clean, expressive C++20 features where they improve clarity or safety.  
- **Readable over micro improvements** – Clear code and tests matter more than intricate template tricks.  
- **Tests matter** – Keep the safety net alive.

---

## 🛠️ Setting Up the Project

To work on it locally:

1. Clone the repository:
   ```bash
   git clone https://github.com/ThoSe1990/cwt-cucumber.git
   cd cwt-cucumber
   ```

2. Build and run the example tests:
   ```bash
   cmake -B build
   cmake --build build
   ./build/bin/unittest
   ```

3. (Optional) Run the example scenarios manually:
   ```bash
   ./build/bin/examples ./examples/features
   ```

You should see the sample feature output in your terminal.

---

## 🧹 Code Style

- Follow the style already present in the codebase (Allman braces, consistent naming).
- Keep an eye on the `clang-tidy` warnings
- Use `clang-format`

---

## 📝 Documentation & Examples

Improving docs, comments, or adding example scenarios is *just as valuable* as adding code.  
If you fix typos or improve clarity — thank you! That helps users tremendously.

---

## ❤️ Community

If you use CWT-Cucumber in production and want to share your experience,  
open a “Showcase” discussion or add a note to the **Who’s Using It** section in the README.  
Hearing how teams use it helps guide development priorities.

---

## 🧠 License

By contributing, you agree that your code will be licensed under the same license as CWT-Cucumber (MIT).

---

Thanks for helping make CWT-Cucumber the most natural way to do BDD in C++ 🚀
