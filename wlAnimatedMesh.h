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

#define PURE_KINEMATIC 0
#define PENALTY        1
#define IMPULSE_FORCE  2

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

  /// \brief Test la collision avec une autre mesh de meme geometrie et calcule point d'impact et normale en ce point.
  /// Trois situations peuvent se produire :
  /// <ul><li>il n'y a pas de collision et alors un tableau vide est renvoye,</li>
  ///     <li>il y a une collision mais celle-ci a ete detectee au dela de la tolerance admise, soit trop tard : le
  ///         tableau renvoye comporte alors une seule valeur, qui est a distance de laquelle chaque objet s'est enfonce dans l'autre,</li>
  ///     <li>il y a une collision et celle-ci est dans le perimetre tolere : le tableau renvoye comporte alors 6 valeurs, le point de contact et la normale en ce point.</li>
  /// </ul>
  /// Cette methode est normalement pure virtuelle mais a ete definie afin que l'application puisse s'execute sans instance definie.
  virtual QVector<double> ComputeCollisionWithOther(wlAnimatedMesh *other);
  /// \brief Test la collision avec un plan et calcule point d'impact et normale en ce point.
  /// Trois situations peuvent se produire :
  /// <ul><li>il n'y a pas de collision et alors un tableau vide est renvoye,</li>
  ///     <li>il y a une collision mais celle-ci a ete detectee au dela de la tolerance admise, soit trop tard : le
  ///         tableau renvoye comporte alors une seule valeur, qui est a distance de laquelle chaque objet s'est enfonce dans l'autre,</li>
  ///     <li>il y a une collision et celle-ci est dans le perimetre tolere : le tableau renvoye comporte alors 6 valeurs, le point de contact et la normale en ce point.</li>
  /// </ul>
  /// Cette methode est normalement pure virtuelle mais a ete definie afin que l'application puisse s'execute sans instance definie.
  virtual QVector<double> ComputeCollisionWithPlan(QVector<double> equation);
  /// \brief Calcule la reaction a une collision, en fonction des elements en tant que parametres (point d'impact et normale) obtenus lors du dernier calcul de collision.
  /// Cette methode est normalement pure virtuelle mais a ete definie afin que l'application puisse s'execute sans instance definie.
  virtual void ComputeReaction(QVector<double> impact);

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
  /// \brief Definit la force s'appliquant sur l'ensemble de la mesh.
  void SetForce(double Fx, double Fy, double Fz);
  /// \brief Definit la composante X de la force s'appliquant sur l'ensemble de la mesh.
  void SetForceX(QString Fx);
  /// \brief Definit la composante Y de la force s'appliquant sur l'ensemble de la mesh.
  void SetForceY(QString Fy);
  /// \brief Definit la composante Z de la force s'appliquant sur l'ensemble de la mesh.
  void SetForceZ(QString Fz);
  /// \brief Definit la vitesse instantanee de la mesh.
  void SetVelocity(double Vx, double Vy, double Vz);
  /// \brief Definit la composante X de la vitesse instantanee.
  void SetVelocityX(QString Vx);
  /// \brief Definit la composante Y de la vitesse instantanee.
  void SetVelocityY(QString Vy);
  /// \brief Definit la composante Z de la vitesse instantanee.
  void SetVelocityZ(QString Vz);
//  /// \brief Definit la masse de la mesh.
//  void SetMass(float mass) throw(std::invalid_argument);

   /// \brief Definit le type de reaction utilisee pour gerer le rebond.
  virtual void SetReaction(int index);
  /// \brief Definit le coefficient d'attenuation du rebond lors d'une reaction cinematique pure.
  virtual void SetAttenuationCoefficientForPureKinematicReaction(double k);
  /// \brief Definit le coefficient d'attenuation du rebond lors d'une reaction cinematique pure.
  virtual void SetAttenuationCoefficientForPureKinematicReaction(QString t);
  /// \brief Definit la resistance des ressorts utilises lors d'une reaction a handicap.
  virtual void SetSpringCoefficientForPenaltyReaction(double Ks);
  /// \brief Definit la resistance des ressorts utilises lors d'une reaction a handicap.
  virtual void SetSpringCoefficientForPenaltyReaction(QString t);
  /// \brief Definit la masse associee aux points lors d'une reaction a handicap.
  virtual void SetPointWeightForPenaltyReaction(double m);
  /// \brief Definit la masse associee aux points lors d'une reaction a handicap.
  virtual void SetPointWeightForPenaltyReaction(QString t);

  /// \brief Definit le nombre de pas de temps de l'animation.
  void SetNumberOfTimeSteps(int n);
  /// \brief Definit le nombre de pas de temps de l'animation.
  void SetNumberOfTimeSteps(QString n);
  /// \brief Definit le pas de temps de l'animation.
  void SetTimeStep(double t);
  /// \brief Definit le pas de temps pour le calcul de l'animation.
  virtual void SetTimeStep(QString t);

  /// \brief Definit la tolerance pour la detection des collisions instantanees.
  /// On considerera que la collision est instantanee si la distance entre les objets est inferieure a d.
  void SetTolerance(double d) {this->tolerance = d;}
  /// Renvoie la position actuelle du centre de gravite de la mesh au format (<x>, <y>, <z>).
  QVector<double> GetCurrentCenter();
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
  /// \brief Revient au pas de temps precedent.
  virtual void Back();
  /// \brief Lance l'animation.
  void Play();
  /// \brief Stoppe l'animation en cours.
  void Stop();

  virtual void makeList(int reset_camera=0);
  virtual void draw();

protected:
  // un timer pour faire du temps reel
  QTimer *timer;
  // la force s'appliquant sur l'ensemble de la mesh
  QVector<double> force;
  // la vitesse instantanee de la mesh
  QVector<double> velocity;
  // le type de reaction utilisee pour gerer les rebonds
  int reaction;
  // le coefficient d'attenuation du rebond lors d'une reaction cinematique pure
  double k;
  // la resistance des ressorts utilises lors d'une reaction a handicap
  double Ks;
  // la masse associee aux points lors d'une reaction a handicap
  double m;
  // le pas de temps de la simulation
  double timestep;
  // le nombre de pas de temps de la simulation
  int nsteps;
  // le pas de temps courant
  int cstep;
  // la vitesse au pas de temps courant
  QVector<double> cvel;
  // la vitesse au pas de temps precedent
  QVector<double> cvel_p;
  // les coordonnees du centre de l'objet, afin de le ramener a sa place initiale si besoin
  std::vector<float> position;
  // le vecteur translation au pas de temps courant pour amener l'objet a sa position actuelle
  QVector<double> Tmat;
  // le vecteur translation au pas de temps precedent pas de temps
  QVector<double> Tmat_p;
  // position actuelle du centre de l'objet.
  QVector<double> ccenter;
  // la tolerance a la detection de la collision
  double tolerance;

  // vide toutes les structures internes
  virtual void Clear();
  // cette methode contient les transformations geometriques OpenGL a l'objet
  virtual void glTransformations();
};

#endif
