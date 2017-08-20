AM_CPPFLAGS = -I$(top_srcdir)/include
AM_CXXFLAGS =
AM_LDFLAGS = -lpthread
if DEBUG
AM_CXXFLAGS += -g
else
AM_CPPFLAGS += -DNDEBUG
AM_CXXFLAGS += -O3
endif
if PROFILE
AM_CXXFLAGS += -pg
AM_LDFLAGS += -pg
endif
LDADD = $(top_srcdir)/src/libsuisha.la
AM_DEFAULT_SOURCE_EXT = .cc
check_PROGRAMS = loop

TEST_EXTENSIONS = .xm
AM_TESTS_ENVIRONMENT = export XEMMAI_MODULE_PATH=$(top_builddir)/xemmai/.libs:$(XEMMAI_MODULE_PATH);
XM_LOG_COMPILER = $(XEMMAI_BIN_PATH)
AM_XM_LOG_FLAGS = --verbose
scripts = xemmai-loop.xm
EXTRA_DIST = $(scripts)

TESTS = $(check_PROGRAMS) $(scripts)
