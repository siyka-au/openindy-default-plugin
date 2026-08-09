#ifndef SIMULATEDOBSERVATION_H
#define SIMULATEDOBSERVATION_H

#include <cmath>

#include "trackererrormodel.h"
#include "oivec.h"
#include "oimat.h"
#include "reading.h"
#include "observation.h"

using namespace oi;
using namespace oi::math;

/*!
 * The scene / station-pose / inverse-geometry layer Stage 2 needed to make
 * "a driver reports believable accuracy" an actually testable claim, rather
 * than something only checkable by eye against real hardware.
 *
 * Originally lived only in tst_simulatedscene.cpp; promoted here once Stage
 * 4 became the second real consumer (pose-solving from simulated total
 * station observations of a constellation), as tst_simulatedscene.cpp's own
 * comment anticipated. Still deliberately test-scoped: real production code
 * (Stage 5's Leica driver, or a future total-station driver) computes
 * accuracy from its own calibration data, it does not resample a synthetic
 * error model.
 */

//! world-frame position and orientation of a simulated total station.
//! orientation rotates a world-frame vector into the station-local frame.
struct StationPose{
    OiVec position{3};
    OiMat orientation{3, 3};
};

//the true (noise-free) polar observation a station at `pose` would make of
//a point at `sceneXyz` - same spherical convention Reading::toPolar() uses
//(azimuth from +x, zenith from +z), so results compose with the rest of the
//codebase's polar/cartesian handling without a second convention to track
inline ReadingPolar trueObservationOf(const OiVec &sceneXyz, const StationPose &pose){

    OiVec relative(3);
    relative.setAt(0, sceneXyz.getAt(0) - pose.position.getAt(0));
    relative.setAt(1, sceneXyz.getAt(1) - pose.position.getAt(1));
    relative.setAt(2, sceneXyz.getAt(2) - pose.position.getAt(2));

    const OiVec local = pose.orientation * relative;

    const double x = local.getAt(0);
    const double y = local.getAt(1);
    const double z = local.getAt(2);

    ReadingPolar result;
    result.azimuth = std::atan2(y, x);
    result.distance = std::sqrt(x * x + y * y + z * z);
    result.zenith = std::acos(z / result.distance);
    result.isValid = true;
    return result;

}

//one simulated observation of a scene point: apply the error model to the
//true polar observation, then reuse Reading's own polar->cartesian
//propagation (the same Jacobian PseudoTracker now relies on) to get xyz and
//sigmaXyz consistently, rather than a third hand-rolled conversion
inline QPointer<Observation> simulateObservation(const OiVec &sceneXyz, const StationPose &pose,
                                                  const TrackerErrorModel &model, int id){

    const ReadingPolar trueReading = trueObservationOf(sceneXyz, pose);
    const ReadingPolar noisy = model.apply(trueReading.azimuth, trueReading.zenith, trueReading.distance);

    const Reading asCartesian(noisy);
    const ReadingCartesian &local = asCartesian.getCartesianReading();

    //station-local cartesian -> scene/world cartesian: undo the orientation
    //and re-add the station position
    OiVec localXyz(3);
    localXyz.setAt(0, local.xyz.getAt(0));
    localXyz.setAt(1, local.xyz.getAt(1));
    localXyz.setAt(2, local.xyz.getAt(2));
    const OiVec worldRelative = pose.orientation.t() * localXyz;

    OiVec worldXyz(4);
    worldXyz.setAt(0, worldRelative.getAt(0) + pose.position.getAt(0));
    worldXyz.setAt(1, worldRelative.getAt(1) + pose.position.getAt(1));
    worldXyz.setAt(2, worldRelative.getAt(2) + pose.position.getAt(2));
    worldXyz.setAt(3, 1.0);

    QPointer<Observation> observation = new Observation(worldXyz, id, true);
    observation->setIsSolved(true);

    //orientation is a pure rotation, so sigma magnitudes carry across
    //directly - only their alignment with the world axes changes, which
    //doesn't matter for the isotropic checks these tests make
    OiVec sigma(4);
    sigma.setAt(0, local.sigmaXyz.getAt(0));
    sigma.setAt(1, local.sigmaXyz.getAt(1));
    sigma.setAt(2, local.sigmaXyz.getAt(2));
    sigma.setAt(3, 1.0);
    observation->setSigmaXyz(sigma);

    return observation;

}

#endif // SIMULATEDOBSERVATION_H
