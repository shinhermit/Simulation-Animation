TEMPLATE = app

QT *= opengl xml

DEPENDPATH += .
INCLUDEPATH += /usr/include/qt4/Qt /usr/include/QGLViewer /usr/include/GL

LIBS += -L/usr/local/lib -lQGLViewer
QMAKE_CXXFLAGS += -Wno-write-strings
QMAKE_CXXFLAGS_DEBUG += -Wno-deprecated
QMAKE_CXXFLAGS_RELEASE += -Wno-deprecated

HEADERS += wlCore.h saViewer.h wlQGLViewer.h wlAnimatedMesh.h wlMesh.h wlAnimatedSphere.h wlSimulator.h wlCage.h wlGround.h wlSimulationEnvironment.h \
    ParticleSimulator.h \
    SPHKernel.h \
    SpikyKernel.h \
    Poly6Kernel.h \
    ViscosityKernel.h \
    Particle.h \
    SPHKernels.h \
    ProjectView.h \
    Project.h \
    sa.h \
    DefaultParameters.h
SOURCES += sa.cxx wlCore.cxx saViewer.cxx wlQGLViewer.cxx wlAnimatedMesh.cxx wlMesh.cxx wlAnimatedSphere.cxx wlSimulator.cxx wlCage.cxx wlGround.cxx wlSimulationEnvironment.cxx \
    ParticleSimulator.cpp \
    SPHKernel.cpp \
    SpikyKernel.cpp \
    Poly6Kernel.cpp \
    ViscosityKernel.cpp \
    Particle.cpp \
    ProjectView.cpp \
    Project.cpp \
    DefaultParameters.cpp


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
