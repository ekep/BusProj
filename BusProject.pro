TEMPLATE = app

QT += qml quick location

CONFIG += c++11

SOURCES += main.cpp \
    backend.cpp

CONFIG += qmltypes
QML_IMPORT_NAME = io.qt.examples.backend
QML_IMPORT_MAJOR_VERSION = 1

qmlfiles.files = $$files($$PWD/*.qml)
qmlfiles.prefix = /
qmlfiles.base = $$PWD

RESOURCES += qmlfiles

DISTFILES += $$files($$PWD/*.py) $$files($$PWD/*.GeoJSON) main.qml \
    StopMarker.qml \
    stops.txt

target.path = $$[QT_INSTALL_EXAMPLES]/location/itemview_transitions
INSTALLS += target

HEADERS += \
    backend.h \
    main.h
