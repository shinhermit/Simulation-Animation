///////////////////////////////////////////////////////////////////////////////
//  File         :    wlSimulationEnvironment.h                              //
//  Author       :    Wilfrid LEFER                                          //
//  Comment      :    Environnement de simulation pour le projet de SA       //
//  Related file :    wlSimulationEnvironment.cxx                            //
//                    (c) Wilfrid Lefer - UPPA - 2010                        //
///////////////////////////////////////////////////////////////////////////////


#ifndef _WL_SIMULATION_ENVIRONMENT_H
#define _WL_SIMULATION_ENVIRONMENT_H

#include <wlMesh.h>
#include <wlAnimatedMesh.h>

/** \brief wlSimulationEnvironment represente un environnement de simulation pour le projet de SA.

    Un environnement de simulation designe typiquement en un ensemble d'obstacles fixes.
    Cette classe doit implementer des methodes de detection de collision avec les objets animes de la scene
    et etre capable de se visualiser.
*/
class wlSimulationEnvironment : public wlMesh
{
  Q_OBJECT

public:
  /// Instancie un nouveau environnement de simulation.
  wlSimulationEnvironment(int debug=0);
  /// Destructeur.
  virtual ~wlSimulationEnvironment() {}
  /// Necessaire pour l'heritage de la classe wlCore.
  virtual char *GetClassName() {return "wlSimulationEnvironment";}
};

#endif
