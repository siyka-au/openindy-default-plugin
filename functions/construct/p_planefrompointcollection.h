#ifndef P_PLANEFROMPOINTCOLLECTION_H
#define P_PLANEFROMPOINTCOLLECTION_H

#include <QObject>
#include <QPointer>
#include <QtMath>

#include "constructfunction.h"
#include "fitfunction.h"
#include "oivec.h"
#include "oimat.h"

using namespace oi;

/*!
 * \brief The PlaneFromPointCollection class
 * Fits a plane to as many Points as are tagged for it (D12), rather than
 * to a bound PointCollection.
 *
 * Stage 7f retires PointCollection as a fit-input mechanism: tagging solves
 * the same binding-friction problem PointCollection existed for, and solves
 * it better. This function's declared shape (one NeededElement, infinite)
 * never changes regardless of how many points are tagged for it - the
 * input list still cannot grow "by construction", it is just resolved by
 * OiJob::resolvePointIntent matching MeasurementIntent{featureId, roleName}
 * against this NeededElement::roleName instead of by a bound collection
 * object. Automating a thousand reflectorless shots across a factory wall
 * still never touches this function: each shot arrives already tagged.
 *
 * Retained name (not renamed to e.g. "PlaneFromPoints", which already
 * exists as the ePointElement-single/non-infinite-input equivalent kept
 * from before this stage) so existing saved projects referencing this
 * function by name still resolve.
 *
 * The fit body is unchanged from before this stage (still the shared
 * BestFitPlaneUtil::bestFitPlane PCA/SVD solver) - only what feeds the
 * input differs.
 */
class PlaneFromPointCollection : public ConstructFunction, public BestFitPlaneUtil
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

    bool exec(Plane &plane);

private:

    //##############
    //helper methods
    //##############

    bool setUpResult(Plane &plane);

};

#endif // P_PLANEFROMPOINTCOLLECTION_H
