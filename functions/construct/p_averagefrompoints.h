#ifndef P_AVERAGEFROMPOINTS_H
#define P_AVERAGEFROMPOINTS_H

#include <QObject>
#include <QPointer>

#include "constructfunction.h"

using namespace oi;

/*!
 * \brief The AverageFromPoints class
 *
 * D15/D15a (domain-model.md S2): how repeated shots of one physical target
 * combine. "One aiming event = one new Point, always. There is no
 * re-measure mode." - combining several raw Points into one is an
 * ordinary derived Point with a declared function, not a hidden
 * acquisition mode:
 *
 *   Tracker dies mid-job, total station comes out. Four nests x three
 *   sets = 12 raw Points, one observation each. Four derived Points, each
 *   averaging its three. The plane still fits from four points.
 *
 * Same shape as LineFromPlaneNormal/PointFromPlaneOrigin (D8): an ordinary
 * construct function on a user-created feature, consuming as many Points
 * as tag resolution (D12) or hand-picking gives it. Unlike those two, its
 * input is infinite - repeated shots of the same target, however many
 * there are, not a single fixed reference.
 *
 * Deliberately a plain arithmetic mean of solved input points' positions,
 * not a weighted least-squares adjustment - that is BestFitPoint's job
 * (over raw Observations, the one bridge from readings into geometry,
 * D13). This function's job is combining already-measured Points, which
 * is a different, simpler operation.
 */
class AverageFromPoints : public ConstructFunction
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

private:

    //##############
    //helper methods
    //##############

    bool setUpResult(Point &point);

};

#endif // P_AVERAGEFROMPOINTS_H
