#some variable for data directory defined here "These Value Can use in source file"
INCLUDES =	-DIJADI_RESOURCES=\"$(datadir)/ijadi/Resources\" \
		-DIJADI_UI=\"$(datadir)/ijadi/UI\" \
		-DIJADI_SCRIPT=\"$(datadir)/ijadi/Script\"

bin_PROGRAMS = CVTest

CVTest_SOURCES = example6.c

CVTest_CFLAGS = $(OPENCV_CFLAGS) $(GTK_CFLAGS)

CVTest_LDFLAGS = $(OPENCV_LIBS) $(GTK_LIBS)
