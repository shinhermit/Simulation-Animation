///////////////////////////////////////////////////////////////////////////////
//  File         :    wlSimulationEnvironment.cxx                            //
//  Author       :    Wilfrid LEFER                                          //
//  Comment      :    Environnement de simulation pour le projet de SA       //
//  Related file :    wlSimulationEnvironment.h                              //
//                    (c) Wilfrid Lefer - UPPA - 2010                        //
///////////////////////////////////////////////////////////////////////////////


#include "wlSimulationEnvironment.h"

wlSimulationEnvironment::wlSimulationEnvironment(int debug)
  : wlMesh(debug)
{
  this->Trace("-> wlSimulationEnvironment()");
  wlMesh::Clear();
  this->Trace("<- wlSimulationEnvironment()");
}
