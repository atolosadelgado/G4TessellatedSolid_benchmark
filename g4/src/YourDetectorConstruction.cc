
#include "YourDetectorConstruction.hh"

#include "G4Box.hh"
#include "G4Orb.hh"
#include "G4TessellatedSolid.hh"
#include "G4QuadrangularFacet.hh"
#include "G4TriangularFacet.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4NistManager.hh"

#include "G4SystemOfUnits.hh"

#include "G4RunManager.hh"

void OrbTessellated(G4TessellatedSolid * shape, double R = 1, int nthetatotal = 50, int nphitotal = 50)
{
  struct mypoint
  {
    int theta;
    int phi;
    double x;
    double y;
    double z;
    void SetCoordinates(double R, double theta, double phi)
    {
      x = R*sin(theta)*cos(phi);
      y = R*sin(theta)*sin(phi);
      z = R*cos(theta);
    }
  };


  std::vector<G4ThreeVector> vertices;
  std::vector<mypoint> myvertex_x;

  myvertex_x.reserve(nthetatotal*nphitotal);
  double thetastep = 3.1415/(nthetatotal-1);
  double phistep   = 6.2830/nphitotal;
  std::cout << "Step in theta = " << thetastep << std::endl;
  std::cout << "Step in phi = " << phistep << std::endl;
  // setup the vertex
  for(int ntheta =0; ntheta<nthetatotal; ++ntheta)
  {
    for(int nphi=0; nphi<nphitotal; ++nphi)
    {
      if( (ntheta==0 || ntheta==(nthetatotal-1)) && nphi !=0) continue;

      mypoint p;
      p.phi = nphi;
      p.theta = ntheta;
      p.SetCoordinates(R, ntheta*thetastep, nphi*phistep);
      myvertex_x.push_back(p);

      vertices.emplace_back(G4ThreeVector(p.x,p.y,p.z));
    }
  }


  std::cout << "\t ++ Total number of vertex = " << vertices.size() << "\n";
  // std::cin.ignore();

  // define facets
  auto find_vertex = [&](int ntheta, int nphi){int val=0; for(auto v : myvertex_x){if(v.theta==ntheta && v.phi==nphi){break;}++val;} return val;};
  // create facets on the sides, poles are singular cases
  for(int ntheta =1; ntheta<nthetatotal-2; ++ntheta)
  {
    for(int nphi=0; nphi<nphitotal; ++nphi)
    {
        int v1 = find_vertex(ntheta,nphi);
        int v2 = find_vertex(ntheta+1,nphi);
        int v3 = find_vertex(ntheta+1,(nphi+1)%nphitotal);
        int v4 = find_vertex(ntheta,(nphi+1)%nphitotal);
        auto myFacet = new G4QuadrangularFacet(vertices[v1],vertices[v2],vertices[v3],vertices[v4],ABSOLUTE );
        shape->AddFacet((G4VFacet*) myFacet);
    }
  }
  // create pole z>0, ie, ntheta=0
  {
    int vpole = find_vertex(0,0);

    for(int nphi=0; nphi<nphitotal; ++nphi)
    {
        int v2 = find_vertex(1,nphi);
        int v3 = find_vertex(1,(nphi+1)%nphitotal);
        auto myFacet = new G4TriangularFacet(vertices[vpole],vertices[v2],vertices[v3],ABSOLUTE);
        shape->AddFacet((G4VFacet*) myFacet);
    }
  }
  // create pole z<0, ie, ntheta=0
  {
    int vpole = find_vertex(nthetatotal-1,0);

    for(int nphi=0; nphi<nphitotal; ++nphi)
    {
        int v2 = find_vertex(nthetatotal-2,nphi);
        int v3 = find_vertex(nthetatotal-2,(nphi+1)%nphitotal);
        auto myFacet = new G4TriangularFacet(vertices[v2],vertices[vpole],vertices[v3],ABSOLUTE);
        shape->AddFacet((G4VFacet*) myFacet);
    }
  }

  shape->SetSolidClosed(true);
  return;
}

YourDetectorConstruction::YourDetectorConstruction(shape inputShape, bool isTessellated_input, G4double size, G4int nvertex)
: G4VUserDetectorConstruction() {

  fNVertex = nvertex;
  fSize = size;
  fSeparation = 0.05*size;
  fShape = inputShape;
  fIsTessellated = isTessellated_input;
  fTargetPhysicalV = nullptr;  
  fGunXPosition    = 0.0;
}

YourDetectorConstruction::~YourDetectorConstruction() {}

G4VPhysicalVolume* YourDetectorConstruction::Construct() {
  G4cout << " === YourDetectorConstruction::Construct() === " << G4endl;
  // I. Create/get materials (only for World) 
  G4Material* matWorld = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
  
  // II. Create the geometry
  // 1. define world size (full lengths !!!)
  G4double worldSize   = (fNSolids+2)*(fSize+fSeparation);

  // 2. Create the world (a box at (0,0,0))
  G4Box* worldSolid = new G4Box("solid-world", 
                                 0.5*worldSize,
                                 0.5*worldSize,
                                 0.5*worldSize);
  G4LogicalVolume* worldLogical = new G4LogicalVolume(worldSolid, matWorld, "logic-world");
  G4VPhysicalVolume* worldPhysical = new G4PVPlacement(nullptr, 
                                                       G4ThreeVector(0,0,0),
                                                       worldLogical, 
                                                       "world",
                                                        nullptr,
                                                        false, 0);
  // 3. Create the target inside the world
  G4VSolid* targetSolid;
  if( fIsTessellated )
  {
    targetSolid = new G4TessellatedSolid("solid-target");

    if( shape::sphere == fShape) OrbTessellated( (G4TessellatedSolid*)targetSolid, 0.5*fSize, sqrt(fNVertex), sqrt(fNVertex) );
    if( shape::cube == fShape)   throw std::runtime_error("Not yet ready :)");
  }
  else
  {
    if( shape::sphere == fShape) targetSolid = new G4Orb("solid-target", 0.5*fSize);
    if( shape::cube == fShape)   targetSolid = new G4Box("solid-target", 0.5*fSize, 0.5*fSize, 0.5*fSize);
  }
  G4LogicalVolume* targetLogical = new G4LogicalVolume(targetSolid, matWorld, "logic-target");

  auto xstep = fSeparation+fSize;
  for( int ix= -fNSolids/2; ix<= fNSolids/2; ++ix)
  {
    for( int iy= -fNSolids/2; iy<= fNSolids/2; ++iy)
    {
      for( int iz= -fNSolids/2; iz<= fNSolids/2; ++iz)
      {
        new G4PVPlacement(nullptr,
                          G4ThreeVector(ix*xstep,iy*xstep,iz*xstep),
                          targetLogical,
                          "target",
                            worldLogical,
                            false, 0);
      }
    }

  }


  // 4. Return the ptr to the world physical volume
  return worldPhysical;
}













