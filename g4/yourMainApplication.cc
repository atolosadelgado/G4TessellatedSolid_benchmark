

#include <iostream>

#include "G4RunManagerFactory.hh"

#include "G4PhysListFactory.hh"


#include "YourDetectorConstruction.hh"
#include "YourActionInitialization.hh"

#include "G4UImanager.hh"
#include "G4UIExecutive.hh"

#include "G4VisManager.hh"
#include "G4VisExecutive.hh"

#include <chrono>

struct myStopWatch
{
  myStopWatch(std::string i):msg(i){}

  void Start(){start = std::chrono::system_clock::now();}
  void Stop() {stop  = std::chrono::system_clock::now();}
  void Print(){
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    printf("%sTime = %lld ms\n", msg.c_str(), static_cast<long long int>(elapsed.count()));
  }
  std::chrono::time_point<std::chrono::system_clock> start;
  std::chrono::time_point<std::chrono::system_clock> stop;
  std::string msg;
};

int main(int argc, char** argv) {
  
  bool IsTessellated = false;
  int nevents = 1000000;
  int nvertex = 50*50;

  G4UIExecutive* ui  = nullptr; 
  if ( argc == 1 ) {
    // there is no input
    //ui = new G4UIExecutive(argc, argv, "tcsh");
    ui = new G4UIExecutive(argc, argv);
  }
  else {
    for(int i = 1; i<argc; ++i)
    {
      if( 0 == strcmp("-T", argv[i]) )
        IsTessellated = true;
      if( 0 == strcmp("-N", argv[i]) && ((i+1)<argc) )
        nevents = atoi(argv[i+1]);
      if( 0 == strcmp("-V", argv[i]) && ((i+1)<argc) )
        nvertex = atoi(argv[i+1]);


    }
  }

  std::cout << "Configuration:\n";
  std::cout << "\tIsTessellated: " << IsTessellated << "\n";
  std::cout << "\tNumber of evt: " << nevents << "\n";
  std::cout << "\tNumber of vx:  " << nvertex << "\n";


  auto* runManager  = G4RunManagerFactory::CreateRunManager(G4RunManagerType::SerialOnly);
  
  YourDetectorConstruction* detector = new YourDetectorConstruction(YourDetectorConstruction::sphere, IsTessellated, 1*CLHEP::cm, nvertex);
  runManager->SetUserInitialization( detector );

  const G4String physListName = "FTFP_BERT";
  G4PhysListFactory physListFactory;
  G4VModularPhysicsList* physList = physListFactory.GetReferencePhysList( physListName );   
  runManager->SetUserInitialization( physList );

  YourActionInitialization* action = new YourActionInitialization( detector );
  runManager->SetUserInitialization( action );




  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  if ( ui == nullptr) {
    myStopWatch init_chrono("Initialization.\t");
    myStopWatch run_chrono("Tracking.\t");

    init_chrono.Start();
    UImanager->ApplyCommand("/run/initialize");
    init_chrono.Stop();

    run_chrono.Start();
    UImanager->ApplyCommand("/run/beamOn " + std::to_string(nevents));
    run_chrono.Stop();

    init_chrono.Print();
    run_chrono.Print();

  } else {
    // set up visualisation
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();
    // interactive: start session 
    ui->SessionStart();
    delete ui;
  }





  return 0;
} 
