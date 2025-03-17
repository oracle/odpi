#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#------------------------------------------------------------------------------
# Copyright (c) 2016, 2025, Oracle and/or its affiliates.
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

import os
import sys

# If your extensions are in another directory, add it here.
sys.path.append(os.path.abspath("_ext"))

# Add any Sphinx extension module names here, as strings. They can be extensions
# coming with Sphinx (named 'sphinx.ext.*') or your custom ones.
extensions = ["table_with_summary", "parameters_table", 'sphinx_rtd_theme']

# the location of templates that are being used, relative to this directory
templates_path = ['.templates']

# the suffix used for all source files
source_suffix = '.rst'

# the name of the root document
root_doc = master_doc = 'index'

# general information about the project
project = 'ODPI-C'
copyright = '2016, 2025, Oracle and/or its affiliates. All rights reserved.'
author = 'Oracle'

# the version info for the project, acts as replacement for |version| and
# |release|, also used in various other places throughout the built documents
#
# the short X.Y version
version = '5.5'

# the full version, including alpha/beta/rc tags
release = '5.5.1'

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = 'sphinx'

# Options for HTML output
# -----------------------

# The style sheet to use for HTML and HTML Help pages. A file of that name
# must exist either in Sphinx' static/ path, or in one of the custom paths
# given in html_static_path.
# html_style = 'default.css'

# The theme to use for readthedocs.
html_theme = 'sphinx_rtd_theme'

# the name for this set of documents.
html_title = 'ODPI-C v' + release

# the location for static files (such as style sheets) relative to this
# directory; these are copied after the builtin static files and will overwrite
# them
html_static_path = ['.static']

# the location of the favicon to use for all pages
# html_favicon = "_themes/oracle/static/favicon.ico"

# If not '', a 'Last updated on:' timestamp is inserted at every page bottom,
# using the given strftime format.
html_last_updated_fmt = '%b %d, %Y'

# If true, the reST sources are included in the HTML build as _sources/<name>.
html_copy_source = False

# Output file base name for HTML help builder.
htmlhelp_basename = 'oracledbdoc'

numfig = True

# default domain is C
primary_domain = "c"

# Display tables with no horizontal scrollbar
def setup(app):
    app.add_css_file('custom.css')
