Installation
============

To get started, build the project with **CMake**:

.. code-block:: sh

  cmake -S . -B build
  cmake --build build -j12

This will build two libraries: ``cucumber`` and ``cucumber-no-main``.

- ``cucumber`` includes a built-in ``main()`` function, so you only need to implement your step definitions.
- ``cucumber-no-main`` lets you provide your own entry point.  
  See :ref:`subch-installation-using-your-own-main` for details.

If ``nlohmann_json`` is available, CMake will automatically enable JSON report generation.
CMake simply runs ``find_package(nlohmann_json)`` and checks ``if (nlohmann_json_FOUND)``,  
so it does not matter how the package is provided.

Conan
-----

CWT-Cucumber is also available on `ConanCenter <https://conan.io/center/recipes/cwt-cucumber>`_.  
If you are using Conan for dependency management, you can add it to your ``conanfile.txt``:

.. code-block:: ini

  [requires]
  cwt-cucumber/2.7

  [generators]
  CMakeToolchain
  CMakeDeps


.. _subch-installation-using-your-own-main:

Using your own ``main.cpp``
---------------------------

If you prefer to define your own ``main()``, there are two ways to run the tests:

**Option 1:** Use ``entry_point()``
This behaves like the built-in main and runs all feature tests automatically.

.. code-block:: cpp

  int main(int argc, const char* argv[])
  {
    auto result = cuke::entry_point(argc, argv);
    return result == cuke::results::test_status::passed
             ? EXIT_SUCCESS
             : EXIT_FAILURE;
  }

**Option 2:** Use ``cwt_cucumber`` directly
This gives you full control over execution, output, and result handling.

.. code-block:: cpp

  cwt_cucumber cucumber(argc, argv);
  cucumber.run_tests();
  cucumber.print_results();
  return cucumber.final_result();

