///////////////////////////////////////////////////////////////////////////////
//  File         :    si.cxx                                                 //
//  Author       :    Wilfrid LEFER                                          //
//  Comment      :    Projet de SA                                           //
//  Related file :    saProject.{h,cxx}, wlAnimatedMesh.{h,cxx}              //
//                    Master Technologies de l'Internet - SA - 2010          //
///////////////////////////////////////////////////////////////////////////////


#include <saProject.h>
#include <QApplication>

#include "Project.h"

#define USAGE(cmd) { \
  fprintf(stderr, "Usage: %s [-debug]\n", cmd); \
  exit(-1); \
}

int
main(int argc, char *argv[])
{
  int debug=0, size[2]={800,600};

  for (int i=1 ; i<argc ; i++)
    if (!strcmp(argv[i], "-debug"))
      debug = 1;
    else if (!strcmp(argv[i], "-size"))
    {
      if (sscanf(argv[++i], "%dx%d", &size[0], &size[1]) != 2)
        USAGE(argv[0])
    }
    else USAGE(argv[0]);

  // l'application Qt
  QApplication app(argc, argv);

//  saProject *project = new saProject(QString::fromLocal8Bit("Projet de Simulation & Animation - annee 2011 - Chute d'une balle avec collisions", 81), QSize(size[0], size[1]), debug);
//  project->move(0, 0);
//  project->show();

  Project * project = new Project(27, debug);
  project->show();

  return app.exec();
}


