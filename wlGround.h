///////////////////////////////////////////////////////////////////////////////
//  File         :    wlGround.h                                             //
//  Author       :    Wilfrid LEFER                                          //
//  Comment      :    Environnement de type sol pour le projet de SA         //
//  Related file :    wlGround.cxx                                           //
//                    (c) Wilfrid Lefer - UPPA - 2010                        //
///////////////////////////////////////////////////////////////////////////////


#ifndef _WL_GROUND_H
#define _WL_GROUND_H

#include <wlSimulationEnvironment.h>

/** \brief wlGround represente un sol a z=0 pour le projet de SA.
*/
class wlGround : public wlSimulationEnvironment
{
  Q_OBJECT

public:
  /// Instancie un nouvel environnement de type sol.
  wlGround(int debug=0);
  /// Destructeur.
  virtual ~wlGround() {};
  /// Necessaire pour l'heritage de la classe wlCore.
  virtual char *GetClassName() {return "wlGround";};

  QVector<double> ComputeCollision(wlAnimatedMesh *am);

public slots:

protected:
};

#endif
