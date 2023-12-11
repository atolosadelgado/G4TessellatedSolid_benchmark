
#include "YourActionInitialization.hh"


#include "YourDetectorConstruction.hh"
#include "YourPrimaryGeneratorAction.hh"


YourActionInitialization::YourActionInitialization( YourDetectorConstruction* det) 
: G4VUserActionInitialization(),
  fDetector(det)
{}

YourActionInitialization::~YourActionInitialization() {}

void YourActionInitialization::Build() const {
  YourPrimaryGeneratorAction* primGen = new YourPrimaryGeneratorAction( fDetector );
  SetUserAction( primGen );

}
