
# Build the new idl client.

AUTOMAKE_OPTIONS = foreign check-news
ACLOCAL_AMFLAGS = -I conf

lib_LTLIBRARIES = libcedar_auth.la

libcedar_auth_la_SOURCES = cedar_authentication.cc ltoa.c cedar_authentication.h
libcedar_auth_la_CPPFLAGS = $(MYSQLPP_INC) $(MYSQL_CPPFLAGS) $(IDL_CFLAGS)
libcedar_auth_la_LIBADD = $(MYSQLPP_LIB) $(MYSQL_LIBS) $(IDL_LIBS)
libcedar_auth_la_LDFLAGS = -avoid-version -module 

prodir = $(idl_pro_dir)

nobase_dist_pro_DATA = cedar_authentication_setup.pro

EXTRA_DIST = 

CLEANFILES = *~ *.sum *.log

###########################################################################

# Build linux RPMs

srpm:
	rpmbuild -ts idl-client-@PACKAGE_VERSION@.tar.gz

rpm:
	rpmbuild -tb idl-client-@PACKAGE_VERSION@.tar.gz
