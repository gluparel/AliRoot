#ifndef ALIPHOSRECPARTICLE_H
#define ALIPHOSRECPARTICLE_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//_________________________________________________________________________
//  A Reconstructed Particle in PHOS    
//  To become a general class of AliRoot ?        
//  why not      
//*-- Author: Yves Schutz (SUBATECH)

// --- ROOT system ---

// --- Standard library ---

// --- AliRoot header files ---

#include "AliPHOSFastRecParticle.h"
#include "AliESDtrack.h" 

class TParticle ;
#include  "TVector3.h"  

class AliPHOSRecParticle : public AliPHOSFastRecParticle {

 public:
  
  AliPHOSRecParticle() ; 
  AliPHOSRecParticle(const AliPHOSRecParticle & rp) ;  // ctor
  virtual ~AliPHOSRecParticle(){  }

  Int_t   GetPHOSTSIndex()const {    return fPHOSTrackSegment ;  }
  virtual const Int_t GetNPrimariesToRecParticles() const ;
  virtual const Int_t GetNPrimaries() const ;
  TVector3 GetPos() const { return fPos ; } 
  virtual const TParticle * GetPrimary(Int_t index) const ;
  const Double_t *GetPID();
  void    SetDebug() { fDebug = kTRUE ; } 
  void    SetPos(TVector3 pos) { fPos.SetXYZ( pos.X(), pos.Y(), pos.Z() ); } 
  void    UnsetDebug() { fDebug = kFALSE ; }
  void    SetTrackSegment(Int_t index){fPHOSTrackSegment = index; }

  typedef TClonesArray RecParticlesList ; 
  
 private:

  Int_t fPHOSTrackSegment ; // pointer to the associated track segment in PHOS  
  Bool_t fDebug ; // to steer debug output
  TVector3 fPos ; // position in the global alice coordinate system 
  Double_t fPID[AliESDtrack::kSPECIES+4] ; // PID probability densities

  ClassDef(AliPHOSRecParticle,3)  // Reconstructed Particle
};

#endif // AliPHOSRECPARTICLE_H
