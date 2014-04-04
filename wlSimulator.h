///////////////////////////////////////////////////////////////////////////////
//  File         :    wlSimulator.h                                          //
//  Author       :    Wilfrid LEFER                                          //
//  Comment      :    Simulateur pour le projet de SA                        //
//  Related file :    wlSimulator.cxx                                        //
//                    (c) Wilfrid Lefer - UPPA - 2010                        //
///////////////////////////////////////////////////////////////////////////////


#ifndef _WL_SAMULATOR_H
#define _WL_SAMULATOR_H

#include <stdexcept>
#include <cmath>

#include <QObject>
#include <QTimer>
#include <saViewer.h>
#include <wlAnimatedMesh.h>
#include <wlSimulationEnvironment.h>

#include "qclcontext.h"

/** \brief wlSimulator est la classe qui pilote l'animation dans les projets de SA.
*/
class wlSimulator : public QObject, public wlCore
{
    Q_OBJECT

public:
    /// Instancie un nouveau simulateur.
    /// <em>viewer</em> designe le viewer OpenGL associe a l'environnement.
    /// <em>environment</em> designe l'environnement de la simulation, qui doit etre capable notamment de detecter des collisions ou de faire son propre rendu.
    /// <em>items</em> designe les objets animes.
    wlSimulator(int debug=0, saViewer *viewer=NULL,
                wlSimulationEnvironment *environment=NULL,
                QVector<wlAnimatedMesh *> * items=NULL);
    /// Destructeur.
    virtual ~wlSimulator() {}
    /// Necessaire pour l'heritage de la classe wlCore.
    virtual char *GetClassName() {return "wlSimulator";};

    /// \brief Associe un nouvel environnement.
    virtual void SetEnvironment(wlSimulationEnvironment *env);
    /// \brief Définit un environnement OpenCl
    virtual void setOpenClContext(QCLContext * openClContext=NULL,
                                  QCLVector<float> * openClInput=NULL);
    /// \brief Enregistre un objet supplementaire.
    virtual void AddItem(wlAnimatedMesh *item);
    /// \brief Supprime tous les objets animes.
    virtual void ClearItems();
    /// \brief Renvoie vrai si un environnement de simulation existe.
    /// L'environnement de simulation designe typiquement en un ensemble d'obstacles fixes.
    virtual bool HasEnvironment() {return this->environment != NULL;};

signals:

public slots:
    /// \brief Definit le type de reaction utilisee pour gerer le rebond.
    virtual void SetReaction(int index);
    /// \brief Definit le coefficient d'attenuation du rebond lors d'une reaction cinematique pure.
    virtual void SetAttenuationCoefficientForPureKinematicReaction(double k);
    /// \brief Definit le coefficient d'attenuation du rebond lors d'une reaction cinematique pure.
    virtual void SetAttenuationCoefficientForPureKinematicReaction(QString t);
    /// \brief Definit la resistance des ressorts utilises lors d'une reaction a handicap.
    virtual void SetSpringCoefficientForPenaltyReaction(double Ks);
    /// \brief Definit la resistance des ressorts utilises lors d'une reaction a handicap.
    virtual void SetSpringCoefficientForPenaltyReaction(QString t);
    /// \brief Definit la masse associee aux points lors d'une reaction a handicap.
    virtual void SetPointWeightForPenaltyReaction(double m);
    /// \brief Definit la masse associee aux points lors d'une reaction a handicap.
    virtual void SetPointWeightForPenaltyReaction(QString t);

    /// \brief Definit le nombre de pas de temps de l'animation.
    void SetNumberOfTimeSteps(int n);
    /// \brief Definit le nombre de pas de temps de l'animation.
    void SetNumberOfTimeSteps(QString n);
    /// \brief Definit le pas de temps de l'animation.
    void SetTimeStep(double t);
    /// \brief Definit le pas de temps pour le calcul de l'animation.
    virtual void SetTimeStep(QString t);

    /// \brief Reinitialise l'animation.
    virtual void Reset();
    /// \brief Execute un pas de temps de l'animation.
    virtual void Step();
    /// \brief Lance l'animation.
    virtual void Play();
    /// \brief Stoppe l'animation en cours.
    virtual void Stop();

    virtual void draw();

    /// Affiche la valeur des principales variables.
    virtual void PrintSelf();
    /// Affiche le contenu des differents champs (en mode debug uniquement).
    virtual void PrintContent();

protected:
    // L'environnement dans le quel se deroule la simulation.
    wlSimulationEnvironment *environment;
    // La liste des objets animes.
    QVector<wlAnimatedMesh *> & items;
    // Le viewer OpenGL.
    saViewer *viewer;
    // la duree d'un pas de temps de la simulation
    double timestep;
    // la duree du pas de temps courant (peut etre different si une collision est intervenue)
    double ctimestep;
    // le nombre de pas de temps de la simulation
    int nsteps;
    // le pas de temps courant
    int cstep;
    // le timer pour le temps reel
    QTimer *timer;
    // le type de reaction utilisee pour gerer les rebonds
    int reaction;
    // le coefficient d'attenuation du rebond lors d'une reaction cinematique pure
    double k;
    // la resistance des ressorts utilises lors d'une reaction a handicap
    double Ks;
    // la masse associee aux points lors d'une reaction a handicap
    double m;

    // vide toutes les structures internes
    virtual void Clear();

private:
    void ShowEntireMesh();
};

#endif
