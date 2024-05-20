#include "TkSteppingAction.h"
#include "TkDetectorDescription.h"
#include "EventWriter.h"

#include "G4RunManager.hh"

TkSteppingAction::TkSteppingAction(EventWriter* eventWriter) :
  m_eventWriter(eventWriter),
  m_currentVolumeName("World"),
  m_energyAtEntrance(-1.)   
{

  m_detDescr = static_cast<const TkDetectorDescription*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
  
}


TkSteppingAction::~TkSteppingAction()
{

}

void TkSteppingAction::UserSteppingAction(const G4Step* step)
{

  if ( !step->GetPostStepPoint()->GetTouchableHandle()->GetVolume() ) return;

  /// check if the particle is changing volume
  G4LogicalVolume* currVol = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
  G4LogicalVolume* nextVol = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();

  // entering a new volume
  if ( currVol && nextVol && (currVol->GetName() != nextVol->GetName()) ) {
///    
    /// add the info about the exit from the previous volume, only if different from World
    int volNum = -1;
      if ( currVol->GetName()=="World" ) {
	volNum=0;
      }
      else if ( currVol->GetName()=="PCB1" ) {
	volNum=1;
      }
      else if ( currVol->GetName()=="CopperLayer1" ) {
	volNum=2;
      }
      else if ( currVol->GetName()=="GasGap" ) {
	volNum=3;
      }
      else if (currVol->GetName()=="CopperLayer2"){
  volNum=4;
      }
      else if(currVol->GetName()== "PCB2"){
  volNum=5;
      }
    if ( currVol->GetName() != "World" ) {
      m_eventWriter->enDepPdgId(step->GetTrack()->GetDynamicParticle()->GetDefinition()->GetPDGEncoding());
      m_eventWriter->enDepX(step->GetPreStepPoint()->GetPosition().x());
      m_eventWriter->enDepY(step->GetPreStepPoint()->GetPosition().y());
      m_eventWriter->enDepZ(step->GetPreStepPoint()->GetPosition().z());  
      m_eventWriter->enDepPX(step->GetPreStepPoint()->GetMomentum().x());
      m_eventWriter->enDepPY(step->GetPreStepPoint()->GetMomentum().y());
      m_eventWriter->enDepPZ(step->GetPreStepPoint()->GetMomentum().z());  
      m_eventWriter->enDepIsExit(1);
      m_eventWriter->enDepDetId(volNum);
      /// get the energy deposit in the volume
      if( nextVol->GetName() == "CopperLayer2" && step->GetTrack()->GetDynamicParticle()->GetPDGcode() == 22){
      m_eventWriter->enDep(m_energyAtEntrance-step->GetTrack()->GetDynamicParticle()->GetKineticEnergy());
    }}
///
    /// add the information about the entrance in the new volume
    if ( nextVol->GetName() != "World") { 
      m_eventWriter->enDepPdgId(step->GetTrack()->GetDynamicParticle()->GetDefinition()->GetPDGEncoding());
      m_eventWriter->enDepX(step->GetPostStepPoint()->GetPosition().x());
      m_eventWriter->enDepY(step->GetPostStepPoint()->GetPosition().y());
      m_eventWriter->enDepZ(step->GetPostStepPoint()->GetPosition().z());  
      m_eventWriter->enDepPX(step->GetPostStepPoint()->GetMomentum().x());
      m_eventWriter->enDepPY(step->GetPostStepPoint()->GetMomentum().y());
      m_eventWriter->enDepPZ(step->GetPostStepPoint()->GetMomentum().z());  
      m_eventWriter->enDepIsExit(0);
      /// get the energy deposit in the volume
      // m_energyAtEntrance = step->GetPreStepPoint()->GetKineticEnergy();
      m_energyAtEntrance = step->GetTrack()->GetDynamicParticle()->GetKineticEnergy();
      m_eventWriter->enDep(m_energyAtEntrance);
      if(nextVol->GetName() == "GasGap" && step->GetTrack()->GetDynamicParticle()->GetPDGcode() == 22){
        std :: cout << "energy ate entrance is  : " <<  m_energyAtEntrance << std:: endl;   
      }
    }
///    
    m_currentVolumeName = nextVol->GetName();
  }

  // fill truth event
  // get all particles entering each volume
  if (currVol && nextVol ) {
  
    if ( step->GetPostStepPoint()->GetStepStatus()==fGeomBoundary ) {
      //std::cout << "Exiting from a volume: " << currVol->GetName() << std::endl;
      
      int volNum = -1;
      if ( currVol->GetName()=="World" ) {
	volNum=0;
      }
      else if ( currVol->GetName()=="PCB1" ) {
	volNum=1;
      }
      else if ( currVol->GetName()=="CopperLayer1" ) {
	volNum=2;
      }
      else if ( currVol->GetName()=="GasGap" ) {
	volNum=3;
      }
      else if (currVol->GetName()=="CopperLayer2"){
  volNum=4;
      }
      else if(currVol->GetName()== "PCB2"){
  volNum=5;
      }

      /// pdg id of the particle
      int partId = step->GetTrack()->GetDynamicParticle()->GetDefinition()->GetPDGEncoding();
      /// position of the exit point
      G4ThreeVector pos = step->GetPreStepPoint()->GetPosition();
      /// position of the particle production vertex
      G4ThreeVector vPos = step->GetTrack()->GetVertexPosition();
      /// momentum
      G4ThreeVector mom = step->GetTrack()->GetMomentum();
      /// energy
      double ene = step->GetTrack()->GetKineticEnergy();

      m_eventWriter->partVol(volNum);
      m_eventWriter->partId(partId);
      m_eventWriter->partPX(mom.x());
      m_eventWriter->partPY(mom.y());
      m_eventWriter->partPZ(mom.z());
      m_eventWriter->partE(ene);
      m_eventWriter->partPosX(pos.x());
      m_eventWriter->partPosY(pos.y());
      m_eventWriter->partPosZ(pos.z());
      m_eventWriter->partVX(vPos.x());
      m_eventWriter->partVY(vPos.y());
      m_eventWriter->partVZ(vPos.z());
            
    }
    
  }

  
}
