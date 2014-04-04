#ifndef JACOLLISIONPROCESSOR_H
#define JACOLLISIONPROCESSOR_H

#include <QVector>;
#include "wlSimulator.h"
#include "wlAnimatedMesh.h"
#include "wlSimulationEnvironment.h"

namespace ja
{

class CollisionProcessor
{
public:
    CollisionProcessor(wlSimulator * simulator);

    void process();

private:
    void _process(const double & instant1, const double & instant2);
    void _detect(const double & instant1, const double & instant2);
    void _convert();
};

}

#endif // JACOLLISIONPROCESSOR_H
