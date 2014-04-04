///////////////////////////////////////////////////////////////////////////////
//  File         :    saViewer.cxx                                           //
//  Author       :    Wilfrid LEFER                                          //
//  Comment      :    Un viewer specifique pour les TP de SA                 //
//  Related file :    saViewer.h                                             //
//                    Wilfrid Lefer - UPPA - 2010                            //
///////////////////////////////////////////////////////////////////////////////


#include <saViewer.h>
#include <wlMesh.h>

saViewer::saViewer(QWidget *parent, QSize size, const QGLWidget *shareWidget, Qt::WFlags flags)
  : wlQGLViewer(parent, size, shareWidget, flags)
{
}

saViewer::saViewer(QGLContext *context, QWidget *parent, QSize size, const QGLWidget *shareWidget, Qt::WFlags flags)
  : wlQGLViewer(context, parent, size, shareWidget, flags)
{
}

saViewer::saViewer(const QGLFormat &format, QWidget *parent, QSize size, const QGLWidget *shareWidget, Qt::WFlags flags)
  : wlQGLViewer(format, parent, size, shareWidget, flags)
{
}

void
saViewer::draw()
{
  if (this->meshes.size() == 0)
    return;
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  for (int i=0 ; i<this->meshes.size() ; i++)
    glCallList(this->meshes[i]->GetList());
  static int first = 1;
  if (first) {
    this->camera()->setPosition(qglviewer::Vec(0.0, -50.0, 50.0));
    this->camera()->setRevolveAroundPoint(qglviewer::Vec(0.0, 0.0, 0.0));
    this->camera()->setViewDirection(qglviewer::Vec(0.0, 50.0, -50.0));
    this->camera()->setFieldOfView(1.5);
    first = 0;
  }
  glFlush();
}
