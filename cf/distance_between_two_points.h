#ifndef OI_DEFAULT_PLUGIN_CF_DISTANCE_BETWEEN_TWO_POINTS_H_
#define OI_DEFAULT_PLUGIN_CF_DISTANCE_BETWEEN_TWO_POINTS_H_

#include "construct_function.h"

using namespace oi;

class DistanceBetweenTwoPoints : public ConstructFunction,
                                 public Producer<ScalarEntityDistance> {
 public:
  void init() override;

  std::expected<ScalarEntityDistance, std::string> exec() override;
};

#endif  // OI_DEFAULT_PLUGIN_CF_DISTANCE_BETWEEN_TWO_POINTS_H_
