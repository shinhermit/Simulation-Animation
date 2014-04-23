#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>

#include "wlCore.h"
#include "wlSimulationEnvironment.h"
#include "saViewer.h"
#include "AnimatedObject.h"

class Simulator : public QObject, public wlCore
{
    Q_OBJECT

public:
    /// Instancie un nouveau simulateur.
    /// <em>viewer</em> designe le viewer OpenGL associe a l'environnement.
    /// <em>environment</em> designe l'environnement de la simulation, qui doit etre capable notamment de detecter des collisions ou de faire son propre rendu.
    /// <em>items</em> designe les objets animes.
    Simulator(int debug=0, saViewer *viewer=NULL,
                wlSimulationEnvironment *environment=NULL,
                QVector<AnimatedObject*> * items=NULL);
    /// Destructeur.
    virtual ~Simulator();
    /// Necessaire pour l'heritage de la classe wlCore.
    virtual char *getClassName()const;

    /// \brief Associe un nouvel environnement.
    virtual void setEnvironment(wlSimulationEnvironment *env);
    /// \brief Enregistre un objet supplementaire.
    virtual void addItem(AnimatedObject * item);
    /// \brief Supprime tous les objets animes.
    virtual void clearItems();
    /// \brief Renvoie vrai si un environnement de simulation existe.
    /// L'environnement de simulation designe typiquement en un ensemble d'obstacles fixes.
    virtual bool hasEnvironment()const;

public slots:
    /// \brief Definit le nombre de pas de temps de l'animation.
    void setNumberOfTimeSteps(const int & nbSteps);
    /// \brief Definit le pas de temps de l'animation.
    void setTimeStep(const double & timestep);

    /// \brief Reinitialise l'animation.
    virtual void reset();
    /// \brief Replace les objets à leurs positions initiales
    virtual void restart();
    /// \brief Execute un pas de temps de l'animation.
    virtual void step();
    /// \brief Lance l'animation.
    virtual void play();
    /// \brief Stoppe l'animation en cours.
    virtual void stop();

    virtual void draw();

    /// Affiche la valeur des principales variables.
    virtual void printSelf();

protected:
    // L'environnement dans le quel se deroule la simulation.
    wlSimulationEnvironment *_env;
    // La liste des objets animes.
    QVector<AnimatedObject*> & _items;
    // Le viewer OpenGL.
    saViewer *_viewer;
    // la duree d'un pas de temps de la simulation
    float _timestep;
    // le nombre de pas de temps de la simulation
    int _nsteps;
    // le pas de temps courant
    int _cstep;
    // le timer pour le temps reel
    QTimer * _timer;

    // vide toutes les structures internes
    virtual void _clear();

private:
    void _showEntireMesh();
};

#endif // SIMULATOR_H
