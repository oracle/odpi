#------------------------------------------------------------------------------
# Copyright (c) 2016, 2017 Oracle and/or its affiliates.  All rights reserved.
# This program is free software: you can modify it and/or redistribute it
# under the terms of:
#
# (i)  the Universal Permissive License v 1.0 or at your option, any
#      later version (http://oss.oracle.com/licenses/upl); and/or
#
# (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
#------------------------------------------------------------------------------

# Makefile to generate ODPI-C documentation using Sphinx

SPHINXOPTS    =
SPHINXBUILD   = sphinx-build
PAPER         = letter
BUILDDIR      = build

PAPEROPT_letter = -D latex_paper_size=letter
PAPEROPT_a4     = -D latex_paper_size=a4
ALLSPHINXOPTS   = -d $(BUILDDIR)/doctrees $(PAPEROPT_$(PAPER)) $(SPHINXOPTS) src

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)/*

.PHONY: html
html:
	$(SPHINXBUILD) -b html $(ALLSPHINXOPTS) $(BUILDDIR)/html
	@echo "The ODPI-C HTML documentation is in $(BUILDDIR)/html."

.PHONY: epub
epub:
	$(SPHINXBUILD) -b epub3 $(ALLSPHINXOPTS) $(BUILDDIR)/epub
	@echo "The ODPI-C epub documentation is in $(BUILDDIR)/epub."

.PHONY: pdf
pdf:
	$(SPHINXBUILD) -b latex $(ALLSPHINXOPTS) $(BUILDDIR)/pdf
	@echo "Running LaTeX files through pdflatex..."
	$(MAKE) -C $(BUILDDIR)/pdf all-pdf
	@echo "The ODPI-C PDF documentation is in $(BUILDDIR)/pdf."
