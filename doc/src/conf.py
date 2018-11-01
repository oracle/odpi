#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#------------------------------------------------------------------------------
# Copyright (c) 2016, 2018 Oracle and/or its affiliates.  All rights reserved.
# This program is free software: you can modify it and/or redistribute it
# under the terms of:
#
# (i)  the Universal Permissive License v 1.0 or at your option, any
#      later version (http://oss.oracle.com/licenses/upl); and/or
#
# (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
#------------------------------------------------------------------------------

# the location of templates that are being used, relative to this directory
templates_path = ['_templates']

# the suffix used for all source files
source_suffix = '.rst'

# the name of the master document
master_doc = 'index'

# general information about the project
project = 'ODPI-C'
copyright = '2016, 2018, Oracle and/or its affiliates. All rights reserved.'
author = 'Oracle'

# the version info for the project, acts as replacement for |version| and
# |release|, also used in various other places throughout the built documents
#
# the short X.Y version
version = '3.1'

# the full version, including alpha/beta/rc tags
release = '3.1.0-dev'

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
    (master_doc, 'ODPI-C.tex', 'ODPI-C Documentation', 'Oracle', 'manual'),
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

