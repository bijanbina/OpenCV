#some variable for data directory defined here "These Value Can use in source file"
INCLUDES =	-DIJADI_RESOURCES=\"$(datadir)/ijadi/Resources\" \
		-DIJADI_UI=\"$(datadir)/ijadi/UI\" \
		-DIJADI_SCRIPT=\"$(datadir)/ijadi/Script\"

bin_PROGRAMS = CVTest

bin_PROGRAMS = CV_GTK

CVTest_SOURCES = example6.c

CVTest_CFLAGS = $(OPENCV_CFLAGS) $(GTK_CFLAGS)

CVTest_LDFLAGS = $(OPENCV_LIBS) $(GTK_LIBS)

CV_GTK_SOURCES = main.c

CV_GTK_CFLAGS = $(OPENCV_CFLAGS) $(GTK_CFLAGS)# -E

CV_GTK_LDFLAGS = $(OPENCV_LIBS) $(GTK_LIBS)
