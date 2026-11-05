#ifndef P_BESTFITSPHERE_H
#define P_BESTFITSPHERE_H

#include <QObject>
#include <QPointer>
#include <QtMath>

#include "fitfunction.h"
#include "oimat.h"
#include "oivec.h"

using namespace oi;

/*!
 * \brief The BestFitSphere class
 */
class BestFitSphere : public FitFunction, public Producer<Sphere> {
  Q_OBJECT

 public:
  // ##############################
  // function initialization method
  // ##############################

  void init();

  // ############
  // exec methods
  // ############

  std::expected<Sphere, QString> exec() override;

 private:
  // ##############
  // helper methods
  // ##############

  bool setUpResult(Sphere& sphere);
  bool approximate(Sphere& sphere, const QList<QPointer<Point> >& inputPoints);
  bool fit(Sphere& sphere, const QList<QPointer<Point> >& inputPoints,
           const QList<QPointer<Point> >& allUsablePoints);
};

#endif  // P_BESTFITSPHERE_H
