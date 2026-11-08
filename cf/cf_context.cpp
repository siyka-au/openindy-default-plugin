#include "cf_context.h"

#include <format>
#include <utility>
#include <vector>

#include "containers.h"

CFContext::CFContext() {}

/**
 * @brief getFunction
 * @param name
 * @return function instance and removes function from list
 */
oi::Guarded<Function> CFContext::getFunction(std::string name) {
  for (int index = 0; std::cmp_less(index, functions.size()); index++) {
    if (!functions[index].isNull() &&
        functions[index]->getMetaData().name.compare(name) == 0) {
      oi::Guarded<Function> f = functions[static_cast<std::size_t>(index)];
      functions.erase(functions.begin() + index);  // remove from list
      return f;
    }
  }

  throw logic_error(std::format("no function found: {}", name));
}

InputElement CFContext::getInputElement(std::string name) {
  std::vector<InputElement> ies =
      oi::value(this->global_inputElements,
                oi::indexOf(config.getNeededElementNames(), name));
  if (ies.size() == 0) {
    throw logic_error(std::format("invalid count ({}) of InputElements: {}",
                                  ies.size(), name));
  }
  return ies.front();
}

/**
 * @brief createApplicableFeature
 * @param featureType
 * @return feature for element of featuresTypes
 */
oi::Guarded<FeatureWrapper> CFContext::createApplicableFeature(
    FeatureTypes featureType) {
  switch (featureType) {
    case FeatureTypes::ePointFeature: {
      auto feature = std::make_unique<Point>();
      feature->setFeatureName("generated Point");
      return this->adopt(std::move(feature));
    }
    case FeatureTypes::eScalarEntityDistanceFeature: {
      auto feature = std::make_unique<ScalarEntityDistance>();
      feature->setFeatureName("generated ScalarEntityDistance");
      return this->adopt(std::move(feature));
    }
  }

  throw logic_error(std::string("can not create applicable feature"));
}

oi::Guarded<FeatureWrapper> CFContext::adopt(std::unique_ptr<Feature> feature) {
  oi::Guarded<FeatureWrapper> wrapper = feature->getFeatureWrapper();
  this->generatedFeatures.push_back(std::move(feature));
  return wrapper;
}

InputElement CFContext::createInputElement(
    oi::Guarded<FeatureWrapper> feature) {
  InputElement element;
  element.label = std::format("generated InputElement: {}",
                              feature->getFeatureTypeString());

  switch (feature->getFeatureTypeEnum()) {
    case FeatureTypes::ePointFeature:
      element.typeOfElement = ElementTypes::ePointElement;
      element.point = feature->getPoint();
      element.geometry = feature->getPoint();
      break;
    case FeatureTypes::eScalarEntityDistanceFeature:
      element.typeOfElement = ElementTypes::eScalarEntityDistanceElement;
      element.scalarEntityDistance = feature->getScalarEntityDistance();
      element.geometry = feature->getScalarEntityDistance();
      break;
    default:
      throw logic_error(
          std::format("can not create input element for feature type: {}",
                      feature->getFeatureTypeString()));
  }
  return element;
}
