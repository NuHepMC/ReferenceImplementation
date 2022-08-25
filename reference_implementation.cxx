#include "HepMC3/GenRunInfo.h"

#include "HepMC3/Attribute.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"

#include "HepMC3/WriterAscii.h"

#include "NuHepMC/utils.hxx"

// E.C.4
const double cm2_to_pb = 1E36;

namespace NuHepMC {
namespace VertexStatus {
const int kFSIAbs = 2;
}
namespace ParticleStatus {
const int kFSIState = 11;
}
} // namespace NuHepMC

std::shared_ptr<HepMC3::GenRunInfo> BuildGenRunInfo() {
  // G.R.1
  std::shared_ptr<HepMC3::GenRunInfo> run_info =
      std::make_shared<HepMC3::GenRunInfo>();

  // G.R.1
  NuHepMC::GenRunInfo::WriteNuHepMCVersion(run_info);

  // G.R.3
  run_info->tools().emplace_back(
      HepMC3::GenRunInfo::ToolInfo{"MyGen", "0.0.1", "My Favorite Generator"});

  // G.R.4
  std::map<int, std::pair<std::string, std::string>> ProcessIDs = {
      {200, {"CCQE", "The Moon QE Model -- PRD 1234 (1990)"}}, // G.C.5
      {250, {"NCQE", "The Moon QE Model -- PRD 1234 (1990)"}}, // G.C.5
      {300, {"MEC", "My Shiny MEC Model -- PRL 1 (1950)"}},    // G.C.5
      {500, {"Single PiPlus Production", "PiPlus 2049"}},
      {600, {"DIS", "Badabing Badaboom"}},
  };
  NuHepMC::GenRunInfo::WriteProcessIDDefinitions(run_info, ProcessIDs);

  // G.R.5
  std::map<int, std::pair<std::string, std::string>> VertexStatuses = {
      {1, {"PrimVer", "The primary vertex or hard scatter"}},
      {2, {"FSIAbs", "Final state absorption interaction"}},
  };
  NuHepMC::GenRunInfo::WriteVertexStatusIDDefinitions(run_info, ProcessIDs);

  // G.R.6
  std::map<int, std::pair<std::string, std::string>> ParticleStatuses = {
      {1, {"FinalState", "Undecayed physical particle"}},
      {4, {"InitialState", "Incoming beam particle"}},
      {11, {"FSI", "Final state interaction steps"}},
  };
  NuHepMC::GenRunInfo::WriteParticleStatusIDDefinitions(run_info,
                                                        ParticleStatuses);

  // G.R.7
  run_info->set_weight_names({
      "CV",
  });

  // G.C.1
  run_info->add_attribute(
      "NuHepMC.Conventions",
      std::make_shared<HepMC3::VectorStringAttribute>(
          std::vector<std::string>{"G.C.1", "G.C.2", "G.C.4", "G.C.5", "E.C.1",
                                   "E.C.2", "E.C.3", "E.C.4", "E.C.5"}));

  // G.C.2
  run_info->add_attribute("NuHepMC.Exposure.NEvents",
                          std::make_shared<HepMC3::IntAttribute>(3));

  // G.C.4
  run_info->add_attribute(
      "NuHepMC.FluxAveragedTotalCrossSection",
      std::make_shared<HepMC3::DoubleAttribute>(1.234E-38 * cm2_to_pb));

  return run_info;
}

HepMC3::GenEvent BuildEvent(std::shared_ptr<HepMC3::GenRunInfo> &run_info) {
  // E.R.3
  HepMC3::GenEvent evt(run_info, HepMC3::Units::MEV, HepMC3::Units::MM);
  // E.C.1
  evt.weight("CV") = 1;

  // E.C.2
  evt.add_attribute(
      "TotXS", std::make_shared<HepMC3::DoubleAttribute>(1.2E-36 * cm2_to_pb));
  // E.C.3
  evt.add_attribute(
      "ProcXS", std::make_shared<HepMC3::DoubleAttribute>(0.8E-36 * cm2_to_pb));

  // E.C.5
  double etime_s = 0;
  // E.R.4
  evt.add_attribute("LabPos", std::make_shared<HepMC3::VectorDoubleAttribute>(
                                  std::vector<double>{0, 0, 0, etime_s}));

  HepMC3::GenParticlePtr ISNeutron = std::make_shared<HepMC3::GenParticle>(
      HepMC3::FourVector{1.5255172492130473e+02, 8.9392830847276528e+01,
                         6.4870597568257821e+01, 9.5825554558124941e+02},
      2112, NuHepMC::ParticleStatus::kIncomingBeamParticle);
  // E.R.6
  HepMC3::GenParticlePtr ISnumu = std::make_shared<HepMC3::GenParticle>(
      HepMC3::FourVector{0.0000000000000000e+00, 0.0000000000000000e+00,
                         1.5000000000000000e+03, 1.5000000000000000e+03},
      14, NuHepMC::ParticleStatus::kIncomingBeamParticle);
  HepMC3::GenParticlePtr FSmuon = std::make_shared<HepMC3::GenParticle>(
      HepMC3::FourVector{-6.8928697531845643e+01, 4.8219068401438176e+02,
                         1.2406574501351240e+03, 1.3370316161682497e+03},
      13, NuHepMC::ParticleStatus::kUndecayedPhysicalParticle);
  HepMC3::GenParticlePtr FSProton = std::make_shared<HepMC3::GenParticle>(
      HepMC3::FourVector{2.2148042245314980e+02, -3.9279785316710411e+02,
                         3.2421314743313258e+02, 1.0903266675337304e+03},
      2212, NuHepMC::ParticleStatus::kUndecayedPhysicalParticle);
  HepMC3::GenParticlePtr FSIPiPlus = std::make_shared<HepMC3::GenParticle>(
      HepMC3::FourVector{2.2148042245314980e+02, -3.9279785316710411e+02,
                         3.2421314743313258e+02, 1.0903266675337304e+03},
      211, NuHepMC::ParticleStatus::kFSIState);

  ISNeutron->set_generated_mass(9.3956499999999994e+02);
  ISnumu->set_generated_mass(0.0000000000000000e+00);
  FSmuon->set_generated_mass(1.0565800000000023e+02);
  FSProton->set_generated_mass(9.3827200000000005e+02);
  FSIPiPlus->set_generated_mass(1.3957039000000000e+02);

  HepMC3::GenVertexPtr primvertex =
      std::make_shared<HepMC3::GenVertex>(HepMC3::FourVector{});
  // E.R.5
  primvertex->set_status(NuHepMC::VertexStatus::kPrimaryVertex);

  primvertex->add_particle_in(ISNeutron);
  primvertex->add_particle_in(ISnumu);
  primvertex->add_particle_out(FSmuon);
  primvertex->add_particle_out(FSProton);
  primvertex->add_particle_out(FSIPiPlus);

  evt.add_vertex(primvertex);

  HepMC3::GenParticlePtr FSProton2 = std::make_shared<HepMC3::GenParticle>(
      HepMC3::FourVector{2.2148042245314980e+02, -3.9279785316710411e+02,
                         3.2421314743313258e+02, 1.0903266675337304e+03},
      2212, NuHepMC::ParticleStatus::kUndecayedPhysicalParticle);
  FSProton2->set_generated_mass(9.3827200000000005e+02);

  HepMC3::GenVertexPtr fsivertex = std::make_shared<HepMC3::GenVertex>(
      HepMC3::FourVector{1E-10, 2E-10, 3E-10, 4E-10});
  // V.R.1
  fsivertex->set_status(NuHepMC::VertexStatus::kFSIAbs);

  fsivertex->add_particle_in(FSIPiPlus);
  fsivertex->add_particle_out(FSProton2);

  evt.add_vertex(fsivertex);

  return evt;
}

int main() {

  auto run_info = BuildGenRunInfo();

  auto writer =
      std::make_shared<HepMC3::WriterAscii>("example.hepmc3", run_info);

  auto evt = BuildEvent(run_info);
  evt.set_event_number(1);
  // E.R.1
  writer->write_event(evt);
  // E.R.2
  evt.add_attribute("ProcID", std::make_shared<HepMC3::IntAttribute>(200));

  evt.set_event_number(2);
  evt.add_attribute("ProcID", std::make_shared<HepMC3::IntAttribute>(300));
  writer->write_event(evt);

  evt.set_event_number(3);
  evt.add_attribute("ProcID", std::make_shared<HepMC3::IntAttribute>(500));
  writer->write_event(evt);

  writer->close();
}