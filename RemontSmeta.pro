QT += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    db/dbexception.cpp \
    db/dbtable.cpp \
    db/inputservice.cpp \
    dbtablebaseproxymodel.cpp \
    editstringdialog.cpp \
    globals.cpp \
    loggingcategories.cpp \
    main.cpp \
    mainwindow.cpp \
    models/treeitem.cpp \
    models/treemodel.cpp \
    navigationdockwidget.cpp \
    person/adresslineedit.cpp \
    person/editcontactdlg.cpp \
    person/tpersonservice.cpp \
    person/personpanel.cpp \
    subwindowbase.cpp \
    tables/tclient.cpp

HEADERS += \
    db/DbField.hpp \
    db/dbexception.h \
    db/dbtable.h \
    db/inputservice.h \
    dbtablebaseproxymodel.h \
    editstringdialog.h \
    exceptionbase.h \
    globals.h \
    loggingcategories.h \
    mainwindow.h \
    models/treeitem.h \
    models/treemodel.h \
    navigationdockwidget.h \
    person/adresslineedit.h \
    person/editcontactdlg.h \
    person/tpersonservice.h \
    person/personpanel.h \
    subwindowbase.h \
    tables/tclient.h \
    typelist.hpp

FORMS += \
    editstringdialog.ui \
    person/personpanel.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
