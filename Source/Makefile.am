#some variable for data directory defined here "These Value Can use in source file"
INCLUDES =	-DIJADI_RESOURCES=\"$(datadir)/ijadi/Resources\" \
		-DIJADI_UI=\"$(datadir)/ijadi/UI\" \
		-DIJADI_SCRIPT=\"$(datadir)/ijadi/Script\"

bin_PROGRAMS = CVTest

CVTest_SOURCES = main.c

CVTest_CFLAGS = $(OPENCV_CFLAGS)

CVTest_LDFLAGS = $(OPENCV_LIBS)
