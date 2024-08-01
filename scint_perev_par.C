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
using namespace Garfield;

struct Level{
  double energy;
  int gas_index;
  int type;
  std::string descr;
  long double ratio;
  double ar_percent;
  std::string id;
  Level(double e, int g_i, int t, std::string d, long double r, double f){
    energy = e;
    gas_index = g_i;
    type = t;
    descr = d;
    ratio = r;
    ar_percent = f;
    id = std::to_string(g_i) + std::to_string(t) + std::to_string(e);
  }
};

void write_levels(std::vector<Level>& level_list, std::string run_number){
  std::ofstream outfile;
  outfile.open(run_number + ".txt");
  for (Level to_write:level_list){
    std::string id = std::to_string(to_write.gas_index) + std::to_string(to_write.type) + std::to_string(to_write.energy);
    outfile << to_write.ar_percent << "," << to_write.gas_index << "," << to_write.type << "," << to_write.descr << "," << to_write.energy << "," << to_write.ratio << "," << id << std::endl;
  }
  outfile.close();
}
  
double deposited = 0;
std::vector<double> ratio_list(200);

void userHandle(double x, double y, double z, double t,
int type, int level, Medium* m,
double e0, double e1,
double dx0, double dy0, double dz0,
double dx1, double dy1, double dz1){
  ++ratio_list[level];
  deposited += (e0-e1);
}

void scint_perev_par(double ar_percent, int rn){

  std::string run_number = std::to_string(rn);

  //Create MM object for gas and set it up
  MediumMagboltz gas;
  gas.SetComposition("cf4", 100.-ar_percent, "argon", ar_percent);
  gas.SetTemperature(293.15);
  gas.SetPressure(50.);
  gas.Initialise();

  //load ion mobility data
  //gas.LoadIonMobility("/path/to/file");

  //Setup handover from Elmer
  ComponentElmer fm;

  std::string mapDir = "";
  //The path must end in a "/"
  mapDir += "/opt/ppd/scratch/szwarcer/elmertest/GEM7/";
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
  const double view_min_z = -0.03;

  //this is the region electrons are tracked in
  Sensor sensor;
  sensor.AddComponent(&fm);
  sensor.SetArea(-3*pitch, -3*pitch, view_min_z, 3*pitch, 3*pitch, 0.08);

  //Set up the microscopic avalanche
  AvalancheMicroscopic aval;
  aval.SetSensor(&sensor);
  aval.SetUserHandleCollision(userHandle);
  int ne,ni;

  // How many events we want to run for each composition
  constexpr unsigned int nEvents = 25;

  //starting positions 
  double x0 = 0;
  double y0 = 0;
  const double z0 = 0.07;
  const double t0 = 0.;
  const double e0 = 0.1;

  std::vector<Level> level_list;

  //used in GetLevel
  int ngas;
  int type;
  std::string descr;
  double e;

  // Loop for events
  for (unsigned int i = 0; i < nEvents; ++i){

    std::vector<double> ratio_list_backup = ratio_list;
    double deposited_backup = deposited;
    
    //generate avalanche
    aval.AvalancheElectron(x0, y0, z0, t0, e0, 0., 0., 0.);
    aval.GetAvalancheSize(ne, ni);
    std::cout << "Event " << i + 1 << " of " << nEvents << " (" << ne << ")" << std::endl;
    if (ne <= 100){
      std::cout << "rejected event " << i+1 << std::endl;
      ratio_list = ratio_list_backup;
      deposited = deposited_backup;
    }
  }

  //collate level info
  unsigned int num_levels = gas.GetNumberOfLevels();
  for (int level_index=0;level_index<num_levels;++level_index){
    gas.GetLevel(level_index,ngas,type,descr,e);
    long double ratio = ratio_list[level_index]/deposited;
    if (ar_percent >= 99.99){
      ngas = 1;
    }
    else if(ar_percent <= 0.01){
      ngas = 0;
    }

    Level new_level(e,ngas,type,descr,ratio, ar_percent);
    
    //format the description to remove redundant info
    std::string substr1 = "ELOSS";
    std::string substr2 = "ELEVEL";
    size_t pos = 0;
    size_t found1 = new_level.descr.find(substr1);
    size_t found2 = new_level.descr.find(substr2);
    if (found1 != std::string::npos){
      new_level.descr.erase(found1, new_level.descr.length()-found1);
    }
    if (found2 != std::string::npos){
      new_level.descr.erase(found2, new_level.descr.length()-found2);
    }
    level_list.push_back(new_level);
  }

  write_levels(level_list, run_number);
  std::cout << "ALL DONE!" << std::endl;
}