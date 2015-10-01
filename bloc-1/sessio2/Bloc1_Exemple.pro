TEMPLATE    = app
QT         += opengl

LIBS += /usr/local/Cellar/glew/1.11.0/lib/libGLEW.a
INCLUDEPATH +=  /usr/include/glm /usr/include/GL

HEADERS += MyGLWidget.h

SOURCES += main.cpp \
        MyGLWidget.cpp
