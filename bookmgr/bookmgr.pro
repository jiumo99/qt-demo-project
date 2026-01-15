QT += core gui widgets sql charts
CONFIG += c++17

TARGET = BookBorrowSystem
TEMPLATE = app

SOURCES += \
    bookmgr.cpp \
    dbhelper.cpp \
    login.cpp \
    main.cpp \
    mainwindow.cpp \
    record.cpp \
    usermgr.cpp

HEADERS += \
    bookmgr.h \
    dbhelper.h \
    login.h \
    mainwindow.h \
    record.h \
    usermgr.h

FORMS += \
    bookmgr.ui \
    login.ui \
    mainwindow.ui \
    record.ui \
    usermgr.ui

RC_FILE = $$PWD/res/app.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
