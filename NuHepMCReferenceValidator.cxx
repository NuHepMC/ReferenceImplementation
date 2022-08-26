#include "NuHepMC/ReaderUtils.hxx"

#include "HepMC3/Print.h"
#include "HepMC3/ReaderFactory.h"

#include <sstream>
#include <stdexcept>
#include <string>

NEW_NuHepMC_EXCEPT(RequirementException);
NEW_NuHepMC_EXCEPT(ConventionException);

NEW_NuHepMC_EXCEPT(ToolMetadataException);

void Validate_GR1(std::shared_ptr<HepMC3::GenRunInfo> &run_info) {
  if (!run_info) {
    throw RequirementException() << "[FAILED]: G.R.1 Valid GenRunInfo";
  }
}

void Validate_GR2(std::shared_ptr<HepMC3::GenRunInfo> &run_info) {
  try {
    auto version_info = NuHepMC::GenRunInfo::ReadNuHepMCVersion(run_info);

    std::cout << "NuHepMC Version: " << std::get<0>(version_info) << "."
              << std::get<1>(version_info) << "." << std::get<2>(version_info)
              << std::endl;
  } catch (std::exception &e) {
    throw RequirementException()
        << "Subexception: (\n"
        << e.what() << " )\n[FAILED]: G.R.2 NuHepMC Version";
  }
}

void Validate_GR3(std::shared_ptr<HepMC3::GenRunInfo> &run_info) {
  std::cout << "Tools Used:" << std::endl;
  for (auto const &t : run_info->tools()) {
    std::cout << "\tname: " << t.name << std::endl;
    std::cout << "\t\tversion: " << t.version << std::endl;
    std::cout << "\t\tdescription: " << t.description << std::endl;
    if (!t.name.size() || !t.version.size() || !t.description.size()) {
      throw RequirementException()
          << "[FAILED]: G.R.3 Generator Identification";
    }
  }
}

int main(int argc, char const *argv[]) {
  auto reader = HepMC3::deduce_reader(argv[1]);
  if (!reader || reader->failed()) {
    return 1;
  }

  // Some readers
  HepMC3::GenEvent evt;
  reader->read_event(evt);

  auto run_info = reader->run_info();
  Validate_GR1(run_info);
  Validate_GR2(run_info);
  Validate_GR3(run_info);

  while (!reader->failed()) {
    // check event

    // try and read the next one
    reader->read_event(evt);
  }

  return 0;
}