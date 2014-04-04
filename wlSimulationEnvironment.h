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
  virtual ~wlSimulationEnvironment() {};
  /// Necessaire pour l'heritage de la classe wlCore.
  virtual char *GetClassName() {return "wlSimulationEnvironment";};

  /// \brief Determine si un objet est entre en collision avec l'environment.
  /// Trois situations peuvent se produire :
  /// <ul><li>il n'y a pas de collision et alors un tableau vide est renvoye,</li>
  ///     <li>il y a une collision mais celle-ci a ete detectee au dela de la tolerance admise, soit trop tard : le
  ///         tableau renvoye comporte alors une seule valeur, qui est a distance de laquelle l'objet s'est enfonce dans l'environnement,</li>
  ///     <li>il y a une collision et celle-ci est dans le perimetre tolere : le tableau renvoye comporte alors 6 valeurs, le point de contact et la normale en ce point.</li>
  /// </ul>
  /// Cette methode fait appel aux methodes de calcul de collision propres aux objets.
  virtual QVector<double> ComputeCollision(wlAnimatedMesh *am);

public slots:
  /// \brief Definit la tolerance pour la detection des collisions instantanees.
  /// On considerera que la collision est instantanee si la distance entre l'objet et l'environnement est inferieure a d.
  void SetTolerance(double d) {this->tolerance = d;};
  /// Affiche la valeur des principales variables.
  virtual void PrintSelf();
  /// Affiche le contenu des differents champs (en mode debug uniquement).
  virtual void PrintContent();

protected:
  // la tolerance a la detection de la collision
  double tolerance;
};

#endif
