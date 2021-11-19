QT += core gui sql qml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 qml_debug
DESTDIR = bin

DEFINES += QT_QML_DEBUG

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    clientswindow/clientsviewmodel.cpp \
    clientswindow/viewpartywindow.cpp \
    db/dbtableformat.cpp \
    jsclasses/jsclassexecuter.cpp \
    jsclasses/jsconsole.cpp \
    renovation/questionstreeview.cpp \
    renovation/renovationcontentwidget.cpp \
    renovation/renovationquestitemdelegate.cpp \
    renovation/renovationquesttionmodel.cpp \
    renovation/trenovationservice.cpp \
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
    renovation/renovationpanel.cpp \
    subwindowbase.cpp \
    tables/renovation.cpp \
    tables/tclient.cpp \
    widgets/filteredtablewidget.cpp \
    widgets/selectpartydlg.cpp

HEADERS += \
    FactoryTemplate.h \
    clientswindow/clientsviewmodel.h \
    clientswindow/viewpartywindow.h \
    db/dbtableformat.hpp \
    jsclasses/jsclassexecuter.h \
    jsclasses/jsconsole.h \
    renovation/questionstreeview.h \
    renovation/renovationcontentwidget.h \
    renovation/renovationquestitemdelegate.h \
    renovation/renovationquesttionmodel.h \
    renovation/trenovationservice.h \
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
    renovation/renovationpanel.h \
    subwindowbase.h \
    tables/renovation.h \
    tables/tclient.h \
    typelist.hpp \
    widgets/filteredtablewidget.h \
    widgets/selectpartydlg.h

FORMS += \
    editstringdialog.ui \
    person/personpanel.ui \
    renovation/renovationcontentwidget.ui \
    renovation/renovationpanel.ui \
    widgets/selectpartydlg.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
