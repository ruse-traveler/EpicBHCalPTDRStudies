from DDSim.DD4hepSimulation import DD4hepSimulation
from g4units import mm, GeV, MeV, degree

SIM = DD4hepSimulation()
SIM.gun.energy       = 10*GeV
SIM.gun.particle     = "mu+"
SIM.gun.multiplicity = 1
SIM.gun.position     = (0.0, 0.0, 0.0)
SIM.gun.distribution = "cos(theta)"
SIM.gun.thetaMin     = 33.5*degree
SIM.gun.thetaMax     = 146*degree
