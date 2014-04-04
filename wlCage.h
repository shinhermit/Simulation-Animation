///////////////////////////////////////////////////////////////////////////////
//  File         :    wlCage.h                                               //
//  Author       :    Wilfrid LEFER                                          //
//  Comment      :    Environnement de type cage pour le projet de SA        //
//  Related file :    wlCage.cxx                                             //
//                    (c) Wilfrid Lefer - UPPA - 2011                        //
///////////////////////////////////////////////////////////////////////////////


#ifndef _WL_CAGE_H
#define _WL_CAGE_H

#include <wlSimulationEnvironment.h>

/** \brief wlCage represente une cage fermee pour le projet de SA.
*/
class wlCage : public wlSimulationEnvironment
{
  Q_OBJECT

public:
  /// Instancie un nouvel environnement de type cage.
  wlCage(int debug=0);
  /// Destructeur.
  virtual ~wlCage() {};
  /// Necessaire pour l'heritage de la classe wlCore.
  virtual char *GetClassName() {return "wlCage";};

  QVector<double> ComputeCollision(wlAnimatedMesh *am);

public slots:

protected:
  QVector<QVector<double> > equations;
};

#endif
