# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

import os
import re
import yaml

cmake_file_path = "./../CMakeLists.txt"
pattern = r'project\(cwt-cucumber VERSION (\d+\.\d+\.\d+)\)'
with open(cmake_file_path, 'r') as file:
    content = file.read()
match = re.search(pattern, content)

version = match.group(1) if match else "0.0.0"
release = version



project = 'CWT-Cucumber'
copyright = '2023, Thomas Sedlmair'
author = 'Thomas Sedlmair'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = []

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

extensions = [
  'breathe'
]
breathe_projects = {
  "cwt-cucumber": "./_build/doxygen/xml"
}

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
html_show_sourcelink = False
html_theme_options = {
    'display_version': True,
}



