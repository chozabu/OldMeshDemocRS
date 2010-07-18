TEMPLATE = lib
CONFIG += staticlib release
CONFIG -= qt
TARGET = bitdht
QMAKE_CXXFLAGS *= -Wall -DBE_DEBUG

profiling {
	QMAKE_CXXFLAGS -= -fomit-frame-pointer
	QMAKE_CXXFLAGS *= -pg -g -fno-omit-frame-pointer
}

release {
	# not much here yet.
}

#CONFIG += debug
debug {
        QMAKE_CXXFLAGS -= -O2 -fomit-frame-pointer
        QMAKE_CXXFLAGS *= -g -fno-omit-frame-pointer
}

################################# Linux ##########################################
linux-* {
	DESTDIR = lib
	QMAKE_CC = g++
}

linux-g++ {
	OBJECTS_DIR = temp/linux-g++/obj
}

linux-g++-64 {
	OBJECTS_DIR = temp/linux-g++-64/obj
}

#################### Cross compilation for windows under Linux ####################

win32-x-g++ {	
	OBJECTS_DIR = temp/win32xgcc/obj
	DESTDIR = lib.win32xgcc
	# These have been replaced by _WIN32 && __MINGW32__
	# DEFINES *= WINDOWS_SYS WIN32 WIN_CROSS_UBUNTU
	QMAKE_CXXFLAGS *= -Wmissing-include-dirs
	QMAKE_CC = i586-mingw32msvc-g++
	QMAKE_LIB = i586-mingw32msvc-ar
	QMAKE_AR = i586-mingw32msvc-ar
	DEFINES *= STATICLIB WIN32

	INCLUDEPATH *= /usr/i586-mingw32msvc/include ${HOME}/.wine/drive_c/pthreads/include/
}
################################# Windows ##########################################

win32 {
		QMAKE_CC = g++
		OBJECTS_DIR = temp/obj
		MOC_DIR = temp/moc
		DEFINES *= STATICLIB 
		# These have been replaced by _WIN32 && __MINGW32__
		#DEFINES *= WINDOWS_SYS WIN32 STATICLIB MINGW
		DESTDIR = lib
}

################################# MacOSX ##########################################

mac {
		QMAKE_CC = g++
		OBJECTS_DIR = temp/obj
		MOC_DIR = temp/moc
		DESTDIR = lib
}

################################### COMMON stuff ##################################
################################### COMMON stuff ##################################

#DEPENDPATH += . \
INCLUDEPATH += . \

HEADERS += \
	bitdht/bdiface.h	\
	bitdht/bencode.h	\
	bitdht/bdobj.h		\
	bitdht/bdmsgs.h		\
	bitdht/bdpeer.h		\
	bitdht/bdquery.h    	\
	bitdht/bdhash.h		\
	bitdht/bdstore.h	\
	bitdht/bdnode.h		\
	bitdht/bdmanager.h	\
	bitdht/bdstddht.h	\
	util/bdnet.h	\
	util/bdthreads.h	\
	udp/udplayer.h   	\
	udp/udpstack.h		\
	udp/udpbitdht.h   	\

SOURCES += \
	bitdht/bencode.c	\
	bitdht/bdobj.cc    	\
	bitdht/bdmsgs.cc	\
	bitdht/bdpeer.cc	\
	bitdht/bdquery.cc	\
	bitdht/bdhash.cc	\
	bitdht/bdstore.cc	\
	bitdht/bdnode.cc	\
	bitdht/bdmanager.cc	\
	bitdht/bdstddht.cc	\
	util/bdnet.cc  	\
	util/bdthreads.cc  	\
	udp/udplayer.cc		\
	udp/udpstack.cc		\
	udp/udpbitdht.cc  	\


