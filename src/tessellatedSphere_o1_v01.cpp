#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Printout.h"
#include "DDRec/DetectorData.h"
#include <XML/Helper.h>

using namespace dd4hep;

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>


TessellatedSolid create_sphere(double R = 1, int nthetatotal = 50, int nphitotal = 50)
{
  struct mypoint
  {
    int theta;
    int phi;
    double x;
    double y;
    double z;
  };


  using Vertex = TessellatedSolid::Vertex;
  std::vector<Vertex> vertices;
  std::vector<mypoint> myvertex_x;

  myvertex_x.reserve(nthetatotal*nphitotal);
  double thetastep = 3.1415/(nthetatotal-1);
  double phistep   = 6.2830/nphitotal;
  std::cout << "Step in theta = " << thetastep << std::endl;
  std::cout << "Step in phi = " << phistep << std::endl;

  ROOT::Math::Polar3D<double> p;

  std::cout << std::setprecision(3) << std::scientific;
  // setup the vertex
  for(int ntheta =0; ntheta<nthetatotal; ++ntheta)
  {
    for(int nphi=0; nphi<nphitotal; ++nphi)
    {
      if( (ntheta==0 || ntheta==(nthetatotal-1)) && nphi !=0) continue;
      p.SetCoordinates(R, ntheta*thetastep, nphi*phistep);
      myvertex_x.push_back({ntheta,nphi,p.x(),p.y(),p.z()});
      vertices.emplace_back(Vertex(p.x(),p.y(),p.z()));

      auto v = myvertex_x.back();
          std::cout << v.theta << '\t'
              << v.phi   << '\t'
              << v.x << '\t'
              << v.y << '\t'
              << v.z << " ++ "
              << ntheta*thetastep << '\t'
              << nphi*phistep << '\n';

    }
    // std::cin.ignore();
  }


  std::cout << "\t ++ Total number of vertex = " << vertices.size() << "\n";
  // std::cin.ignore();

  // define facets
  TessellatedSolid shape("kk", vertices);
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
        shape->AddFacet(v1,v2,v3,v4);
    }
  }
  // create pole z>0, ie, ntheta=0
  {
    int vpole = find_vertex(0,0);

    for(int nphi=0; nphi<nphitotal; ++nphi)
    {
        int v2 = find_vertex(1,nphi);
        int v3 = find_vertex(1,(nphi+1)%nphitotal);
        shape->AddFacet(vpole,v2,v3);
    }
  }
  // create pole z<0, ie, ntheta=0
  {
    int vpole = find_vertex(nthetatotal-1,0);

    for(int nphi=0; nphi<nphitotal; ++nphi)
    {
        int v2 = find_vertex(nthetatotal-2,nphi);
        int v3 = find_vertex(nthetatotal-2,(nphi+1)%nphitotal);
        shape->AddFacet(v2,vpole,v3);
    }
  }

  shape->CloseShape(true, false, true);
  return shape;
}

// create the detector
static Ref_t TessellatedSphere(Detector &desc, xml::Handle_t handle, SensitiveDetector sens)
{
  xml::DetElement detElem = handle;
  std::string detName = detElem.nameStr();
  int detID = detElem.id();
  DetElement det(detName, detID);
  sens.setType("tracker");
  double radius_of_the_sphere = 1*cm;
  // Vessel
  auto shape = create_sphere(radius_of_the_sphere,100,100);
  Volume sphereVol(detName, shape, desc.material("Aluminum"));
  sphereVol.setVisAttributes( "aerogel_vis" );
  sphereVol.setSensitiveDetector(sens);


  double separation = 0.1*radius_of_the_sphere;
  double dpos = 2*radius_of_the_sphere+separation;
  int nshapes = 6;
  int cellCounter = 0;
  Assembly mytest_assembly("mytest_assembly");

  for(int ix=-nshapes/2; ix<nshapes/2; ++ix)
  {
    for(int iy=-nshapes/2; iy<nshapes/2; ++iy)
    {
      for(int iz=-nshapes/2; iz<nshapes/2; ++iz)
      {
        // skip central sphere at 000
        if( (ix==0)&&(iy==0)&&(iz==0) ) continue;
        PlacedVolume spherePV = mytest_assembly.placeVolume(sphereVol, Position( dpos*ix, dpos*iy, dpos*iz));
        spherePV.addPhysVolID("system", detID);
        spherePV.addPhysVolID("modx", ix);
        spherePV.addPhysVolID("mody", iy);
        spherePV.addPhysVolID("modz", iz);
        ++cellCounter;
        DetElement cellDE(det, "DE"+std::to_string(cellCounter), 3 * cellCounter);
        cellDE.setPlacement(spherePV);
      }
    }
  }

  // place mother volume (vessel)
  Volume motherVol = desc.pickMotherVolume(det);
  auto mytest_assemblyPV = motherVol.placeVolume(mytest_assembly);
  det.setPlacement(mytest_assemblyPV);


  return det;
}
DECLARE_DETELEMENT(TessellatedSphere_o1_v01, TessellatedSphere)





