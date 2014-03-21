#some variable for data directory defined here "These Value Can use in source file"
INCLUDES =	-DIJADI_RESOURCES=\"$(datadir)/ijadi/Resources\" \
		-DIJADI_UI=\"$(datadir)/ijadi/UI\" \
		-DIJADI_SCRIPT=\"$(datadir)/ijadi/Script\"

bin_PROGRAMS = CVTest

CVTest_SOURCES = main.c

CVTest_CFLAGS = $(GTK_CFLAGS) $(LIBANJUTA_CFLAGS)

CVTest_LDFLAGS = $(GTK_LIBS) $(LIBANJUTA_LIBS)
