#ifndef P_PLANEFROMPLANE_H
#define P_PLANEFROMPLANE_H

#include <QObject>
#include <QPointer>

#include "constructfunction.h"
#include "oivec.h"
#include "oimat.h"

using namespace oi;

/*!
 * \brief The PlaneFromPlane class
 */
class PlaneFromPlane : public ConstructFunction, public Producer<Plane>
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

    std::expected<Plane, QString> exec() override;

private:

    //##############
    //helper methods
    //##############

    bool setUpResult(Plane &plane);

};

#endif // P_PLANEFROMPLANE_H
