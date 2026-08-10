#ifndef P_NOMINALPOINT_H
#define P_NOMINALPOINT_H

#include <QObject>
#include <QPointer>

#include "constructfunction.h"

using namespace oi;

/*!
 * \brief The NominalPoint class
 *
 * A nominal producer function (domain-model.md §4, D17): it takes only
 * hand-entered scalar parameters, declares no needed elements, and is
 * therefore never a fit. Geometry::getOwnerDescription relies on exactly
 * that distinction - no producer, or a producer that is not a
 * FitFunction, leaves the result user-owned and editable - so a Point
 * produced by this function is an ordinary editable nominal, not an
 * actual.
 *
 * Parametric nominals (a named job-level parameter table) are
 * deliberately out of scope here - this reads literal scalars only.
 * Tolerance is deferred too; there is nowhere to attach it yet.
 */
class NominalPoint : public ConstructFunction
{
    Q_OBJECT

public:

    //##############################
    //function initialization method
    //##############################

    void init();

protected:

    //############
    //exec methods
    //############

    bool exec(Point &point);

};

#endif // P_NOMINALPOINT_H
