#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#------------------------------------------------------------------------------
# Copyright (c) 2016, 2022 Oracle and/or its affiliates.
#
# This software is dual-licensed to you under the Universal Permissive License
# (UPL) 1.0 as shown at https://oss.oracle.com/licenses/upl and Apache License
# 2.0 as shown at http://www.apache.org/licenses/LICENSE-2.0. You may choose
# either license.
#
# If you elect to accept the software under the Apache License, Version 2.0,
# the following applies:
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#------------------------------------------------------------------------------

# the location of templates that are being used, relative to this directory
templates_path = ['_templates']

# the suffix used for all source files
source_suffix = '.rst'

# the name of the root document
root_doc = master_doc = 'index'

# general information about the project
project = 'ODPI-C'
copyright = '2016, 2022, Oracle and/or its affiliates. All rights reserved.'
author = 'Oracle'

# the version info for the project, acts as replacement for |version| and
# |release|, also used in various other places throughout the built documents
#
# the short X.Y version
version = '4.6'

# the full version, including alpha/beta/rc tags
release = '4.6.0'

# the theme to use for HTML pages
html_theme = 'oracle'

# the location where themes are found, relative to this directory
html_theme_path = ["_themes"]

# the name for this set of documents.
html_title = 'ODPI-C v' + release

# the location for static files (such as style sheets) relative to this
# directory; these are copied after the builtin static files and will overwrite
# them
html_static_path = ['_static']

# the location of the favicon to use for all pages
html_favicon = "_themes/oracle/static/favicon.ico"

# the location of any extra paths that contain custom files (such as robots.txt
# or .htaccess), relative to this directory; these files are copied directdly
# to the root of the documentation
html_extra_path = []

# do not generate an index
html_use_index = False

# do not use "smart" quotes
smart_quotes = False

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title,
#  author, documentclass [howto, manual, or own class]).
latex_documents = [
    (root_doc, 'ODPI-C.tex', 'ODPI-C Documentation', 'Oracle', 'manual'),
]

# default domain is C
primary_domain = "c"

# define setup to prevent the search page from being generated
def setup(app):
    app.connect('builder-inited', on_builder_inited)

# define method to override the HTML builder to prevent the search page from
# being generated
def on_builder_inited(app):
    if app.builder.name == "html":
        app.builder.search = False
        app.builder.script_files.clear()
