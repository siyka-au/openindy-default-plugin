#ifndef P_REGISTER_H
#define P_REGISTER_H

#include <QObject>
#include <QPointer>

#include "objecttransformation.h"
#include "oimat.h"
#include "oivec.h"

using namespace oi;

/*!
 * \brief The Register class
 */
class Register : public ObjectTransformation,
                 public FunctionUtil,
                 public Transform<Point>,
                 public Transform<Sphere>,
                 public Transform<Circle> {
 public:
  // ##############################
  // function initialization method
  // ##############################

  void init() override;

  // ############
  // exec methods
  // ############

  std::expected<Point, std::string> exec(const Point& input) override;
  std::expected<Sphere, std::string> exec(const Sphere& input) override;
  std::expected<Circle, std::string> exec(const Circle& input) override;

 private:
  // ##############
  // helper methods
  // ##############

  bool setUpResult(Point& point);
  bool setUpResult(Sphere& sphere);
  bool setUpResult(Circle& circle);
};

#endif  // P_REGISTER_H
