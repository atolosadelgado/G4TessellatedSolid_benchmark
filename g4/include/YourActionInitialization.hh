
#ifndef YOURACTIONINITIALIZATION_HH
#define YOURACTIONINITIALIZATION_HH

#include "G4VUserActionInitialization.hh"

class YourDetectorConstruction;

class YourActionInitialization : public G4VUserActionInitialization {
public:
  YourActionInitialization( YourDetectorConstruction* det);
  virtual ~YourActionInitialization();

  virtual void Build() const;

private:

  YourDetectorConstruction* fDetector;

}; 

#endif 
