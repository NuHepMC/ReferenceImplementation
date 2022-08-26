#pragma once

#include "HepMC3/GenRunInfo.h"

#include "HepMC3/Attribute.h"

#include "NuHepMC/Constants.hxx"
#include "NuHepMC/Types.hxx"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace NuHepMC {

namespace GenRunInfo {

inline void WriteIDDefinitions(
    std::shared_ptr<HepMC3::GenRunInfo> &run_info,
    StatusCodeDescriptors const &Definitions,
    std::pair<std::string, std::string> const &AttributeStubs) {

  std::vector<int> IDs;
  for (auto const &p : Definitions) {
    IDs.push_back(p.first);
  }
  run_info->add_attribute("NuHepMC." + AttributeStubs.first,
                          std::make_shared<HepMC3::VectorIntAttribute>(IDs));

  for (auto const &p : Definitions) {
    IDs.push_back(p.first);
  }

  for (auto const &p : Definitions) {
    run_info->add_attribute(
        "NuHepMC." + AttributeStubs.second + "[" + std::to_string(p.first) +
            "].Name",
        std::make_shared<HepMC3::StringAttribute>(p.second.first));
    run_info->add_attribute(
        "NuHepMC." + AttributeStubs.second + "[" + std::to_string(p.first) +
            "].Description",
        std::make_shared<HepMC3::StringAttribute>(p.second.second));
  }
}

// G.R.4
inline void WriteProcessIDDefinitions(
    std::shared_ptr<HepMC3::GenRunInfo> &run_info,
    StatusCodeDescriptors const &Definitions) {
  WriteIDDefinitions(run_info, Definitions, {"ProcessIDs", "ProcessInfo"});
}

// G.R.5
inline void WriteVertexStatusIDDefinitions(
    std::shared_ptr<HepMC3::GenRunInfo> &run_info,
    StatusCodeDescriptors const &Definitions) {
  WriteIDDefinitions(run_info, Definitions,
                     {"VertexStatusIDs", "VertexStatusInfo"});
}

// G.R.6
inline void WriteParticleStatusIDDefinitions(
    std::shared_ptr<HepMC3::GenRunInfo> &run_info,
    StatusCodeDescriptors const &Definitions) {
  WriteIDDefinitions(run_info, Definitions,
                     {"ParticleStatusIDs", "ParticleStatusInfo"});
}

// G.R.2
inline void WriteNuHepMCVersion(std::shared_ptr<HepMC3::GenRunInfo> &run_info) {
  run_info->add_attribute("NuHepMC.Version.Major",
                          std::make_shared<HepMC3::IntAttribute>(0));
  run_info->add_attribute("NuHepMC.Version.Minor",
                          std::make_shared<HepMC3::IntAttribute>(1));
  run_info->add_attribute("NuHepMC.Version.Patch",
                          std::make_shared<HepMC3::IntAttribute>(0));
}

} // namespace GenRunInfo
} // namespace NuHepMC