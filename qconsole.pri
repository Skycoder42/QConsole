
HEADERS += \
	$$PWD/qconsole.h

SOURCES += \
	$$PWD/qconsole.cpp

win32 {
	HEADERS += $$PWD/readthread_win.h
	SOURCES += $$PWD/readthread_win.cpp
}

INCLUDEPATH += $$PWD

QDEP_PACKAGE_EXPORTS += Q_CONSOLE_EXPORT
!qdep_build: DEFINES += "Q_CONSOLE_EXPORT="
