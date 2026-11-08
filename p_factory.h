#ifndef OI_DEFAULT_PLUGIN_P_FACTORY_H_
#define OI_DEFAULT_PLUGIN_P_FACTORY_H_
#include <memory>
#include <string>
#include <vector>

#include "bundle_adjustment.h"
#include "configured_function.h"
#include "configured_function_config.h"
#include "construct_function.h"
#include "distance_between_two_points.h"
#include "fit_function.h"
#include "function.h"
#include "function_config_parser.h"
#include "geodetic_function.h"
#include "laser_tracker.h"
#include "object_transformation.h"
#include "p_average_from_points.h"
#include "p_best_fit_circle.h"
#include "p_best_fit_circle_in_plane.h"
#include "p_best_fit_cylinder.h"
#include "p_best_fit_line.h"
#include "p_best_fit_plane.h"
#include "p_best_fit_point.h"
#include "p_best_fit_sphere.h"
#include "p_change_radius.h"
#include "p_helmert6_param.h"
#include "p_helmert7_param.h"
#include "p_intersect_line_line.h"
#include "p_intersect_line_plane.h"
#include "p_intersect_plane_plane.h"
#include "p_line_from_plane_normal.h"
#include "p_nominal_point.h"
#include "p_oi_exchange_ascii.h"
#include "p_plane_from_plane.h"
#include "p_point_between_two_points.h"
#include "p_point_from_plane_origin.h"
#include "p_point_from_points.h"
#include "p_pseudo_tracker.h"
#include "p_rectify_to_point.h"
#include "p_rectify_to_vector.h"
#include "p_register.h"
#include "p_shift.h"
#include "p_translate_by_value.h"
#include "plugin.h"
#include "sensor.h"
#include "sensor_configuration.h"
#include "simple_polar_measurement.h"
#include "simulation_model.h"
#include "system_transformation.h"
#include "total_station.h"
#include "xyz_filter.h"

using namespace oi;

class OiTemplatePlugin : public Plugin {
 public:
  OiTemplatePlugin();

  // ##########################################################
  // create instances of all available plugin types in a plugin
  // ##########################################################

  std::vector<std::unique_ptr<Sensor>> createSensors();
  std::vector<std::unique_ptr<Function>> createFunctions();
  std::vector<std::unique_ptr<BundleAdjustment>> createBundleAdjustments();
  std::vector<std::unique_ptr<SimulationModel>> createSimulations();
  std::vector<std::unique_ptr<ExchangeSimpleAscii>>
  createSimpleAsciiExchanges();
  std::vector<std::unique_ptr<ExchangeDefinedFormat>>
  createDefinedFormatExchanges();

  // ###################################################
  // create instances of a plugin type with a given name
  // ###################################################

  std::unique_ptr<Sensor> createSensor(const std::string& name);
  std::unique_ptr<Function> createFunction(const std::string& name);
  std::unique_ptr<Function> createFunctionFromConfig(const std::string& name);
  std::unique_ptr<BundleAdjustment> createBundleAdjustment(
      const std::string& name);
  std::unique_ptr<SimulationModel> createSimulation(const std::string& name);
  std::unique_ptr<ExchangeSimpleAscii> createSimpleAsciiExchange(
      const std::string& name);
  std::unique_ptr<ExchangeDefinedFormat> createDefinedFormatExchange(
      const std::string& name);
};

#endif  // OI_DEFAULT_PLUGIN_P_FACTORY_H_
