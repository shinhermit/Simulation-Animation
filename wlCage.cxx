///////////////////////////////////////////////////////////////////////////////
//  File         :    wlCage.cxx                                             //
//  Author       :    Wilfrid LEFER                                          //
//  Comment      :    Environnement de type cage pour le projet de SA        //
//  Related file :    wlCage.h                                               //
//                    (c) Wilfrid Lefer - UPPA - 2011                        //
///////////////////////////////////////////////////////////////////////////////


#include "wlCage.h"

// une mesh pour representer une cage posse sur le sol
// l'astuce pour la visibilite consiste a orienter tous les bords vers l'interieur
float cage_vertices[][5] = { {-20,-20,0,0,0}, {20,-20,0,1,0}, {20,20,0,1,1}, {-20,20,0,0,1}, {-20,-20,40,1,1}, {20,-20,40,0,1}, {20,20,40,0,0}, {-20,20,40,1,0}, {0,0,100,0,0} };
int cage_triangles[][3] = { {0,1,3}, {1,2,3}, {4,5,0}, {0,5,1}, {1,6,2}, {1,5,6}, {0,3,7}, {0,7,4}, {3,2,6}, {3,6,7}, {8,6,7}, {8,6,5}, {8,5,4}, {8,4,7}};
int cage_nv=8, cage_niv=1, cage_nt=10, cage_nit=4;

wlCage::wlCage(int debug)
  : wlSimulationEnvironment(debug)
{
  this->Trace("-> wlCage()");
  std::vector<std::vector<float> > verts;
  std::vector<float> vert(5);
  for (int i=0 ; i<cage_nv+cage_niv ; i++) {
    for (int j=0 ; j<5 ; j++)
      vert[j] = cage_vertices[i][j];
    verts.push_back(vert);
  }
  std::vector<std::vector<int> > faces;
  std::vector<int> face(3);
  for (int i=0 ; i<cage_nt+cage_nit ; i++) {
    for (int j=0 ; j<3 ; j++)
      face[j] = cage_triangles[i][j];
    faces.push_back(face);
  }
  this->InitMesh(verts, faces, cage_niv, cage_nit);
  this->SetTexture(QImage("Sand00.png"));
  // on prepare l'equation des plans pour les tests d'intersection
  QVector<double> equation;
  equation << 0 << 0 << 0 << 0 << 0 << 1;
  this->equations << equation;
  equation[0] = -20;
  equation[3] = 1;
  equation[5] = 0;
  this->equations << equation;
  equation[0] = 20;
  equation[3] = -1;
  this->equations << equation;
  equation[1] = -20;
  equation[3] = 0;
  equation[4] = 1;
  this->equations << equation;
  equation[1] = 20;
  equation[4] = -1;
  this->equations << equation;
  this->Trace("<- wlCage()");
}

QVector<double>
wlCage::ComputeCollision(wlAnimatedMesh *am)
{
  if (am == NULL) {
    this->Error("ComputeCollision: pointeur sur mesh null");
    return QVector<double>();
  }
  return QVector<double>();
}
