///////////////////////////////////////////////////////////////////////////////
//  File         :    wlAnimatedSphere.h                                     //
//  Author       :    Wilfrid LEFER                                          //
//  Comment      :    Modele de sphere animee                                //
//  Related file :    wlAnimatedSphere.cxx, wlAnimatedMesh.{h,cxx}           //
//                    Wilfrid Lefer - UPPA - 2010                            //
///////////////////////////////////////////////////////////////////////////////


#ifndef _WL_ANIMATED_SPHERE_H
#define _WL_ANIMATED_SPHERE_H

#include <wlAnimatedMesh.h>

/** \brief wlAnimatedSphere est une classe pour l'animation d'une sphere.
*/
class wlAnimatedSphere : public wlAnimatedMesh
{
  Q_OBJECT

public:
  /// \brief Cree une nouvelle sphere animee.
  /// La sphere est eventuellement initialisee a partir du contenu du fichier.
  wlAnimatedSphere(int debug=0, wlQGLViewer *const v=NULL, QString filename=QString());
  virtual ~wlAnimatedSphere() {};
  virtual char *GetClassName() {return "wlAnimatedSphere";};

  QVector<double> ComputeCollisionWithOther(wlAnimatedMesh *other);
  QVector<double> ComputeCollisionWithPlan(QVector<double> equation);
  void ComputeReaction(QVector<double> impact);

  /// A partir du moment ou on sait avoir affaire a une sphere, on peut affiner ce calcul, qui est inexact dans le cas d'une mesh quelconque.
  float GetRadius();

public slots:
  /// Imprime le contenu de la mesh a la resolution courante : coordonnees des sommets, aretes et faces.
  virtual void PrintSelf();
  /// Imprime le contenu detaille de la mesh a la resolution courante : contenu des differentes tables.
  virtual void PrintContent();
};

#endif
