kde_module_LTLIBRARIES= kimg_eps.la kimg_xview.la kimg_krl.la \
	         $(TIFF_MODULE) kimg_ico.la

if HAVE_LIBTIFF
TIFF_MODULE = kimg_tiff.la kimg_g3.la
TIFF_KIMGIO = tiff.kimgio g3.kimgio
endif

KIMGIO_PLUGIN = -avoid-version -no-undefined -export-symbols-regex 'kimgio_.*_(read|write)'

kimg_tiff_la_SOURCES = tiffr.cpp
kimg_tiff_la_LDFLAGS = -module $(USER_LDFLAGS) $(KIMGIO_PLUGIN)
kimg_tiff_la_LIBADD = $(LIBTIFF) 

kimg_g3_la_SOURCES = g3r.cpp
kimg_g3_la_LDFLAGS = -module $(USER_LDFLAGS) $(KIMGIO_PLUGIN)
kimg_g3_la_LIBADD = $(LIBTIFF)   

kimg_xview_la_SOURCES = xview.cpp
kimg_xview_la_LDFLAGS = -module $(USER_LDFLAGS) $(KIMGIO_PLUGIN)
kimg_xview_la_LIBADD = 

kimg_krl_la_SOURCES = krl.cpp
kimg_krl_la_LDFLAGS = -module $(USER_LDFLAGS) $(KIMGIO_PLUGIN)
kimg_krl_la_LIBADD = 

kimg_eps_la_SOURCES = eps.cpp
kimg_eps_la_LDFLAGS = -module $(USER_LDFLAGS) $(KIMGIO_PLUGIN)
kimg_eps_la_LIBADD = 

kimg_ico_la_SOURCES = ico.cpp
kimg_ico_la_LDFLAGS = -module $(USER_LDFLAGS) $(KIMGIO_PLUGIN)
kimg_ico_la_LIBADD = 

noinst_HEADERS= xview.h eps.h tiffr.h krl.h g3r.h ico.h

INCLUDES = -I$(top_srcdir)/kio -I$(top_srcdir)/dcop -I$(top_srcdir)/libltdl $(all_includes)

EXTRA_DIST = netpbm.cpp netpbm.h

servicedir = $(kde_servicesdir)
service_DATA = png.kimgio xpm.kimgio bmp.kimgio gif.kimgio pnm.kimgio xbm.kimgio \
               jpeg.kimgio xv.kimgio krl.kimgio eps.kimgio \
               $(TIFF_KIMGIO) ico.kimgio


