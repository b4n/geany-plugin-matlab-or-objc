#!/usr/bin/make -f

PLUGIN = matlab-or-objc
PLUGIN_CFLAGS = -DLOCALEDIR='"$(datadir)/locale"' \
                -DGETTEXT_PACKAGE='"$(PLUGIN)"' \
                -DG_LOG_DOMAIN='"$(PLUGIN)"'

prefix  ?= /usr/local
datadir ?= $(prefix)/share

VPATH ?= .

# fetch from https://github.com/b4n/geany-plugin.mk
include $(VPATH)/geany-plugin.mk
