///////////////////////////////////////////////////////////////////////////////
//  File         :    wlQGLViewer.cxx                                        //
//  Author       :    Wilfrid LEFER                                          //
//  Comment      :    Le QGLViewer dont quelques mouse bindings ont ete      //
//                    modifies a ma convenance :-)                           //
//  Related file :    wlQGLViewer.h                                          //
//                    Wilfrid Lefer - UPPA - 2008                            //
///////////////////////////////////////////////////////////////////////////////


#include <wlQGLViewer.h>
#include <wlMesh.h>
#include <QFile>

wlQGLViewer::wlQGLViewer(QWidget *parent, QSize size, const QGLWidget *shareWidget, Qt::WFlags flags)
  : QGLViewer(parent, shareWidget, flags)
{
  this->init(size);
}

wlQGLViewer::wlQGLViewer(QGLContext *context, QWidget *parent, QSize size, const QGLWidget *shareWidget, Qt::WFlags flags)
  : QGLViewer(context, parent, shareWidget, flags)
{
  this->init(size);
}

wlQGLViewer::wlQGLViewer(const QGLFormat &format, QWidget *parent, QSize size, const QGLWidget *shareWidget, Qt::WFlags flags)
  : QGLViewer(format, parent, shareWidget, flags)
{
  this->init(size);
}

QSize
wlQGLViewer::sizeHint() const
{
  QSize size = QWidget::sizeHint();
  return size.isValid() ? size : this->size;
}

void
wlQGLViewer::init(QSize size)
{
  this->size = size;
  this->setSnapshotFormat("JPEG");
  this->setSnapshotQuality(100);
  /*
  Ca ne marche pas sur Mac ! Probablement un probleme de codage des touches.
  this->setMouseBinding(Qt::SHIFT + Qt::LeftButton, ZOOM_ON_PIXEL);
  // double clic sur le bouton de gauche permet de voir la scene entiere
  this->setMouseBinding(Qt::LeftButton, SHOW_ENTIRE_SCENE, true);
  */
  // clic droit pour zoomer
  this->setMouseBinding(Qt::RightButton, CAMERA, ZOOM);
  connect((QObject *)this, SIGNAL(drawNeeded()), this, SLOT(draw()));
}

void
wlQGLViewer::updateGL()
{
  QGLViewer::updateGL();
  if (this->save_snapshot)
    this->SaveImage();
}

void
wlQGLViewer::SetImageDirectory(QString dir)
{
  this->image_directory = dir;
  if (this->image_directory.endsWith('/'))
    this->image_directory.chop(1);
}

void
wlQGLViewer::SetImageBasename(QString basename)
{
  this->image_basename = basename;
}

void
wlQGLViewer::ClearImages()
{
  if (!this->image_directory.isNull() && !this->image_basename.isNull()) {
    QString bname(this->image_directory + "/" + this->image_basename);
    for (int i=0 ;; i++) {
      QString name(bname + QString::number(i));
      QFile f(name);
      if (!f.exists())
	break;
      f.remove();
    }
  }
}

void
wlQGLViewer::SaveImage()
{
  if (!this->image_directory.isNull() && !this->image_basename.isNull()) {
    QString bname(this->image_directory + "/" + this->image_basename);
    for (int i=0 ;; i++) {
      QString name = bname + QString("%1").arg(i, 4, 10, QChar('0')) + ".jpg";
      QFile f(name);
      if (!f.exists()) {
	this->saveSnapshot(name, false);
	break;
      }
    }
  }
}

void
wlQGLViewer::draw()
{
  if (this->meshes.size() == 0)
    return;
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  for (int i=0 ; i<this->meshes.size() ; i++)
    glCallList(this->meshes[i]->GetList());
  std::vector<float> ext1 = this->meshes[0]->GetExtends();
  std::vector<float> ext2 = this->meshes[1]->GetExtends();
  ext1[0] = ext1[0]<ext2[0] ? ext1[0] : ext2[0];
  ext1[1] = ext1[1]>ext2[1] ? ext1[1] : ext2[1];
  ext1[2] = ext1[2]<ext2[2] ? ext1[2] : ext2[2];
  ext1[3] = ext1[3]>ext2[3] ? ext1[3] : ext2[3];
  ext1[4] = ext1[4]<ext2[4] ? ext1[4] : ext2[4];
  ext1[5] = ext1[5]>ext2[5] ? ext1[5] : ext2[5];
  ext2[0] = ext1[1] - ext1[0];
  ext2[1] = ext1[3] - ext1[2];
  ext2[2] = ext1[5] - ext1[4];
  float radius = ext2[0] > ext2[1] ?
                   ext2[0] > ext2[2] ? ext2[0]/2 :
                                       ext2[2]/2 :
                   ext2[1] > ext2[2] ? ext2[1]/2 :
                                       ext2[2]/2;
  this->setSceneRadius(radius);
  this->setSceneCenter(qglviewer::Vec(ext1[0]+ext2[0]/2, ext1[2]+ext2[1]/2, ext1[4]+ext2[2]/2));
  this->camera()->setRevolveAroundPoint(this->sceneCenter());
  this->camera()->showEntireScene();
  glFlush();
}
