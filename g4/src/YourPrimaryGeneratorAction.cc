
#include "YourPrimaryGeneratorAction.hh"

#include "YourDetectorConstruction.hh"

#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"

YourPrimaryGeneratorAction::YourPrimaryGeneratorAction( YourDetectorConstruction* det )
: G4VUserPrimaryGeneratorAction(),
  fDetector(det), 
  fGun(nullptr) 
{
  fGun = new G4GeneralParticleSource( );
  G4ParticleDefinition* part = G4ParticleTable::GetParticleTable()->FindParticle( "geantino" );
  fGun->SetParticleDefinition(part);
  fGun->SetNumberOfParticles(1);

  fGun->GetCurrentSource()->GetAngDist()->SetAngDistType("iso");
  fGun->GetCurrentSource()->GetPosDist()->SetCentreCoords(G4ThreeVector(0,0,0));



}

YourPrimaryGeneratorAction::~YourPrimaryGeneratorAction() {
  delete fGun;
}


void YourPrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
  fGun->GeneratePrimaryVertex( event );
}




