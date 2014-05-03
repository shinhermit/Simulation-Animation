///////////////////////////////////////////////////////////////////////////////
//  File         :    si.cxx                                                 //
//  Author       :    Wilfrid LEFER                                          //
//  Comment      :    Projet de SA                                           //
//  Related file :    saProject.{h,cxx}, wlAnimatedMesh.{h,cxx}              //
//                    Master Technologies de l'Internet - SA - 2010          //
///////////////////////////////////////////////////////////////////////////////


#include <QApplication>

#include "Project.h"
#include "DefaultParameters.h"
#include "sa.h"

int
main(int argc, char *argv[])
{
  int size[2]= {DefaultParameters::WindowSize[0], DefaultParameters::WindowSize[1]},
          nbItems = DefaultParameters::NbParticles;

  // Get command line parameters
  parseCmd(argc, argv, size, nbItems);

  // l'application Qt
  QApplication app(argc, argv);

  Project * project = new Project(nbItems);
  project->setViewSize(size[0], size[1]);
  project->show();

  return app.exec();
}

void parseCmd(const int & argc, char * argv[], int * size, int & nbItems)
{
    for (int i=1 ; i<argc ; i++)
    {
        if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h"))
          usage(argv[0], false);
      else if (!strcmp(argv[i], "--window-size") || !strcmp(argv[i], "-w"))
      {
        if (sscanf(argv[++i], "%d", &size[0]) != 1 ||
            sscanf(argv[++i], "%d", &size[1]) != 1)
          usage(argv[0]);
      }
      else if (!strcmp(argv[i], "--nb-particles") || !strcmp(argv[i], "-n"))
      {
        if (sscanf(argv[++i], "%d", &nbItems) != 1)
          usage(argv[0]);
      }
      else usage(argv[0]);
    }
}

void usage(char * cmd, const bool & terminate)
{
    fprintf(stderr, "Usage: %s [-h|--help] [-n|--nb-particles nb]\n", cmd);

    if(terminate)
        exit(-1);
}
