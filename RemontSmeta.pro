QT += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    clientswindow/clientsviewmodel.cpp \
    clientswindow/viewpartywindow.cpp \
    widgets/columnalignedlayout.cpp \
    db/dbexception.cpp \
    db/dbtable.cpp \
    db/inputservice.cpp \
    dbtablebaseproxymodel.cpp \
    editstringdialog.cpp \
    globals.cpp \
    loggingcategories.cpp \
    main.cpp \
    mainwindow.cpp \
    models/jsontablemodel.cpp \
    models/subwindowsmodel.cpp \
    models/treeitem.cpp \
    models/treemodel.cpp \
    navigationdockwidget.cpp \
    person/adresslineedit.cpp \
    person/editcontactdlg.cpp \
    person/tpersonservice.cpp \
    person/personpanel.cpp \
    reference/jsonmodeleditor.cpp \
    reference/renovationtypeeditor.cpp \
    renovationpanel.cpp \
    subwindowbase.cpp \
    tables/renovation.cpp \
    tables/tclient.cpp \
    widgets/filteredtablewidget.cpp

HEADERS += \
    FactoryTemplate.h \
    clientswindow/clientsviewmodel.h \
    clientswindow/viewpartywindow.h \
    widgets/columnalignedlayout.h \
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
    models/jsontablemodel.h \
    models/subwindowsmodel.h \
    models/treeitem.h \
    models/treemodel.h \
    navigationdockwidget.h \
    person/adresslineedit.h \
    person/editcontactdlg.h \
    person/tpersonservice.h \
    person/personpanel.h \
    reference/jsonmodeleditor.h \
    reference/renovationtypeeditor.h \
    renovationpanel.h \
    subwindowbase.h \
    tables/renovation.h \
    tables/tclient.h \
    typelist.hpp \
    widgets/filteredtablewidget.h

FORMS += \
    editstringdialog.ui \
    person/personpanel.ui \
    renovationpanel.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
