#include "cf_function_data.h"

CFFunctionData::CFFunctionData() {}

std::string CFFunctionData::prettyPrint() {
  std::string s;
  s += "function: '" +
       (this->function.isNull() ? "null" : this->function->getMetaData().name) +
       "', feature: '" +
       (this->feature.isNull()
            ? "null"
            : this->feature->getFeature()->getFeatureName() +
                  "', featureType: '" + this->feature->getFeatureTypeString());
  return s;
}
