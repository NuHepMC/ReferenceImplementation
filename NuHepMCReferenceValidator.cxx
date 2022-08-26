#include "NuHepMC/ReaderUtils.hxx"

#include "HepMC3/Print.h"
#include "HepMC3/ReaderFactory.h"

#include <sstream>
#include <stdexcept>
#include <string>

NEW_NuHepMC_EXCEPT(RequirementException);
NEW_NuHepMC_EXCEPT(ConventionException);

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

void Validate_GR7(std::shared_ptr<HepMC3::GenRunInfo> &run_info) {
  std::cout << "Event weights declared:" << std::endl;
  for (auto const &wn : run_info->weight_names()) {
    std::cout << "\t" << wn << std::endl;
  }

  if (!run_info->has_weight("CV")) {
    throw RequirementException() << "[FAILED]: G.R.7 Generator Identification";
  }
}

bool DeclaredConvention(std::set<std::string> const &conventions,
                        std::string const &testconv) {
  return std::find(conventions.begin(), conventions.end(), testconv) !=
         conventions.end();
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

  auto ProcessIds = NuHepMC::GenRunInfo::ReadProcessIDDefinitions(run_info);
  if (!ProcessIds.size()) {
    throw RequirementException() << "[FAILED]: G.R.4 Process Metadata";
  }

  std::cout << "Declared Process Identifiers: " << std::endl;
  for (auto &proc_descriptor : ProcessIds) {
    std::cout << "\t" << proc_descriptor.first << ": "
              << proc_descriptor.second.first << ", "
              << proc_descriptor.second.second << std::endl;
  }

  auto VertexStatuses =
      NuHepMC::GenRunInfo::ReadVertexStatusIDDefinitions(run_info);
  std::cout << "Declared Vertex Status Codes: " << std::endl;
  for (auto &vertex_status_descriptor : VertexStatuses) {
    std::cout << "\t" << vertex_status_descriptor.first << ": "
              << vertex_status_descriptor.second.first << ", "
              << vertex_status_descriptor.second.second << std::endl;
  }

  auto ParticleStatuses =
      NuHepMC::GenRunInfo::ReadParticleStatusIDDefinitions(run_info);
  std::cout << "Declared Particle Status Codes: " << std::endl;
  for (auto &particle_status_descriptor : ParticleStatuses) {
    std::cout << "\t" << particle_status_descriptor.first << ": "
              << particle_status_descriptor.second.first << ", "
              << particle_status_descriptor.second.second << std::endl;
  }

  Validate_GR7(run_info);

  auto Conventions = NuHepMC::GenRunInfo::ReadConventions(run_info);
  std::cout << "Reportedly following conventions: " << std::endl;
  for (auto &c : Conventions) {
    std::cout << "\t" << c << std::endl;
  }

  if (DeclaredConvention(Conventions, "G.C.2")) {
    auto NEvents = NuHepMC::CheckedAttributeValue<HepMC3::LongAttribute>(
        run_info, "NuHepMC.Exposure.NEvents", -1);
    if (NEvents == -1) {
      throw ConventionException()
          << "[FAILED] G.C.2 File Exposure (Standalone)";
    }
  }

  if (DeclaredConvention(Conventions, "G.C.3")) {
    auto POT = NuHepMC::CheckedAttributeValue<HepMC3::DoubleAttribute>(
        run_info, "NuHepMC.Exposure.POT", -1);
    auto Livetime = NuHepMC::CheckedAttributeValue<HepMC3::DoubleAttribute>(
        run_info, "NuHepMC.Exposure.Livetime", -1);

    if ((POT == -1) && (Livetime == -1)) {
      throw ConventionException()
          << "[FAILED] G.C.3 File Exposure (Experimental)";
    }
  }

  if (DeclaredConvention(Conventions, "G.C.4")) {
    auto fatx = NuHepMC::CheckedAttributeValue<HepMC3::DoubleAttribute>(
        run_info, "NuHepMC.FluxAveragedTotalCrossSection", -1);
    if (fatx == -1) {
      throw ConventionException()
          << "[FAILED] G.C.4 Flux-averaged Total Cross Section";
    }
  }

  if (!DeclaredConvention(Conventions, "G.C.5")) {
    std::cout << "[WARN]: You are not declaring that you cite contributors to "
                 "your model in your process metadata (G.C.5), we strongly "
                 "recommend that you do cite model-builders and then virtue "
                 "signal by declaring that your implementation adheres to G.C.5"
              << std::endl;
  }

  std::set<int> event_numbers;
  while (!reader->failed()) {
    // check event
    auto evnum = evt.event_number();
    if (event_numbers.count(evnum)) {
      throw RequirementException()
          << "Event number " << std::to_string(evnum)
          << " is not unique within this file.\n[FAILED] Event " << evnum
          << " -- E.R.1 Event Number";
    }
    if (evnum < 0) {
      throw RequirementException() << "Event number " << std::to_string(evnum)
                                   << " is negative.\n[FAILED] Event " << evnum
                                   << " -- E.R.1 Event Number";
    }
    event_numbers.insert(evnum);

    try {
      auto ProcID =
          NuHepMC::CheckedAttributeValue<HepMC3::IntAttribute>(&evt, "ProcID");
      if (!ProcessIds.count(ProcID)) {
        RequirementException()
            << "Process " << ProcID << " was not declared.\n[FAILED] Event "
            << evnum << " -- E.R.2 Process ID";
      }
    } catch (std::exception &e) {
      throw RequirementException() << "Subexception: (\n"
                                   << e.what() << " ).\n[FAILED] Event "
                                   << evnum << " -- E.R.2 Process ID";
    }

    try {
      auto LabPos =
          NuHepMC::CheckedAttributeValue<HepMC3::VectorDoubleAttribute>(
              &evt, "LabPos");
    } catch (std::exception &e) {
      throw RequirementException() << "Subexception: (\n"
                                   << e.what() << " ).\n[FAILED] Event "
                                   << evnum << " -- E.R.4 Lab Position";
    }

    int nprimvtx = 0;
    for (auto const &vtx : evt.vertices()) {
      if (vtx->status() == NuHepMC::VertexStatus::kPrimaryVertex) {
        nprimvtx++;
      }
      if (vtx->status() == 0) {
        throw RequirementException()
            << "Found vertex with status == 0.\n[FAILED] Event " << evnum
            << " -- E.R.5 Vertices";
      }
    }

    if (nprimvtx != 1) {
      throw RequirementException()
          << "Found " << nprimvtx << " Primary vertices.\n[FAILED] Event "
          << evnum << " -- E.R.5 Vertices";
    }

    if (evt.beams().size() < 1) {
      throw RequirementException()
          << "Event contained no beam particles.\n[FAILED] Event " << evnum
          << " -- E.R.6 Beam "
             "Particle";
    }

    if (DeclaredConvention(Conventions, "E.C.2")) {
      auto TotXS = NuHepMC::CheckedAttributeValue<HepMC3::DoubleAttribute>(
          &evt, "TotXS");

      if (TotXS == -1) {
        throw ConventionException()
            << "[FAILED] Event " << evnum << " -- E.C.2 Total Cross Section";
      }
    }

    if (DeclaredConvention(Conventions, "E.C.3")) {
      auto ProcXS = NuHepMC::CheckedAttributeValue<HepMC3::DoubleAttribute>(
          &evt, "ProcXS", -1);

      if (ProcXS == -1) {
        throw ConventionException()
            << "[FAILED] Event " << evnum << " -- E.C.3 Process Cross Section";
      }
    }

    if (DeclaredConvention(Conventions, "E.C.4")) {
      if (!evt.cross_section()) {
        throw ConventionException() << "[FAILED] Event " << evnum
                                    << " -- E.C.4 Estimated Cross Section";
      }
    }

    if (DeclaredConvention(Conventions, "E.C.6")) {
      auto LabPosTime =
          NuHepMC::CheckedAttributeValue<HepMC3::VectorDoubleAttribute>(
              &evt, "LabPos");

      if (LabPosTime.size() != 4) {
        throw ConventionException() << "[FAILED] Event " << evnum
                                    << " -- E.C.6 Estimated Cross Section";
      }
    }

    for (auto const &vtx : evt.vertices()) {
      if (vtx->status() != NuHepMC::VertexStatus::kPrimaryVertex) {
        if (!VertexStatuses.count(vtx->status())) {
          throw RequirementException()
              << "Vertex status " << vtx->status()
              << " was not declared in the GenRunInfo.\n[FAILED] Event "
              << evnum << " -- V.R.1 Vertex Status Codes";
        }
      }
    }

    for (auto const &part : evt.particles()) {
      if (part->status() >= 20) {
        if (!ParticleStatuses.count(part->status())) {
          throw RequirementException()
              << "Particle status " << part->status()
              << " was not declared in the GenRunInfo but was used in "
                 "event.\n[FAILED] Event "
              << evnum << " -- P.R.1 Particle Status Codes";
        }
      } else if (part->status() > 11) {
        throw RequirementException()
            << "Particle status " << part->status()
            << " is reserved but unused. " << evnum
            << "\n[FAILED] P.R.1 Particle Status Codes";

      } else if (part->status() < 1) {
        throw RequirementException()
            << "Particle status " << part->status()
            << " is invalid is event.\n[FAILED] Event " << evnum
            << " -- P.R.1 Particle Status Codes";
      }
    }

    // try and read the next one
    reader->read_event(evt);
  }

  std::cout << "[INFO]: Read " << event_numbers.size() << " events."
            << std::endl;

  return 0;
}