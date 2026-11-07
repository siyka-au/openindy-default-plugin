#include "p_oiexchangeascii.h"

#include <QtCore>
#include <algorithm>
#include <cstdint>
#include <format>
#include <iostream>
#include <iterator>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "containers.h"
#include "stringutil.h"

struct ColumnData {
  std::string featureName;
  std::string comment;
  std::string groupName;
  std::string oiFeatureCommonState;
  OiVec position;
  OiVec direction;
};

/*!
 * \brief OiExchangeAscii::init
 */
void OiExchangeAscii::init() {
  // set plugin meta data
  this->metaData.name = "OiExchangeAscii";
  this->metaData.pluginName = "OpenIndy Default Plugin";
  this->metaData.author = "bra";
  this->metaData.description = "Read simple Ascii files.";
  this->metaData.iid = ExchangeSimpleAscii_iidd;

  // set supported delimiters
  this->supportedDelimiters.push_back("whitespace [ ]");
  this->supportedDelimiters.push_back("semicolon [;]");

  // set supported geometries
  this->supportedGeometries.push_back(GeometryTypes::ePointGeometry);
  this->supportedGeometries.push_back(GeometryTypes::ePlaneGeometry);
  this->supportedGeometries.push_back(GeometryTypes::ePlaneLevelGeometry);
}

namespace {
// like QString::toDouble: the value, or 0 with *ok false
double parseDouble(const std::string& text, bool* ok) {
  const std::optional<double> value = oi::toDouble(text);
  *ok = value.has_value();
  return value.value_or(0.0);
}

// reads lines off a text, the way QTextStream::readLine does: the line
// break (LF or CRLF) is not part of the line
class LineReader {
 public:
  explicit LineReader(const std::string& text) : text(text) {}
  [[nodiscard]] bool atEnd() const {
    return this->position >= this->text.size();
  }
  std::string readLine() {
    const std::size_t end = this->text.find('\n', this->position);
    std::string line = this->text.substr(
        this->position,
        end == std::string::npos ? std::string::npos : end - this->position);
    this->position = end == std::string::npos ? this->text.size() : end + 1;
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }
    return line;
  }

 private:
  const std::string& text;
  std::size_t position{0};
};

// reads the whole stream from its start
std::string readText(std::iostream* stream) {
  stream->clear();
  stream->seekg(0);
  std::string bytes((std::istreambuf_iterator<char>(*stream)),
                    std::istreambuf_iterator<char>());
  stream->clear();
  return std::string(bytes.data(), static_cast<qsizetype>(bytes.size()));
}
}  // namespace

/*!
 * \brief OiExchangeAscii::getDefaultColumnOrder
 * \param typeOfGeometry
 * \return
 */
std::vector<ExchangeSimpleAscii::ColumnType>
OiExchangeAscii::getDefaultColumnOrder(const GeometryTypes& typeOfGeometry) {
  std::vector<ExchangeSimpleAscii::ColumnType> defaultColumnOrder;

  try {
    // check if the stream exists
    if (this->stream == nullptr) {
      return defaultColumnOrder;
    }

    // the whole text of the stream (the stream is read from the start)
    std::string text = readText(this->stream);

    // read the first twenty lines to get the maximum number of columns
    int numColumns = 0;
    int sampleSize = 20;
    LineReader in(text);
    while (!in.atEnd() && sampleSize >= 0) {
      std::string line = in.readLine();

      if (line.starts_with("#")      // skip comment
          || line.starts_with(";")   // skip comment
          || oi::trim(line).empty()  // skip empty lines
      ) {
        continue;
      }

      // split the line and compare its column count to the maximum column count
      // found before
      std::vector<std::string> columns = this->splitLine(line);
      if (std::cmp_greater(columns.size(), numColumns)) {
        numColumns = columns.size();
      }
    }

    // depending on the geometry type and the number of columns fill the default
    // columns order
    switch (typeOfGeometry) {
      case GeometryTypes::ePlaneGeometry:
      case GeometryTypes::ePlaneLevelGeometry:
        if (numColumns == 7) {
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnFeatureName);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnX);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnY);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnZ);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnPrimaryI);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnPrimaryJ);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnPrimaryK);
        }
        break;
      case GeometryTypes::ePointGeometry:
        if (numColumns == 1) {
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnX);
        } else if (numColumns == 2) {
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnX);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnY);
        } else if (numColumns == 3) {
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnX);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnY);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnZ);
        } else if (numColumns == 4) {
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnFeatureName);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnX);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnY);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnZ);
        } else if (numColumns == 5) {
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnFeatureName);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnX);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnY);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnZ);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnGroupName);
        } else if (numColumns == 6) {
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnFeatureName);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnX);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnY);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnZ);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnGroupName);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnComment);
        } else if (numColumns > 6) {
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnFeatureName);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnX);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnY);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnZ);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnGroupName);
          defaultColumnOrder.push_back(OiExchangeAscii::eColumnComment);
          for (int i = 6; i < numColumns; i++) {
            defaultColumnOrder.push_back(OiExchangeAscii::eColumnIgnore);
          }
        }
        break;
    }

  } catch (const exception& e) {
    this->sendMessage(e.what(), MessageTypes::eErrorMessage);
  }

  return defaultColumnOrder;
}

/*!
 * \brief OiExchangeAscii::getFilePreview
 * \param typeOfGeometry
 * \return
 */
std::map<ExchangeSimpleAscii::ColumnType, std::vector<oi::Value>>
OiExchangeAscii::getFilePreview(const GeometryTypes& typeOfGeometry) {
  std::map<ExchangeSimpleAscii::ColumnType, std::vector<oi::Value>> filePreview;

  // get the default column order
  std::vector<ExchangeSimpleAscii::ColumnType> defaultColumnOrder =
      this->getDefaultColumnOrder(typeOfGeometry);

  // insert an empty list for each column as default
  for (auto i : defaultColumnOrder) {
    filePreview.insert_or_assign(i, std::vector<oi::Value>());
  }

  try {
    // check if the stream exists
    if (this->stream == nullptr) {
      return filePreview;
    }

    // the whole text of the stream (the stream is read from the start)
    std::string text = readText(this->stream);

    // get the first twenty lines as preview
    int sampleSize = 20;
    LineReader in(text);
    while (!in.atEnd() && sampleSize >= 0) {
      std::string line = in.readLine();

      // split the line at delimiter
      std::vector<std::string> columns = this->splitLine(line);

      // insert the column entries
      for (int i = 0; std::cmp_less(i, defaultColumnOrder.size()); i++) {
        // add an empty entry if the current row has not enough columns
        if (std::cmp_less_equal(columns.size(), i)) {
          filePreview[defaultColumnOrder.at(i)].push_back(std::string());
          continue;
        }

        // get the rows that already exist and append the current one
        filePreview[defaultColumnOrder.at(i)].push_back(columns.at(i));
      }

      sampleSize--;
    }

  } catch (const exception& e) {
    this->sendMessage(e.what(), MessageTypes::eErrorMessage);
  }

  return filePreview;
}

/*!
 * \brief OiExchangeAscii::getPossibleColumns
 * \param typeOfGeometry
 * \return
 */
std::vector<ExchangeSimpleAscii::ColumnType>
OiExchangeAscii::getPossibleColumns(const GeometryTypes& typeOfGeometry) {
  std::vector<ExchangeSimpleAscii::ColumnType> possibleColumns;

  // add all possible columns depending on the type of geometry
  switch (typeOfGeometry) {
    case GeometryTypes::ePointGeometry:
      possibleColumns.push_back(ExchangeSimpleAscii::eColumnX);
      possibleColumns.push_back(ExchangeSimpleAscii::eColumnY);
      possibleColumns.push_back(ExchangeSimpleAscii::eColumnZ);
      possibleColumns.push_back(ExchangeSimpleAscii::eColumnFeatureName);
      possibleColumns.push_back(ExchangeSimpleAscii::eColumnGroupName);
      possibleColumns.push_back(ExchangeSimpleAscii::eColumnComment);
      break;
  }

  return possibleColumns;
}

/*!
 * \brief OiExchangeAscii::splitLine
 * Splits a line at the delimiter in use. Empty pieces are kept: a line that
 * starts with whitespace has an empty first column.
 */
std::vector<std::string> OiExchangeAscii::splitLine(
    const std::string& line) const {
  if (this->usedDelimiter == "whitespace [ ]") {
    // runs of whitespace separate
    std::vector<std::string> columns;
    std::string current;
    bool inGap = false;
    for (const char c : line) {
      const bool space = c == ' ' || c == '\t' || c == '\n' || c == '\v' ||
                         c == '\f' || c == '\r';
      if (space) {
        if (!inGap) {
          columns.push_back(current);
          current.clear();
          inGap = true;
        }
      } else {
        current.push_back(c);
        inGap = false;
      }
    }
    columns.push_back(current);
    return columns;
  }
  if (this->usedDelimiter == "semicolon [;]") {
    return oi::split(line, ";");
  }

  // any other delimiter used to be the pattern ".*", which matches the whole
  // line: an empty piece before and after it
  return line.empty() ? std::vector<std::string>{""}
                      : std::vector<std::string>{"", ""};
}

/*!
 * \brief OiExchangeAscii::importOiData
 */
void OiExchangeAscii::importOiData(const OperationContext& ctx) {
  try {
    this->features.clear();

    // check if nominal system is valid
    if (this->nominalSystem.isNull()) {
      this->importFinished(false);
      return;
    }

    // set the number of error prone lines to 0
    int numErrors = 0;

    // check if the stream exists
    if (this->stream == nullptr) {
      this->importFinished(false);
      return;
    }

    // the whole text of the stream (the stream is read from the start)
    std::string text = readText(this->stream);

    std::int64_t fileSize = text.size();
    std::int64_t readSize = 0;
    std::int64_t numPoints = 0;

    bool notSkipped = this->getSkipFirstLine();
    // read all lines
    LineReader in(text);
    while (!in.atEnd()) {
      ctx.throwIfCancelled();
      std::string line = in.readLine();
      readSize += line.size();

      if (notSkipped                 // skip first line
          || line.starts_with("#")   // skip comment
          || line.starts_with(";")   // skip comment
          || oi::trim(line).empty()  // skip empty lines
      ) {
        notSkipped = false;
        continue;
      }

      // convert , to .
      std::ranges::replace(line, ',',
                           '.');  // decimal separator TODO use locale format

      // split the line at delimiter
      std::vector<std::string> columns = this->splitLine(line);

      bool errorWhileParsing = false;

      ColumnData columnData = {"", "", "", "", OiVec(3), OiVec(3)};

      for (int i = 0; std::cmp_less(i, columns.size()); i++) {
        // stop parsing if the current line has too many columns
        if (std::cmp_less_equal(this->userDefinedColumns.size(), i)) {
          break;
        }

        // set the point attribute depending on the current column
        switch (this->userDefinedColumns.at(i)) {
          case ExchangeSimpleAscii::eColumnCommonState:
            columnData.oiFeatureCommonState = columns.at(i);
            break;
          case ExchangeSimpleAscii::eColumnFeatureName:
            columnData.featureName = columns.at(i);
            break;
          case ExchangeSimpleAscii::eColumnComment:
            columnData.comment = columns.at(i);
            break;
          case ExchangeSimpleAscii::eColumnGroupName:
            columnData.groupName = columns.at(i);
            break;
          case ExchangeSimpleAscii::eColumnX: {
            double x = 0.0;
            x = parseDouble(columns.at(i), &errorWhileParsing);
            errorWhileParsing = !errorWhileParsing;

            // transform the unit of the imported coordinate to [m]
            if (this->units.contains(DimensionType::eMetric) &&
                oi::value(this->units, DimensionType::eMetric) !=
                    UnitType::eUnitMeter) {
              x = convertToDefault(
                  x, oi::value(this->units, DimensionType::eMetric));
            }

            if (!errorWhileParsing) {
              columnData.position.setAt(0, x);
            }

            break;
          }
          case ExchangeSimpleAscii::eColumnY: {
            double y = 0.0;
            y = parseDouble(columns.at(i), &errorWhileParsing);
            errorWhileParsing = !errorWhileParsing;

            // transform the unit of the imported coordinate to [m]
            if (this->units.contains(DimensionType::eMetric) &&
                oi::value(this->units, DimensionType::eMetric) !=
                    UnitType::eUnitMeter) {
              y = convertToDefault(
                  y, oi::value(this->units, DimensionType::eMetric));
            }

            if (!errorWhileParsing) {
              columnData.position.setAt(1, y);
            }

            break;
          }
          case ExchangeSimpleAscii::eColumnZ: {
            double z = 0.0;
            z = parseDouble(columns.at(i), &errorWhileParsing);
            errorWhileParsing = !errorWhileParsing;

            // transform the unit of the imported coordinate to [m]
            if (this->units.contains(DimensionType::eMetric) &&
                oi::value(this->units, DimensionType::eMetric) !=
                    UnitType::eUnitMeter) {
              z = convertToDefault(
                  z, oi::value(this->units, DimensionType::eMetric));
            }

            if (!errorWhileParsing) {
              columnData.position.setAt(2, z);
            }

            break;
          }
          case ExchangeSimpleAscii::eColumnIgnore:
            break;
          case ExchangeSimpleAscii::eColumnPrimaryI: {
            double value = parseDouble(columns.at(i), &errorWhileParsing);
            errorWhileParsing = !errorWhileParsing;

            // transform the unit of the imported coordinate to [m]
            if (this->units.contains(DimensionType::eAngular) &&
                oi::value(this->units, DimensionType::eAngular) !=
                    UnitType::eUnitDecimalDegree) {
              value = convertToDefault(
                  value, oi::value(this->units, DimensionType::eAngular));
            }

            if (!errorWhileParsing) {
              columnData.direction.setAt(0, value);
            }
            break;
          }
          case ExchangeSimpleAscii::eColumnPrimaryJ: {
            double value = parseDouble(columns.at(i), &errorWhileParsing);
            errorWhileParsing = !errorWhileParsing;

            // transform the unit of the imported coordinate to [m]
            if (this->units.contains(DimensionType::eAngular) &&
                oi::value(this->units, DimensionType::eAngular) !=
                    UnitType::eUnitDecimalDegree) {
              value = convertToDefault(
                  value, oi::value(this->units, DimensionType::eAngular));
            }

            if (!errorWhileParsing) {
              columnData.direction.setAt(1, value);
            }
            break;
          }
          case ExchangeSimpleAscii::eColumnPrimaryK: {
            double value = parseDouble(columns.at(i), &errorWhileParsing);
            errorWhileParsing = !errorWhileParsing;

            // transform the unit of the imported coordinate to [m]
            if (this->units.contains(DimensionType::eAngular) &&
                oi::value(this->units, DimensionType::eAngular) !=
                    UnitType::eUnitDecimalDegree) {
              value = convertToDefault(
                  value, oi::value(this->units, DimensionType::eAngular));
            }

            if (!errorWhileParsing) {
              columnData.direction.setAt(2, value);
            }
            break;
          }
        }

        // if an error occured continue with the next line
        if (errorWhileParsing) {
          numErrors++;
          break;
        }

      }  // loop over colums

      // create geometry and add the imported nominal to OpenIndy
      if (!errorWhileParsing) {
        switch (this->typeOfGeometry) {
          case GeometryTypes::ePointGeometry: {
            auto ownedNominal = std::make_unique<Point>();
            oi::Guarded<Point> myNominal = ownedNominal.get();
            // I use QT property system for transportation, because "common" is
            // not "common" of nominal point but actual point!
            myNominal->setTransientAttribute("OI_FEATURE_COMMONSTATE",
                                             columnData.oiFeatureCommonState);

            myNominal->setFeatureName(columnData.featureName);
            myNominal->setGroupName(columnData.groupName);
            myNominal->setComment(columnData.comment);
            myNominal->setPoint(Position(columnData.position));

            // set group of the geometry
            if (this->groupName.compare("") != 0) {
              myNominal->setGroupName(this->groupName);
            }

            // set nominal system
            myNominal->setNominalSystem(this->nominalSystem);

            this->addImportedFeature(std::move(ownedNominal));

            break;
          }
          case GeometryTypes::ePlaneGeometry:
          case GeometryTypes::ePlaneLevelGeometry: {
            auto ownedPlane = std::make_unique<Plane>();
            oi::Guarded<Plane> plane = ownedPlane.get();
            // I use QT property system for transportation, because level is a
            // special plane
            plane->setTransientAttribute(
                "OI_FEATURE_PLANE_LEVEL",
                this->typeOfGeometry == GeometryTypes::ePlaneLevelGeometry);

            plane->setFeatureName(columnData.featureName);
            plane->setGroupName(columnData.groupName);
            plane->setComment(columnData.comment);

            // plane->setPoint(Position(columnData.position));
            plane->setPlane(Position(columnData.position),
                            Direction(columnData.direction));

            // set group of the geometry
            if (this->groupName.compare("") != 0) {
              plane->setGroupName(this->groupName);
            }

            // set nominal system
            plane->setNominalSystem(this->nominalSystem);

            this->addImportedFeature(std::move(ownedPlane));

            break;
          }
        }  // switch
      }

      // update import progress
      numPoints++;
      // never 100% before the import has finished
      ctx.report(fileSize > 0
                     ? std::min(0.99, static_cast<double>(readSize) /
                                          static_cast<double>(fileSize))
                     : 0.0,
                 std::format("{} nominal(s) loaded", numPoints));
      readSize += 2;
    }

    // import finished signal
    this->importFinished(true);

  } catch (const OperationCancelled&) {
    this->sendMessage("Import cancelled", MessageTypes::eInformationMessage);
    this->importFinished(false);
  } catch (const exception& e) {
    this->sendMessage(e.what(), MessageTypes::eErrorMessage,
                      MessageDestinations::eMessageBoxMessage);
  }
}

/*!
 * \brief OiExchangeAscii::exportOiData
 */
void OiExchangeAscii::exportOiData(const OperationContext& /*ctx*/) {
  try {
    if (this->stream != nullptr) {
      std::string outText;

      for (const oi::Guarded<FeatureWrapper>& fw :
           this->currentJob->getGeometriesList()) {
        if (fw->getGeometry()->getIsSolved() &&
            fw->getGeometry()->getOwnerDescription().has_value()) {
          // write to file
          std::string line = "";
          // std::string delimiter = this->getDelimiter(this->usedDelimiter);
          for (int i = 0; std::cmp_less(i, this->userDefinedColumns.size());
               i++) {
            if (i > 0) {
              line.append("\t");
            }
            switch (this->userDefinedColumns.at(i)) {
              case eColumnFeatureName:
                line.append(fw->getFeature()->getFeatureName());
                break;
              case eColumnX:
                if (fw->getGeometry()->hasPosition()) {
                  line.append(oi::number(
                      convertFromDefault(
                          fw->getGeometry()->getPosition().getVector().getAt(0),
                          this->units[DimensionType::eMetric]),
                      'f', this->getDistanceDigits()));
                } else {
                  line.append("-");
                }
                break;
              case eColumnY:
                if (fw->getGeometry()->hasPosition()) {
                  line.append(oi::number(
                      convertFromDefault(
                          fw->getGeometry()->getPosition().getVector().getAt(1),
                          this->units[DimensionType::eMetric]),
                      'f', this->getDistanceDigits()));
                } else {
                  line.append("-");
                }
                break;
              case eColumnZ:
                if (fw->getGeometry()->hasPosition()) {
                  line.append(oi::number(
                      convertFromDefault(
                          fw->getGeometry()->getPosition().getVector().getAt(2),
                          this->units[DimensionType::eMetric]),
                      'f', this->getDistanceDigits()));
                } else {
                  line.append("-");
                }
                break;
              case eColumnPrimaryI:
                if (fw->getGeometry()->hasDirection()) {
                  line.append(oi::number(
                      fw->getGeometry()->getDirection().getVector().getAt(0),
                      'f', this->getAngleDigits()));
                } else {
                  line.append("-");
                }
                break;
              case eColumnPrimaryJ:
                if (fw->getGeometry()->hasDirection()) {
                  line.append(oi::number(
                      fw->getGeometry()->getDirection().getVector().getAt(1),
                      'f', this->getAngleDigits()));
                } else {
                  line.append("-");
                }
                break;
              case eColumnPrimaryK:
                if (fw->getGeometry()->hasDirection()) {
                  line.append(oi::number(
                      fw->getGeometry()->getDirection().getVector().getAt(2),
                      'f', this->getAngleDigits()));
                } else {
                  line.append("-");
                }
                break;
              case eColumnRadiusA:
                if (fw->getGeometry()->hasRadius()) {
                  line.append(
                      oi::number(convertFromDefault(
                                     fw->getGeometry()->getRadius().getRadius(),
                                     this->units[DimensionType::eMetric]),
                                 'f', this->getDistanceDigits()));
                } else {
                  line.append("-");
                }
                line.append("\n");
                break;
              default:
                break;
            }
          }
          // write to file
          outText += line;
        }
      }
      *this->stream << outText;
      this->stream->flush();
      this->exportFinished(this->stream->good());

    } else {
      this->sendMessage("export stream is not set",
                        MessageTypes::eErrorMessage);
      this->exportFinished(false);
    }

  } catch (const exception& e) {
    this->sendMessage(e.what(), MessageTypes::eErrorMessage,
                      MessageDestinations::eMessageBoxMessage);
  }
}
