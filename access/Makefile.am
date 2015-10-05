
# Build opendapcmdln, part of the BES code.

AUTOMAKE_OPTIONS = foreign

# These are not used by automake but are often useful for certain types of
# debugging. The best way to use these is to run configure as:
#     export CXXFLAGS='...'; ./configure --disable-shared
# the --disable-shared is not required, but it seems to help with debuggers.
CXXFLAGS_DEBUG = -g3 -O0 -fno-defer-pop -Wall -W -Wcast-align -Werror
TEST_COV_FLAGS = -ftest-coverage -fprofile-arcs

bin_PROGRAMS = check_access

check_access_SOURCES = check_access.cc DODSEncode.cc DODSMySQLChannel.cc \
	DODSMySQLConnect.cc DODSMySQLQuery.cc DODSMySQLResult.cc \
	DODSEncode.h DODSException.h DODSMySQLChannel.h DODSMySQLConnect.h \
	DODSMySQLQuery.h DODSMySQLResult.h
check_access_CPPFLAGS = $(MYSQL_CPPFLAGS)
check_access_LDADD = $(MYSQL_LIBS)

