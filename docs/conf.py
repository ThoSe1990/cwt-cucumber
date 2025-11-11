# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

import os
import sys
sys.path.insert(0, os.path.abspath(os.path.dirname(__file__)))
from _version import version

project = 'CWT-Cucumber ' + version
copyright = '2025, Thomas'
author = 'Thomas https://www.codingwiththomas.com/'


# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
  'breathe',
]

breathe_projects = {
  "cwt-cucumber": "./_build/doxygen/xml"
}

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']



# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
html_logo = '_static/logo.png'  # shows in the sidebar automatically
html_theme_options = {
    'collapse_navigation': True,
    'sticky_navigation': True,
    'navigation_depth': 3,
    'includehidden': False,
    'titles_only': False
}

# build_all_docs = os.environ.get("build_all_docs")
# pages_root = os.environ.get("pages_root", "")
#
# if build_all_docs is not None:
#   current_version = os.environ.get("current_version")
#
#   html_context = {
#     'current_version' : current_version,
#     'versions' : [],
#   }
