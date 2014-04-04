///////////////////////////////////////////////////////////////////////////////
//  File         :    wlGround.cxx                                           //
//  Author       :    Wilfrid LEFER                                          //
//  Comment      :    Environnement de type sol pour le projet de SA         //
//  Related file :    wlGround.h                                             //
//                    (c) Wilfrid Lefer - UPPA - 2010                        //
///////////////////////////////////////////////////////////////////////////////


#include "wlGround.h"

// une mesh pour representer un parterre a Z=0
float ground_vertices[][5] = { {-20,-20,0,0,0}, {20,-20,0,1,0}, {20,20,0,1,1}, {-20,20,0,0,1}, {0,0,-1000,0,0} };
int ground_triangles[][3] = { {0,1,3}, {1,2,3}, {4,1,0}, {4,2,1}, {4,3,2}, {4,0,3} };
int ground_nv=5, ground_nt=6;

wlGround::wlGround(int debug)
  : wlSimulationEnvironment(debug)
{
  this->Trace("-> wlGround()");
  std::vector<std::vector<float> > verts;
  std::vector<float> vert(5);
  for (int i=0 ; i<ground_nv ; i++) {
    for (int j=0 ; j<5 ; j++)
      vert[j] = ground_vertices[i][j];
    verts.push_back(vert);
  }
  std::vector<std::vector<int> > faces;
  std::vector<int> face(3);
  for (int i=0 ; i<ground_nt ; i++) {
    for (int j=0 ; j<3 ; j++)
      face[j] = ground_triangles[i][j];
    faces.push_back(face);
  }
  this->InitMesh(verts, faces, 1, 4);
  this->SetTexture(QImage("Sand00.png"));
  this->Trace("<- wlGround()");
}

QVector<double>
wlGround::ComputeCollision(wlAnimatedMesh *am)
{
    if (am == NULL) {
      this->Error("ComputeCollision: pointeur sur mesh null");
      return QVector<double>();
    }
    QVector<double> equation;
    equation << 0 << 0 << 0 << 0 << 0 << 1;
    return am->ComputeCollisionWithPlan(equation);
}
