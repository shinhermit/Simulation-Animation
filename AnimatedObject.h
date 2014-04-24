#ifndef ANIMATEDOBJECT_H
#define ANIMATEDOBJECT_H

#include <QObject>
#include <QVector>

#include "wlCore.h"
#include "DefaultParameters.h"

class AnimatedObject : public QObject, public wlCore
{
    Q_OBJECT

public:
    AnimatedObject(int debug=0);

    virtual ~AnimatedObject();
    virtual char *getClassName()const;

  public slots:
    /// \brief Definit le pas de temps de la simulation
    virtual void setTimeStep(const float & timestep);

    /// \brief Definit la position initiale de l'objet.
    virtual void setInitialPosition(const float & x, const float & y, const float & z);
    /// \brief Definit la position initiale de l'objet.
    virtual void setPosition(const float & x, const float & y, const float & z);

    /// \brief Definit la position initiale de l'objet.
    virtual void setInitialVelocity(const float & vX, const float & vY, const float & vZ);
    /// \brief Definit la vitesse instantanee de la mesh.
    virtual void setVelocity(const float & vX, const float & vY, const float & vZ);
    /// \brief Incrémente la compteur local de pas de temps. Utile après un retour de calcul en GPU.
    virtual void newStep();

    /// \brief Renvoie la vitesse actuelle du centre de gravite de la mesh au format (<x>, <y>, <z>).
    virtual QVector<float> getVelocity()const;

    /// \brief Obtient la position courante du mesh, wich is initial position plus translation
    virtual QVector<float> getPosition()const;
    /// \brief Obtient le pas de temps de la simulation
    virtual float getTimeStep()const;
    /// \brief Obtient le pas de temps de la simulation
    virtual float getCurrentStep()const;

    /// Imprime le contenu de la mesh a la resolution courante : coordonnees des sommets, aretes et faces.
    virtual void printSelf();

    /// \brief Ramene l'objet dans sa configuration initiale.
    virtual void reset();
    /// \brief Execute un pas de temps de l'animation.
    virtual void step();

  protected:
    // Paramètres initiaux
    float _timestep; /*!< la durée d'un pas de temps */

    QVector<float> _initPos; /*!< la position initiale */
    QVector<float> _initVel; /*!< la vitesse initiale */

    // Propriétés du pas de temps
    int _cstep; /*!< le pas de temps courant*/
    QVector<float> _cvel; /*!< la vitesse au pas de temps courant */
    QVector<float> _tVec; /*!< le vecteur translation au pas de temps courant */

    /// \brief vide toutes les structures internes
    virtual void _clear();
};

#endif // ANIMATEDOBJECT_H
