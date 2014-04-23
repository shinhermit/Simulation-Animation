///////////////////////////////////////////////////////////////////////////////
//  File         :    si.cxx                                                 //
//  Author       :    Wilfrid LEFER                                          //
//  Comment      :    Projet de SA                                           //
//  Related file :    saProject.{h,cxx}, wlAnimatedMesh.{h,cxx}              //
//                    Master Technologies de l'Internet - SA - 2010          //
///////////////////////////////////////////////////////////////////////////////


#include <QApplication>

#include "Project.h"
#include "sa.h"

#define USAGE(cmd) { \
  fprintf(stderr, "Usage: %s [-debug]\n", cmd); \
  exit(-1); \
}

int
main(int argc, char *argv[])
{
  int debug=0,
          size[2]={800,600},
          nbItems = 27;

  // Get command line parameters
  parseCmd(argc, argv, debug, size, nbItems);

  // l'application Qt
  QApplication app(argc, argv);

  Project * project = new Project(nbItems, debug);
  project->show();

  return app.exec();
}

void parseCmd(const int & argc, char * argv[], int & debug, int * size, int & nbItems)
{
    for (int i=1 ; i<argc ; i++)
      if (!strcmp(argv[i], "--debug") || !strcmp(argv[i], "-d"))
        debug = 1;
      else if (!strcmp(argv[i], "--window-size") || !strcmp(argv[i], "-w"))
      {
        if (sscanf(argv[++i], "%dx%d", &size[0], &size[1]) != 2)
          USAGE(argv[0])
      }
      else if (!strcmp(argv[i], "--nb-particles") || !strcmp(argv[i], "-n"))
      {
        if (sscanf(argv[++i], "%d", &nbItems) != 1)
          USAGE(argv[0])
      }
      else USAGE(argv[0]);
}
