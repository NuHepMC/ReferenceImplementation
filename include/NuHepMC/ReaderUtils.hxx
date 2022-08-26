#pragma once

#include "HepMC3/GenRunInfo.h"

#include "HepMC3/Attribute.h"

#include "NuHepMC/Constants.hxx"
#include "NuHepMC/Exceptions.hxx"
#include "NuHepMC/Types.hxx"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace NuHepMC {

NEW_NuHepMC_EXCEPT(NullObjectException);
NEW_NuHepMC_EXCEPT(MissingAttributeException);
NEW_NuHepMC_EXCEPT(AttributeTypeException);

template <typename T> bool HasAttribute(T const &obj, std::string const &name) {
  auto attr_names = obj->attribute_names();
  return std::find(attr_names.begin(), attr_names.end(), name) !=
         attr_names.end();
}

template <typename AT, typename T>
auto CheckedAttributeValue(T const &obj, std::string const &name) {
  if (!obj) {
    throw NullObjectException();
  }

  if (!HasAttribute(obj, name)) {
    MissingAttributeException mae;
    mae << "Failed to find attribute: " << name;
    mae << "\n\tKnown attributes: \n";
    for (auto const &a : obj->attribute_names()) {
      mae << "\t\t" << a;
    }
    throw mae;
  }

  if (!obj->template attribute<AT>(name)) {
    throw AttributeTypeException()
        << name << ": " << obj->attribute_as_string(name);
  }

  return obj->template attribute<AT>(name)->value();
}

namespace GenRunInfo {

inline StatusCodeDescriptors
ReadIDDefinitions(std::shared_ptr<HepMC3::GenRunInfo> &run_info,
                  std::pair<std::string, std::string> const &AttributeStubs) {

  auto IDs = CheckedAttributeValue<HepMC3::VectorIntAttribute>(
      run_info, "NuHepMC." + AttributeStubs.first);

  StatusCodeDescriptors status_codes;
  for (auto const &id : IDs) {
    status_codes[id] = std::pair<std::string, std::string>{
        CheckedAttributeValue<HepMC3::StringAttribute>(
            run_info, "NuHepMC." + AttributeStubs.second + "[" +
                          std::to_string(id) + "].Name"),
        CheckedAttributeValue<HepMC3::StringAttribute>(
            run_info, "NuHepMC." + AttributeStubs.second + "[" +
                          std::to_string(id) + "].Description")};
  }
  return status_codes;
}

// G.R.4
inline StatusCodeDescriptors
ReadProcessIDDefinitions(std::shared_ptr<HepMC3::GenRunInfo> &run_info) {
  return ReadIDDefinitions(run_info, {"ProcessIDs", "ProcessInfo"});
}

// G.R.5
inline StatusCodeDescriptors
ReadVertexStatusIDDefinitions(std::shared_ptr<HepMC3::GenRunInfo> &run_info) {
  return ReadIDDefinitions(run_info, {"VertexStatusIDs", "VertexStatusInfo"});
}

// G.R.6
inline StatusCodeDescriptors
ReadParticleStatusIDDefinitions(std::shared_ptr<HepMC3::GenRunInfo> &run_info) {
  return ReadIDDefinitions(run_info,
                           {"ParticleStatusIDs", "ParticleStatusInfo"});
}

// G.R.2
inline std::tuple<int, int, int>
ReadNuHepMCVersion(std::shared_ptr<HepMC3::GenRunInfo> &run_info) {
  return std::tuple<int, int, int>{CheckedAttributeValue<HepMC3::IntAttribute>(
                                       run_info, "NuHepMC.Version.Major"),
                                   CheckedAttributeValue<HepMC3::IntAttribute>(
                                       run_info, "NuHepMC.Version.Minor"),
                                   CheckedAttributeValue<HepMC3::IntAttribute>(
                                       run_info, "NuHepMC.Version.Patch")};
}

} // namespace GenRunInfo
} // namespace NuHepMC