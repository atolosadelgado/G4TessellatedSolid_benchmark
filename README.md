GEOMETRY ARTIFACTS
==================

Benchmarking of navigation in tessellated solids. This example creates a tessellated solid which corresponds to a parametrized sphere (number of facets can be tuned), and simulation can be run to control inpact of number of facets on time execution.

# Basic commands

Build with `cmake`, 
```bash
source /cvmfs/sft.cern.ch/lcg/views/dev4/latest/x86_64-centos7-gcc11-opt/setup.sh
cmake -B build -S . -D CMAKE_INSTALL_PREFIX=install
cmake --build build -- install
export LD_LIBRARY_PATH=$PWD/install/lib:$LD_LIBRARY_PATH
```

To display the geometry using ROOT display

```bash
geoDisplay compact/tessellatedSphere_o1_v01.xml
```

To convert the geometry into GDML (output file also part of the repository)

```bash
geoConverter -compact2gdml -input compact/tessellatedSphere_o1_v01.xml -output tessellatedSphere_o1_v01.gdml
```

Visualze geometry using Geant4+Qt

```bash
ddsim --compactFile compact/tessellatedSphere_o1_v01.xml --runType qt --macroFile vis.mac --part.userParticleHandler=''
```

To show materials in along a given line (in this case from (0,0,-10)cm to (100,0,90)cm), only air (world material) is shown. This may point to an issue just with visualization.

```bash
materialScan compact/tessellatedSphere_o1_v01.xml 0 0 -10 100 0 90
```
ddsim --compactFile ./compact/tessellatedSphere_o1_v01.xml -N 10 -G --gun.particle geantino --gun.distribution 'cos(theta)' --gun.thetaMin "0" --gun.thetaMax "1.57*rad" --gun.phiMin "0" --gun.phiMax "1.57*rad" --gun.position "0 0 0" --filter.calo "" --filter.tracker ""  --random.seed 1 --outputFile ts_geantino_distcostheta.root  --part.userParticleHandler=''
