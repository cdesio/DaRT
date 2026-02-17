//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"
#include "Randomize.hh"
#include "G4GeneralParticleSource.hh"
#include "CommandLineParser.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "G4AnalysisManager.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"

using namespace G4DNAPARSER;
using CLHEP::nanometer;

namespace
{
  G4Mutex messangerInit = G4MUTEX_INITIALIZER;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction(G4String PS_data)
    : G4VUserPrimaryGeneratorAction(), fpParticleGun(nullptr), fPS_data(PS_data)
{
  CommandLineParser *parser = CommandLineParser::GetParser();
  Command *command(0);
  if ((parser->GetCommandIfActive("-decayPS"))||(parser->GetCommandIfActive("-photonPS")))
  {
    G4int n_particle = 1;
    fParticleGun = new G4ParticleGun(n_particle);
    fParticleGun->SetParticleEnergy(0);
    fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., 0.));
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(1., 0., 0.));
  }
  else
  {
    fpParticleGun = new G4GeneralParticleSource();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent)
{
  CommandLineParser *parser = CommandLineParser::GetParser();
  Command *command(0);
  if (parser->GetCommandIfActive("-decayPS"))
  {
    G4long eventNum = anEvent->GetEventID();

    std::ifstream ps_file (fPS_data, std::ifstream::binary);

    ps_file.seekg(eventNum*12*8, ps_file.beg); // find position of event data. 12 doubles which are each 8 bytes

    double line[12];

    ps_file.read((char *)&line, sizeof line);

    ps_file.close();
    G4double positionX = line[0];
    G4double positionY = line[1];
    G4double positionZ = line[2];
    G4double momentumX = line[3];
    G4double momentumY = line[4];
    G4double momentumZ = line[5];
    G4double particleEnergy = line[6];
    part1_EventNum = line[7];
    primaryParticle = line[8];
    part1_CopyNum = line[9];
    part1_Time = line[10];
    part1_particleSource = line[11];

    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition *particle;

    if (primaryParticle == 1)
    {
      particle = particleTable->FindParticle("e-");
      primaryName = "e-";
    }
    else if (primaryParticle == 2)
    {
      particle = particleTable->FindParticle("gamma");
      primaryName = "gamma";
    }
    else if (primaryParticle == 3)
    {
      particle = particleTable->FindParticle("alpha");
      primaryName = "alpha";
    }
    else if (primaryParticle == 4)
    {
      particle = G4IonTable::GetIonTable()->GetIon(86, 220, 0);
      primaryName = "Rn220";
    }
    else if (primaryParticle == 5)
    {
      particle = G4IonTable::GetIonTable()->GetIon(84, 216, 0);
      primaryName = "Po216";
    }
    else if (primaryParticle == 6)
    {
      particle = G4IonTable::GetIonTable()->GetIon(82, 212, 0);
      primaryName = "Pb212";
    }
    else if (primaryParticle == 7)
    {
      particle = G4IonTable::GetIonTable()->GetIon(83, 212, 0);
      primaryName = "Bi212";
    }
    else if (primaryParticle == 8)
    {
      particle = G4IonTable::GetIonTable()->GetIon(81, 208, 0);
      primaryName = "Tl208";
    }
    else if (primaryParticle == 9)
    {
      particle = G4IonTable::GetIonTable()->GetIon(84, 212, 0);
      primaryName = "Po212";
    }
    else if (primaryParticle == 10)
    {
      particle = G4IonTable::GetIonTable()->GetIon(82, 208, 0);
      primaryName = "Pb208";
    }
    if (primaryParticle == 11)
    {
      particle = particleTable->FindParticle("e+");
      primaryName = "e+";
    }
    // G4cout << primaryName << " position = " << G4ThreeVector(positionX, positionY, positionZ) << " momentum = " << G4ThreeVector(momentumX, momentumY, momentumZ) << " energy = " << particleEnergy << "Ra event number = " << part1_EventNum << " copy number = " << part1_CopyNum << " source particle = " << part1_particleSource <<G4endl;
    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticlePosition(G4ThreeVector(positionX, positionY, positionZ));
    fParticleGun->SetParticleEnergy(particleEnergy);
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(momentumX, momentumY, momentumZ));

    fParticleGun->GeneratePrimaryVertex(anEvent);

  }
  else if (parser->GetCommandIfActive("-photonPS"))
  {
    G4long eventNum = anEvent->GetEventID();


    std::ifstream ps_file (fPS_data, std::ifstream::binary);

    ps_file.seekg(eventNum*8*8, ps_file.beg); // find position of event data. 8 doubles which are each 8 bytes

    double line[8];

    ps_file.read((char *)&line, sizeof line);

    ps_file.close();

    G4double positionX = line[0];
    G4double positionY = line[1];
    G4double positionZ = line[2];
    G4double momentumX = line[3];
    G4double momentumY = line[4];
    G4double momentumZ = line[5];
    G4double particleEnergy = line[6];
    part1_EventNum = line[7];

    // G4cout <<  " position = " << G4ThreeVector(positionX, positionY, positionZ) << " momentum = " << G4ThreeVector(momentumX, momentumY, momentumZ) << " energy = " << particleEnergy << " Co60 event number = " << part1_EventNum <<G4endl;


    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition *particle = particleTable->FindParticle("e-");
    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticlePosition(G4ThreeVector(positionX, positionY, positionZ));
    fParticleGun->SetParticleEnergy(particleEnergy);
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(momentumX, momentumY, momentumZ));

    fParticleGun->GeneratePrimaryVertex(anEvent);
  }
  else
  {
    fpParticleGun->GeneratePrimaryVertex(anEvent);
    primaryName = fpParticleGun->GetCurrentSource()->GetParticleDefinition()->GetParticleName();
  }
}