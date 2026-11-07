#ifndef P_BESTFITSPHERE_H
#define P_BESTFITSPHERE_H
#include <QObject>
#include <QPointer>
#include <QtMath>
#include <vector>

#include "fitfunction.h"
#include "oimat.h"
#include "oivec.h"

using namespace oi;

/*!
 * \brief The BestFitSphere class
 */
class BestFitSphere : public FitFunction, public Producer<Sphere> {
 public:
  // ##############################
  // function initialization method
  // ##############################

  void init() override;

  // ############
  // exec methods
  // ############

  std::expected<Sphere, QString> exec() override;

 private:
  // ##############
  // helper methods
  // ##############

  bool setUpResult(Sphere& sphere);
  bool approximate(Sphere& sphere,
                   const std::vector<oi::Guarded<Point> >& inputPoints);
  bool fit(Sphere& sphere, const std::vector<oi::Guarded<Point> >& inputPoints,
           const std::vector<oi::Guarded<Point> >& allUsablePoints);
};

#endif  // P_BESTFITSPHERE_H
