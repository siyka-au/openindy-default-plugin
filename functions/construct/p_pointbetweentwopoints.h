#ifndef P_POINTBETWEENTWOPOINTS_H
#define P_POINTBETWEENTWOPOINTS_H

#include <QObject>
#include <QPointer>

#include "constructfunction.h"
#include "oivec.h"
#include "oimat.h"

using namespace oi;

/*!
 * \brief The PointBetweenTwoPoints class
 */
class PointBetweenTwoPoints : public ConstructFunction, public Producer<Point>
{
    Q_OBJECT

public:

    //##############################
    //function initialization method
    //##############################

    void init();

    //############
    //exec methods
    //############

    std::expected<Point, QString> exec() override;

private:

    //##############
    //helper methods
    //##############

    bool setUpResult(Point &point);

};

#endif // P_POINTBETWEENTWOPOINTS_H
