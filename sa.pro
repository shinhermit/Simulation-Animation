TEMPLATE = app

QT *= opengl xml

DEPENDPATH += .
INCLUDEPATH += /usr/include/qt4/Qt /usr/include/QGLViewer /usr/include/GL

LIBS += -L/usr/local/lib -lQGLViewer
QMAKE_CXXFLAGS += -Wno-write-strings
QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated
QMAKE_CXXFLAGS_RELEASE += -Wno-deprecated

HEADERS +=\
    ParticleSimulator.h \
    SPHKernels.h \
    ProjectView.h \
    Project.h \
    sa.h \
    DefaultParameters.h \
    Environment.h
SOURCES +=\
    ParticleSimulator.cpp \
    ProjectView.cpp \
    Project.cpp \
    DefaultParameters.cpp \
    Environment.cpp \
    SPHKernels.cpp \
    sa.cpp


  QT       += core

  CONFIG   += console
#  CONFIG   += qt debug warn_on
  CONFIG   -= app_bundle

  RESOURCES = particules.qrc

  LIBS += -L/usr/local/lib -lQGLViewer -lGLU

  LIBS += -L/usr/local/lib/opencl -lQtOpenCL -lOpenCL
  INCLUDEPATH += /usr/local/lib/opencl

  TARGET = particules.out

FORMS += \
    MainWindow.ui
