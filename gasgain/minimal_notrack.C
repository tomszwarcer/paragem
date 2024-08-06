//TODO: ion mobility
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <random>

#include "Garfield/ComponentElmer.hh"
#include "Garfield/MediumMagboltz.hh"
#include "Garfield/Sensor.hh"
#include "Garfield/AvalancheMicroscopic.hh"
#include "Garfield/Medium.hh"

#include <string>
#include <vector>
#include <chrono>

using namespace Garfield;

void minimal_notrack(double ar_percent, int rn){

  std::string run_number = std::to_string(rn);

  //Create MM object for gas and set it up
  MediumMagboltz gas;
  gas.SetComposition("cf4", 100.-ar_percent, "argon", ar_percent);
  gas.SetTemperature(293.15);
  gas.SetPressure(60.);
  gas.Initialise();

  //Setup handover from Elmer
  ComponentElmer fm;

  std::string mapDir = "";
  //The path must end in a "/"
  mapDir += "/opt/ppd/scratch/szwarcer/paragem/Gmsh/dGem/";
  //mapDir += "/Users/tomszwarcer/Documents/MIGDAL/GEM7/";

  fm.Initialise(mapDir + "mesh.header",
                mapDir + "mesh.elements",
                mapDir + "mesh.nodes",
                mapDir + "dielectrics.dat",
                mapDir + "mesh.result",
                "mm");
  fm.EnableTetrahedralTreeForElementSearch(true);
  fm.EnablePeriodicityX();
  fm.EnablePeriodicityY();
  fm.SetGas(&gas);
  fm.SetMedium(0,&gas);

  //define consts
  const double pitch = 0.0280;
  const double hole_radius = 0.017/2.;
  const double height = sqrt(3)*pitch;
  const double view_min_z = -0.2;

  //this is the region electrons are tracked in
  Sensor sensor;
  sensor.AddComponent(&fm);
  sensor.SetArea(-3*pitch, -3*pitch, view_min_z, 3*pitch, 3*pitch, 0.4);

  //Set up the microscopic avalanche
  AvalancheMicroscopic aval;
  aval.SetSensor(&sensor);
  unsigned int anode_counter = 0;

  // How many events we want to run for each composition
  constexpr unsigned int nEvents = 1;

  //starting positions 
  double x0 = 0;
  double y0 = 0;
  const double z0 = 0.32;
  const double t0 = 0.;
  const double e0 = 0.1;

  // Loop for events
  auto start = std::chrono::high_resolution_clock::now();

  for (unsigned int i = 0; i < nEvents; ++i){
    //generate avalanche
    aval.AvalancheElectron(x0, y0, z0, t0, e0, 0., 0., 0.);
    std::cout << "Event " << i + 1 << " of " << nEvents << std::endl;
    for (const auto& electron : aval.GetElectrons()){
      if (electron.path.back().z <= view_min_z + 0.001){
        anode_counter += 1;
      }
    }
  }
  std::ofstream file;
  file.open("/opt/ppd/scratch/szwarcer/paragem/output/" + run_number + ".csv");
  file << anode_counter << std::endl;
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  file << duration.count();
  file.close();
  std::cout << "ALL DONE!" << std::endl;
}