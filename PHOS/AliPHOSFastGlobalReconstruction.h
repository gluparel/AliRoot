#ifndef ALIPHOSFASTGLOBALRECONSTRUCTION_H
#define ALIPHOSFASTGLOBALRECONSTRUCTION_H
/* Copyright(c) 1998-2003, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

// Fast global reconstruction class.
// It performes fast reconstruction for charged particles only,
// assuming that they were detected by all central ALICE detectors but PHOS.
// This class acts as a filter for primary particles, selects them
// and deteriorates their 4-momenta.
//!
// Author: Yuri Kharlov. 17 April 2003

//_________________________________________________________________________
class AliGenerator;
class TClonesArray;
class AliPHOSGetter;

class AliPHOSFastGlobalReconstruction : public TObject {

public:
  AliPHOSFastGlobalReconstruction() {};
  AliPHOSFastGlobalReconstruction(const char* headerFile);
  virtual ~AliPHOSFastGlobalReconstruction();
  void FastReconstruction(Int_t event, Bool_t cut = kFALSE, Double_t etacut = -1, Double_t phicutmin = -1, Double_t phicutmax = -1);
  TClonesArray *GetRecParticles() const {return fParticles;}

private:
  Bool_t  Detected(TParticle *particle);
  Float_t Efficiency(Float_t pt, Float_t eta);
  void    SmearMomentum(TLorentzVector &p);

private:
  AliPHOSGetter *gime;        //! Instance of the PHOS getter
  AliGenerator  *fGenerator;  //! MC generator used in simulation
  TClonesArray  *fParticles;  //! Array of reconstructed particles
  Int_t          fNParticles; //! Number of reconstructed particles

ClassDef(AliPHOSFastGlobalReconstruction,1)  // Fast global reconstruction
};

#endif // AliPHOSFASTGLOBALRECONSTRUCTION_H
