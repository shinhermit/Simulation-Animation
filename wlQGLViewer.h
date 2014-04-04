///////////////////////////////////////////////////////////////////////////////
//  File         :    wlQGLViewer.h                                          //
//  Author       :    Wilfrid LEFER                                          //
//  Comment      :    Le QGLViewer dont quelques mouse bindings ont ete      //
//                    modifies a ma convenance :-)                           //
//  Related file :    wlQGLViewer.cxx                                        //
//                    Wilfrid Lefer - UPPA - 2008                            //
///////////////////////////////////////////////////////////////////////////////


#ifndef _WL_QGLVIEWER_H
#define _WL_QGLVIEWER_H

#include <qglviewer.h>

#define SAZE 500

class wlMesh;

/** \brief wlQGLViewer est une reimplementation du QGLViewer permettant de redefinir certaines interactions avec la souris.

    Une serie de methodes permettent de sauvegarder facilement une sequence d'images, par exemple lors d'une animation.
*/
class wlQGLViewer : public QGLViewer
{
  Q_OBJECT

public:
  wlQGLViewer(QWidget *parent=0, QSize size=QSize(SAZE,SAZE), const QGLWidget *shareWidget=0, Qt::WFlags flags=0);
  wlQGLViewer(QGLContext *context, QWidget *parent=0, QSize size=QSize(SAZE,SAZE), const QGLWidget *shareWidget=0, Qt::WFlags flags=0);
  wlQGLViewer(const QGLFormat &format, QWidget *parent=0, QSize size=QSize(SAZE,SAZE), const QGLWidget *shareWidget=0, Qt::WFlags flags=0);
  virtual ~wlQGLViewer() {};
  virtual char *GetClassName() {return "wlQGLViewer";};
  virtual QSize sizeHint () const;

  /// \brief Ajoute une mesh a la liste des mesh gerees par le viewer.
  void AddMesh(wlMesh *m) {this->meshes.append(m);};

  /// \brief Specifie les noms du repertoire et du nom de base des fichiers image qui seront utilises pour sauvegarder les differentes vues 3D.
  void SetImageNames(QString dir, QString basename) {
    this->SetImageDirectory(dir);
    this->SetImageBasename(basename);
  };

public slots:
  /// \brief La methode de la classe mere est inchangee sauf qu'on sauvegarde eventuellement la vue immediatement le calcul fini.
  void updateGL();
  /// \brief Specifie le nom du repertoire ou stocker les sauvegardes de vues 3D.
  void SetImageDirectory(QString dir);
  /// \brief Specifie le nom de base des fichiers image contenant les differentes vues 3D.
  void SetImageBasename(QString basename);
  /// \brief Precise si les snapshot doivent etre sauvegardes ou pas.
  void SaveSnapshot(int on) {
    this->save_snapshot = on ? true : false;
  }
  /// \brief Sauvegarde l'image courante.
  /// Les images sont sauvegardees dans le repertoire prevu a cet effet avec une sequence de noms commencant par <em>basename</em>.
  void SaveImage();
  /// \brief Detruit toutes les images dans le repertoire reserve a cet effet et commencant par le nom de base courant.
  void ClearImages();

  virtual void draw();

protected:
  // la liste des mesh gerees par le viewer
  QList<wlMesh *> meshes;
  // la taille recomandee pour le viewer
  QSize size;
  // le repertoire contenant les images snapshot
  QString image_directory;
  // le nom de base des images snapshot
  QString image_basename;
  // est-ce que l'on doit sauvegarder les images ?
  bool save_snapshot;

  void init(QSize size);
};

#endif
