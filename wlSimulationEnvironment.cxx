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
  this->tolerance = 0.01;
  this->Trace("<- wlSimulationEnvironment()");
}

void
wlSimulationEnvironment::PrintSelf()
{
  this->Print("tolerance : %lf", this->tolerance);
}

void
wlSimulationEnvironment::PrintContent()
{
  this->PrintSelf();
}

QVector<double>
wlSimulationEnvironment::ComputeCollision(wlAnimatedMesh *am)
{
  if (am == NULL)
    this->Error("ComputeCollision: pointeur sur mesh null");
  return QVector<double>();
}
