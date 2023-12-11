
#ifndef YOURPRIMARYGENERATORACTION_HH
#define YOURPRIMARYGENERATORACTION_HH

#include "G4VUserPrimaryGeneratorAction.hh"

class YourDetectorConstruction;
class G4GeneralParticleSource;


class YourPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
    YourPrimaryGeneratorAction( YourDetectorConstruction* det);
   virtual ~YourPrimaryGeneratorAction();

    void GeneratePrimaries(G4Event* event) override; 

private:

   YourDetectorConstruction*  fDetector;

   G4GeneralParticleSource*              fGun;

};

#endif 
