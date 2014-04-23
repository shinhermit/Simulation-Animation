///////////////////////////////////////////////////////////////////////////////
//  File         :    wlAnimatedMesh.h                                       //
//  Author       :    Wilfrid LEFER                                          //
//  Comment      :    Modele de mesh animee                                  //
//  Related file :    wlAnimatedMesh.cxx, wlMesh.{h,cxx}                     //
//                    Wilfrid Lefer - UPPA - 2010                            //
///////////////////////////////////////////////////////////////////////////////


#ifndef _WL_ANIMATED_MESH_H
#define _WL_ANIMATED_MESH_H

#include <wlMesh.h>
#include <QTimer>

#define G 9.81

/** \brief wlAnimatedMesh est une classe pour l'animation d'une mesh.
*/
class wlAnimatedMesh : public wlMesh
{
  Q_OBJECT

public:
  /// \brief Cree une nouvelle mesh animee.
  /// La mesh est eventuellement initialisee a partir du contenu du fichier.
  wlAnimatedMesh(int debug=0, wlQGLViewer *const v=NULL, QString filename=QString());
  virtual ~wlAnimatedMesh() {}
  virtual char *GetClassName() {return "wlAnimatedMesh";}

  /// \brief Obtient la position courante du mesh, wich is initial position plus translation
  virtual QVector<double> getPosition()const;

signals:

public slots:
  /// \brief Definit la position initiale de l'objet.
  void SetPosition(double Px, double Py, double Pz);
  /// \brief Definit la composante X de la force s'appliquant sur l'ensemble de la mesh.
  void SetPositionX(QString Px);
  /// \brief Definit la composante Y de la force s'appliquant sur l'ensemble de la mesh.
  void SetPositionY(QString Py);
  /// \brief Definit la composante Z de la force s'appliquant sur l'ensemble de la mesh.
  void SetPositionZ(QString Pz);
  /// \brief Definit la vitesse instantanee de la mesh.
  void SetVelocity(double Vx, double Vy, double Vz);
  /// \brief Definit la composante X de la vitesse instantanee.
  void SetVelocityX(QString Vx);
  /// \brief Definit la composante Y de la vitesse instantanee.
  void SetVelocityY(QString Vy);
  /// \brief Definit la composante Z de la vitesse instantanee.
  void SetVelocityZ(QString Vz);
  /// \brief Definit le pas de temps de la simulation
  void SetTimeStep(double t);
  /// \brief Definit le pas de temps de la simulation
  void SetTimeStep(QString t);
  /// \brief Renvoie la vitesse actuelle du centre de gravite de la mesh au format (<x>, <y>, <z>).
  inline QVector<double> GetVelocity() {return this->cvel;}

  /// Imprime le contenu de la mesh a la resolution courante : coordonnees des sommets, aretes et faces.
  virtual void PrintSelf();
  /// Imprime le contenu detaille de la mesh a la resolution courante : contenu des differentes tables.
  virtual void PrintContent();

  /// \brief Ramene l'objet dans sa configuration initiale.
  virtual void Reset();
  /// \brief Execute un pas de temps de l'animation.
  virtual void Step();

  virtual void makeList(int reset_camera=0);
  virtual void draw();

protected:
  // la vitesse instantanee de la mesh
  QVector<double> velocity;
  // le pas de temps de la simulation
  double timestep;
  // le pas de temps courant
  int cstep;
  // la vitesse au pas de temps courant
  QVector<double> cvel;
  // les coordonnees du centre de l'objet, afin de le ramener a sa place initiale si besoin
  std::vector<float> position;
  // le vecteur translation au pas de temps courant pour amener l'objet a sa position actuelle
  QVector<double> Tmat;

  // vide toutes les structures internes
  virtual void Clear();
  // cette methode contient les transformations geometriques OpenGL a l'objet
  virtual void glTransformations();
};

#endif
