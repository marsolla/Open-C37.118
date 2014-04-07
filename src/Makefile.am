#
AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} 

lib_LTLIBRARIES = libopenc37118-@OPENC37118_API_VERSION@.la
libopenc37118_@OPENC37118_API_VERSION@_la_SOURCES = c37118.cpp \
c37118configuration.cpp \
c37118header.cpp \
c37118command.cpp \
c37118data.cpp \
c37118pmustation.cpp

libopenc37118_@OPENC37118_API_VERSION@_la_LDFLAGS = -version-info $(OPENC37118_SO_VERSION)
openc37118_includedir = $(includedir)/openc37118-$(OPENC37118_API_VERSION)
nobase_openc37118_include_HEADERS = c37118command.h \
c37118data.h \
c37118header.h \
c37118configuration.h \
c37118.h \
c37118pmustation.h

openc37118_libincludedir = $(libdir)/openc37118-$(OPENC37118_API_VERSION)/include

openc37118_LDADD = ./liboopenc37118-${OPENC37118_API_VERSION}.la 









