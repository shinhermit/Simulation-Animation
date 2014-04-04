///////////////////////////////////////////////////////////////////////////////
//  File          :   saProject.h                                            //
//  Author        :   Wilfrid LEFER                                          //
//  Comment       :   Interface pour le projet de SA                         //
//  Related files :   saProject.cxx, wlAnimatedMesh.{h,cxx}                  //
//                    Wilfrid Lefer - UPPA - 2010                            //
///////////////////////////////////////////////////////////////////////////////


#ifndef _SA_PROJECT_H
#define _SA_PROJECT_H

#include <stdexcept>

#include <wlSimulator.h>
#include <QMainWindow>
#include <QTextEdit>
#include <QLabel>
#include <QProgressBar>
#include <QBoxLayout>
#include <QDateTime>
#include <wlAnimatedSphere.h>

#include "qclcontext.h"

#include "ParticleSimulator.h"

// taille de la fenetre principale par defaut
#define XSAZE 800
#define YSAZE 800

/** \brief saProject realise l'interface du projet de SA.
*/
class saProject : public QMainWindow, public wlCore
{
  Q_OBJECT

public:
  /// Instancie une nouvelle application.
  saProject(QString title=QString::fromLocal8Bit("saProject", 11), QSize size=QSize(), int debug=0, bool gpuMode=false);
  virtual ~saProject() {};
  virtual char *GetClassName() {return "saProject";};

  /// \brief Charge une scene depuis un fichier.
  /// Le format d'un fichier scene est le suivant :
  /// <ul><li>une entete definissant les parametres globaux (il existe des valeurs par defaut), les token possibles etant :<ul>
  ///        <li>TIME_STEP &lt;double&gt;</li>
  ///        <li>NUMBER_OF_TIME_STEPS &lt;unsigned int&gt;</li>
  ///        <li>REACTION_TYPE &lt;PURE_KINEMATIC | PENALTY&gt;</li>
  ///        <li>ATTENUATION_COEFFICIENT_FOR_PURE_KINEMATIC_REACTION &lt;double&gt;</li>
  ///        <li>SPRING_COEFFICIENT_FOR_PENALTY_REACTION &lt;double&gt;</li>
  ///        <li>POINT_WEIGTH_FOR_PENALTY_REACTION &lt;double&gt;</li>
  ///     </ul></li>
  ///     <li>une liste d'objets fixes (sous-classes de <em>wlSimulationEnvironment</em>) ou animes (sous-classes de <em>wlAnimationMesh</em>), les tokens possibles etant :<ul>
  ///        <li>GROUND &lt;double&gt; &lt;double&gt; &lt;double&gt; &lt;double&gt; &lt;double&gt; &lt;double&gt; # necessairement perpendiculaire a l'axe Oz, donnees des coins inferieur et superieur</li>
  ///        <li>CAGE &lt;double&gt; &lt;double&gt; &lt;double&gt; &lt;double&gt; &lt;double&gt; &lt;double&gt; &lt;double&gt; # le fond de la cage est necessairement perpendiculaire a l'axe Oz et ses bords verticaux, les parametres sont les coins inferieur et superieur du sol et la hauteur de la cage</li>
  ///        <li>SPHERE &lt;double&gt; &lt;double&gt; &lt;double&gt; &lt;double&gt; &lt;double&gt; &lt;double&gt; &lt;double&gt; # les parametres sont, dans l'ordre, la position puis le rayon de la sphere, le vecteur vitesse initiale</li>
  ///     <ul></li>
  ///     <li>des eventuelles lignes de commentaires pouvant apparaitre n'importe ou mais devant necessairement avoir le caractere # sur la premiere colonne.</li>
  /// </ul>
  /// Imprime un message dans la console de l'application.
  void Message(QString text);
  /// Recupere la barre de statut.
  inline QStatusBar *GetStatusBar() {return this->sb;};
  /// Demarre la barre de progression dans la barre de status (a appeler au debut d'un calcul
  /// pour lequel on souhaite une barre de progression). "nsteps" est le nombre total de pas
  /// de temps du calcul.
  void Start(int nsteps);
  /// Progresse la barre de progression d'un pas de temps pour le calcul en cours.
  void Step();

private:
  // Objets Qt.
  QTextEdit *console;
  QStatusBar *sb;
  // La barre de progression.
  QProgressBar *progress;
  // Le container pour la gestion des collisions
  QHBoxLayout *reaction;
  // Le parametrage de la reaction de type cinematique pure
  QWidget *reaction_1;
  // Le parametrage de la reaction de type handicap
  QWidget *reaction_2;
  // Nombre de pas de temps completes pour le processus en cours de progression.
  int ns;
  // Temps d'execution total de la derniere execution mesuree.
  QTime *exectime;
  // Le simulateur.
  wlSimulator *simulator;
  // Les objets animes.
  QVector<wlAnimatedMesh *> balls;

  //*********** OpenCl *********
  // Switch to enable GPU computing
  bool gpuMode;

  // Contexte GPU
  QCLContext openClContext;

  //Pour transmettre les données au GPU
  // Vecteur de positions et vitesses
  // chaque particule est représentée par
  // 6 valeurs contigues <px,py,pz,vx,vy,vz>
  QCLVector<float> openClInput;

private slots:
  /// Slot : lit une nouvelle mesh dans un fichier.
  void Open();
  /// Slot : change le parametrage de la reaction en fonction du type de reaction choisie
  void SetReaction(int index);
};

#endif
