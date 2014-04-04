///////////////////////////////////////////////////////////////////////////////
//  File         :    saViewer.h                                             //
//  Author       :    Wilfrid LEFER                                          //
//  Comment      :    Un viewer specifique pour les TP de SA                 //
//  Related file :    saViewer.cxx                                           //
//                    Wilfrid Lefer - UPPA - 2010                            //
///////////////////////////////////////////////////////////////////////////////


#ifndef _WL_SA_VIEWER_H
#define _WL_SA_VIEWER_H

#include <wlQGLViewer.h>

class wlMesh;

/** \brief saViewer est une reimplementation da la classe wlQGLViewer permettant de fixer le point de vue afin qu'il ne bouge pas.

    C'est indispensable afin d'apprecier le mouvement des objets au cours du temps.
*/
class saViewer : public wlQGLViewer
{
  Q_OBJECT

public:
  saViewer(QWidget *parent=0, QSize size=QSize(SAZE,SAZE), const QGLWidget *shareWidget=0, Qt::WFlags flags=0);
  saViewer(QGLContext *context, QWidget *parent=0, QSize size=QSize(SAZE,SAZE), const QGLWidget *shareWidget=0, Qt::WFlags flags=0);
  saViewer(const QGLFormat &format, QWidget *parent=0, QSize size=QSize(SAZE,SAZE), const QGLWidget *shareWidget=0, Qt::WFlags flags=0);
  virtual ~saViewer() {};
  virtual char *GetClassName() {return "saViewer";};

public slots:
  virtual void draw();
};

#endif
