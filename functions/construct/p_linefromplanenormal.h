#ifndef P_LINEFROMPLANENORMAL_H
#define P_LINEFROMPLANENORMAL_H

#include <QObject>
#include <QPointer>

#include "constructfunction.h"

using namespace oi;

/*!
 * \brief The LineFromPlaneNormal class
 * Creates a line along a plane's normal, through the plane's own position.
 *
 * This is the shape D8 settles on for derived geometry. A fit produces a
 * plane; the plane's normal and origin are real, useful features in their
 * own right, and every metrology package needs them. The alternative -
 * letting a fit function spontaneously emit several features at once -
 * inverts the direction everything else in this codebase works in (define
 * a feature, then choose the function that defines it), and is harder to
 * persist, recalculate and reason about.
 *
 * So instead: the user creates a Line and gives it this function. The line
 * is then a normal dependent of the plane in the existing dependency graph,
 * recalculates when the plane changes, and needs no new machinery at all.
 * That is the answer to backfilling derived geometry by hand.
 */
class LineFromPlaneNormal : public ConstructFunction
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

    bool exec(Line &line);

private:

    //##############
    //helper methods
    //##############

    bool setUpResult(Line &line);

};

#endif // P_LINEFROMPLANENORMAL_H
