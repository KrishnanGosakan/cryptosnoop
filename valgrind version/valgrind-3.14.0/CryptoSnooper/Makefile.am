include $(top_srcdir)/Makefile.tool.am

EXTRA_DIST = docs/cs-manual.xml

#----------------------------------------------------------------------------
# CryptoSnooper-<platform>
#----------------------------------------------------------------------------

noinst_PROGRAMS  = CryptoSnooper-@VGCONF_ARCH_PRI@-@VGCONF_OS@
if VGCONF_HAVE_PLATFORM_SEC
noinst_PROGRAMS += CryptoSnooper-@VGCONF_ARCH_SEC@-@VGCONF_OS@
endif

CryptoSnooper_SOURCES_COMMON = cs_main.c

CryptoSnooper_@VGCONF_ARCH_PRI@_@VGCONF_OS@_SOURCES      = \
	$(CryptoSnooper_SOURCES_COMMON)
CryptoSnooper_@VGCONF_ARCH_PRI@_@VGCONF_OS@_CPPFLAGS     = \
	$(AM_CPPFLAGS_@VGCONF_PLATFORM_PRI_CAPS@)
CryptoSnooper_@VGCONF_ARCH_PRI@_@VGCONF_OS@_CFLAGS       = $(LTO_CFLAGS) \
	$(AM_CFLAGS_@VGCONF_PLATFORM_PRI_CAPS@)
CryptoSnooper_@VGCONF_ARCH_PRI@_@VGCONF_OS@_DEPENDENCIES = \
	$(TOOL_DEPENDENCIES_@VGCONF_PLATFORM_PRI_CAPS@)
CryptoSnooper_@VGCONF_ARCH_PRI@_@VGCONF_OS@_LDADD        = \
	$(TOOL_LDADD_@VGCONF_PLATFORM_PRI_CAPS@)
CryptoSnooper_@VGCONF_ARCH_PRI@_@VGCONF_OS@_LDFLAGS      = \
	$(TOOL_LDFLAGS_@VGCONF_PLATFORM_PRI_CAPS@)
CryptoSnooper_@VGCONF_ARCH_PRI@_@VGCONF_OS@_LINK = \
	$(top_builddir)/coregrind/link_tool_exe_@VGCONF_OS@ \
	@VALT_LOAD_ADDRESS_PRI@ \
	$(LINK) \
	$(CryptoSnooper_@VGCONF_ARCH_PRI@_@VGCONF_OS@_CFLAGS) \
	$(CryptoSnooper_@VGCONF_ARCH_PRI@_@VGCONF_OS@_LDFLAGS)

if VGCONF_HAVE_PLATFORM_SEC
CryptoSnooper_@VGCONF_ARCH_SEC@_@VGCONF_OS@_SOURCES      = \
	$(CryptoSnooper_SOURCES_COMMON)
CryptoSnooper_@VGCONF_ARCH_SEC@_@VGCONF_OS@_CPPFLAGS     = \
	$(AM_CPPFLAGS_@VGCONF_PLATFORM_SEC_CAPS@)
CryptoSnooper_@VGCONF_ARCH_SEC@_@VGCONF_OS@_CFLAGS       = $(LTO_CFLAGS) \
	$(AM_CFLAGS_@VGCONF_PLATFORM_SEC_CAPS@)
CryptoSnooper_@VGCONF_ARCH_SEC@_@VGCONF_OS@_DEPENDENCIES = \
	$(TOOL_DEPENDENCIES_@VGCONF_PLATFORM_SEC_CAPS@)
CryptoSnooper_@VGCONF_ARCH_SEC@_@VGCONF_OS@_LDADD        = \
	$(TOOL_LDADD_@VGCONF_PLATFORM_SEC_CAPS@)
CryptoSnooper_@VGCONF_ARCH_SEC@_@VGCONF_OS@_LDFLAGS      = \
	$(TOOL_LDFLAGS_@VGCONF_PLATFORM_SEC_CAPS@)
CryptoSnooper_@VGCONF_ARCH_SEC@_@VGCONF_OS@_LINK = \
	$(top_builddir)/coregrind/link_tool_exe_@VGCONF_OS@ \
	@VALT_LOAD_ADDRESS_SEC@ \
	$(LINK) \
	$(CryptoSnooper_@VGCONF_ARCH_SEC@_@VGCONF_OS@_CFLAGS) \
	$(CryptoSnooper_@VGCONF_ARCH_SEC@_@VGCONF_OS@_LDFLAGS)
endif


