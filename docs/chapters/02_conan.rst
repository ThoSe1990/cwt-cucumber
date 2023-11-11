.. _include_02_conan:

======================
Conan Recipe / Package
======================

At the moment, the Conan recipe is not pushed to the conancenter. For now, I maintain the recipe in `this GitHub repository <https://github.com/ThoSe1990/cwt-cucumber-conan>`_, to build the package locally and to use cwt-cucumber. In the long run, I'll consider moving the recipe to conancenter.

CMake Integration
=================

When you created the package, you can integrate this into your projects fairly easy. There is not too much to do: 

- Find the cwt-cucumber package: `find_package(cwt-cucumber REQUIRED)`
- Compile your sources / steps
- Link against `cwt::cucumber`, `cwt::cucumber-no-main` or `cwt::cucumber-c`

Build the Conan Package & Examples
==================================

You can find all examples in this repository with their ``CMakelists.txt``. You can use this as a general reference. If anything is unclear, please let me know. 

But first we hvae to create the package. Move to ``./package`` and create the conan package:

.. code-block:: sh
  
  cd package
  conan create . --version 1.0.0 --user cwt --channel stable

.. note::
  You can use another user and channel, feel free to do so. But remember to use them correctly in the consumer's ``conanfile.txt``

And now you can move over to ``./consumer`` to build the examples: 

.. code-block:: sh

  cd ../consumer
  conan install . -of ./build 
  cmake -S . -B ./build -DCMAKE_TOOLCHAIN_FILE=./build/conan_toolchain.cmake 
  cmake --build ./build

Run the Examples 
================


There are three executables in the build directory: 
- ``./build/bin/box`` The C++ implementation
- ``./build/bin/box-no-main``` The C++ implementation with its own main
- ``./build/bin/box-c`` The C implementation

Navigate (when not already in this directory):

.. code-block:: sh

  cd ./consumer

And run all features from the ``feature`` directory:

.. code-block:: sh

  ./build/bin/box ./features

Or with an own / custom  ``main``

.. code-block:: sh

  ./build/bin/box-no-main ./features


The C implementation supports all features, but you can not run all feature files from a directory, you have to run them explicitly. For instance:

.. code-block:: sh

  ./build/bin/box-c ./features/first_example.feature ./features/box.feature ./features/scenario_outline.feature

