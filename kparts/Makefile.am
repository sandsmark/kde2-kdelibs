####### General stuff

INCLUDES= -I$(srcdir)/../ -I$(top_srcdir)/kio/ -I$(top_srcdir)/libltdl \
	-I$(top_srcdir)/kfile $(all_includes)
libkparts_la_LDFLAGS = -version-info 1:0:0 -no-undefined
libkparts_la_LIBADD  = $(LIB_KFILE) $(LIB_KIO)

SUBDIRS = .

check-local:
	(cd tests && $(MAKE) check )

####### Files

lib_LTLIBRARIES = libkparts.la

libkparts_la_SOURCES = part.cpp plugin.cpp partmanager.cpp mainwindow.cpp dockmainwindow.cpp \
		       event.cpp browserextension.cpp factory.cpp  historyprovider.cpp \
		       browserinterface.cpp

kpartsincludedir = $(includedir)/kparts
kpartsinclude_HEADERS = part.h plugin.h partmanager.h mainwindow.h dockmainwindow.h event.h \
	browserextension.h factory.h historyprovider.h browserinterface.h

libkparts_la_METASOURCES = AUTO

servicetype_DATA = kpart.desktop krop.desktop krwp.desktop browserview.desktop 
servicetypedir = $(kde_servicetypesdir)

