PUBLIC_HEADERS += \
	$$PWD/qconsole.h 

HEADERS += $$PUBLIC_HEADERS \
    $$PWD/readthread_win.h

SOURCES += \
	$$PWD/qconsole.cpp \
    $$PWD/readthread_win.cpp

INCLUDEPATH += $$PWD
