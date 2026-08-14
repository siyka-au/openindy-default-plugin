#ifndef DISTANCEBETWEENTWOPOINTS_H
#define DISTANCEBETWEENTWOPOINTS_H

#include "constructfunction.h"

using namespace oi;

class DistanceBetweenTwoPoints : public ConstructFunction, public Producer<ScalarEntityDistance>
{
    Q_OBJECT
public:
    void init() override;

    std::expected<ScalarEntityDistance, QString> exec() override;

};

#endif // DISTANCEBETWEENTWOPOINTS_H
