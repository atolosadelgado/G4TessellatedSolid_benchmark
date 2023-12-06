
#ifndef YOURDETECTORCONSTRUCTION_HH
#define YOURDETECTORCONSTRUCTION_HH 

#include "G4VUserDetectorConstruction.hh"

class YourDetectorConstruction : public G4VUserDetectorConstruction {
public:

   enum shape { sphere = 's', cube = 'c' };

   YourDetectorConstruction(shape inputShape, bool isTessellated_input, G4double size, G4int nvertex);
   virtual ~YourDetectorConstruction();


   virtual G4VPhysicalVolume* Construct();

   const G4VPhysicalVolume* GetTargetPhysicalVolume() const { return fTargetPhysicalV; }

   G4double GetGunXPosition() const { return fGunXPosition; }

private:

   // number of vertex in case of tessellated solid
   G4int fNVertex;
   // solid shape
   shape fShape;

   // choose between analytical or tesselated shape
   bool fIsTessellated;

   // solid size
   G4double      fSize;

   // solid size
   G4double      fSeparation;

   // number of solids
   G4int      fNSolids = {10};

   // target physicial volume
   G4VPhysicalVolume* fTargetPhysicalV;

   // the proper X position of the gun
   G4double       fGunXPosition; 
};

#endif 
