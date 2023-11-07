.. _include_10_api:


=================
API Documentation
=================



.. _api cuke_context:

``cuke::context``
=================

.. doxygenfunction:: cuke::context()
  :project: cwt-cucumber

.. doxygenfunction:: cuke::context(Args&&... args)
  :project: cwt-cucumber


.. _api cuke_arg:

``CUKE_ARG``
============

.. doxygendefine:: CUKE_ARG
  :project: cwt-cucumber


.. _api hooks:

Hooks
=====

.. doxygendefine:: BEFORE
  :project: cwt-cucumber

.. doxygendefine:: AFTER
  :project: cwt-cucumber

.. doxygendefine:: BEFORE_STEP
  :project: cwt-cucumber

.. doxygendefine:: AFTER_STEP
  :project: cwt-cucumber


.. _api tagged_hooks:

Tagged Hooks
============

.. doxygendefine:: BEFORE_T
  :project: cwt-cucumber

.. doxygendefine:: AFTER_T
  :project: cwt-cucumber

.. _api asserts:

Asserts
=======

.. doxygenfunction:: equal(const T& lhs, const U& rhs)
  :project: cwt-cucumber

.. doxygenfunction:: not_equal(const T& lhs, const U& rhs)
  :project: cwt-cucumber

.. doxygenfunction:: greater(const T& lhs, const U& rhs)
  :project: cwt-cucumber

.. doxygenfunction:: greater_or_equal(const T& lhs, const U& rhs)
  :project: cwt-cucumber

.. doxygenfunction:: less(const T& lhs, const U& rhs)
  :project: cwt-cucumber

.. doxygenfunction:: less_or_equal(const T& lhs, const U& rhs)
  :project: cwt-cucumber

.. doxygenfunction:: is_true(bool condition)
  :project: cwt-cucumber

.. doxygenfunction:: is_false(bool condition)
  :project: cwt-cucumber