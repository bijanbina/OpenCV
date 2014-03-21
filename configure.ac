
#Ensure that lates version of autoconf avalible
AC_PREREQ([2.63])

#create info about project
AC_INIT([OpenCV],[0.01], , opencv)

#foreign use to break the gnu strict
AM_INIT_AUTOMAKE([foreign dist-bzip2])

#create information for package
AC_CONFIG_HEADERS([config.h])

# Use it to link library
AC_PROG_LIBTOOL


# Checks for programs.
AC_PROG_CC
AC_PROG_CXX

PKG_CHECK_MODULES([OPENCV], [opencv  >= 0.25])
#force to less verbose build output
AM_SILENT_RULES([yes])

AC_CONFIG_FILES([
	Makefile
	Source/Makefile
	Resources/Makefile
	Script/Makefile
])

AC_OUTPUT

