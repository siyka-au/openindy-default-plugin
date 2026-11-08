#ifndef OI_DEFAULT_PLUGIN_FUNCTIONS_OBJECT_TRANSFORMATION_P_REGISTER_H_
#define OI_DEFAULT_PLUGIN_FUNCTIONS_OBJECT_TRANSFORMATION_P_REGISTER_H_

#include <QObject>
#include <QPointer>

#include "object_transformation.h"
#include "oi_mat.h"
#include "oi_vec.h"

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

#endif  // OI_DEFAULT_PLUGIN_FUNCTIONS_OBJECT_TRANSFORMATION_P_REGISTER_H_
