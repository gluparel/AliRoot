#ifndef ALIMUONCHAMBER_H
#define ALIMUONCHAMBER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */
// Revision of includes 07/05/2004

#include <TObject.h>
#include <TObjArray.h>

#include "AliSegmentation.h"
#include "AliMUONResponse.h"

class AliMUONClusterFinderVS;
class AliMUONGeometryModule;

class AliMUONChamber : public TObject
{
 public:
    AliMUONChamber();
    AliMUONChamber(Int_t id);
    virtual ~AliMUONChamber();
    
//
// Get chamber Id
  virtual Int_t   GetId() const {return fId;}
//

// Get chamber Id
  virtual Bool_t  IsSensId(Int_t volId) const;
  
// Initialisation
  virtual void    Init();
// Set z-position of chamber  
  virtual void    SetZ(Float_t Z) {fZ = Z;}
// Get z-position of chamber  
  virtual Float_t Z() const {return fZ;}
// Set inner radius of sensitive volume 
  virtual void SetRInner(Float_t rmin) {frMin=rmin;}
// Set outer radius of sensitive volum  
  virtual void SetROuter(Float_t rmax) {frMax=rmax;}  

// Return inner radius of sensitive volume 
  virtual  Float_t RInner() const      {return frMin;}
// Return outer radius of sensitive volum  
  virtual Float_t ROuter() const       {return frMax;}  
//  
// Set response model
  virtual void    SetResponseModel(AliMUONResponse* thisResponse) {fResponse=thisResponse;}
//  
// Set segmentation model
  virtual void    SetSegmentationModel(Int_t i, AliSegmentation* thisSegmentation) {
      fSegmentation->AddAt(thisSegmentation,i-1);
  }
//  
//  Get pointer to response model
  virtual AliMUONResponse* &ResponseModel(){return fResponse;}
//  
//  Get reference to segmentation model
  virtual AliSegmentation*  SegmentationModel(Int_t isec) {
      return (AliSegmentation *) (*fSegmentation)[isec-1];
  }
  virtual TObjArray* ChamberSegmentation() {return fSegmentation;}

// Get number of segmentation sectors  
  virtual Int_t Nsec() const        {return fnsec;}
// Set number of segmented cathodes (1 or 2)  
  virtual void  SetNsec(Int_t nsec) {fnsec=nsec;}
//
// Member function forwarding to the segmentation and response models
//
// Calculate pulse height from energy loss  
  virtual Float_t IntPH(Float_t eloss) {return fResponse->IntPH(eloss);}
//  
// Ask segmentation if signal should be generated  
  virtual Int_t   SigGenCond(Float_t x, Float_t y, Float_t z);
//
// Initialisation of segmentation for hit  
  virtual void    SigGenInit(Float_t x, Float_t y, Float_t z);
// Initialisation of charge fluctuation for given hit
  virtual void    ChargeCorrelationInit();

// Configuration forwarding
//
// Define signal distribution region
// by number of sigmas of the distribution function
  virtual void   SetSigmaIntegration(Float_t p1)
      {fResponse->SetSigmaIntegration(p1);}
// Set the single electron pulse-height (ADCchan/e)  
  virtual void   SetChargeSlope(Float_t p1)              {fResponse->SetChargeSlope(p1);}
// Set width of charge distribution function  
  virtual void   SetChargeSpread(Float_t p1, Float_t p2) {fResponse->SetChargeSpread(p1,p2);}
// Set maximum ADC count value
  virtual void   SetMaxAdc(Int_t p1)                   {fResponse->SetMaxAdc(p1);}
// Set Pad size
  virtual void   SetPadSize(Int_t isec, Float_t p1, Float_t p2) {
      ((AliSegmentation*) (*fSegmentation)[isec-1])->SetPadSize(p1,p2);
  }
//  
// Cluster formation method (charge disintegration)
  virtual void   DisIntegration(Float_t eloss, Float_t tof,
				Float_t xhit, Float_t yhit, Float_t zhit,
				Int_t& x, Float_t newclust[6][500]);
// Initialize geometry related parameters  
  virtual void    InitGeo(Float_t z);
//
  virtual Float_t DGas() const {return fdGas;}
  virtual Float_t DAlu() const {return fdAlu;}  
  virtual void SetDGas(Float_t DGas) {fdGas = DGas;}
  virtual void SetDAlu(Float_t DAlu) {fdAlu = DAlu;}  
  virtual void SetChargeCorrel(Float_t correl) {fResponse->SetChargeCorrel(correl);}

// geometry  
  void SetGeometry(AliMUONGeometryModule* geometry) {fGeometry = geometry;}
  AliMUONGeometryModule* GetGeometry() const {return fGeometry; }

  
 protected:
  AliMUONChamber(const AliMUONChamber & rChamber);
  // assignment operator  
  AliMUONChamber& operator =(const AliMUONChamber& rhs);

  Int_t   fId;   // chamber number
  Float_t fdGas; // half gaz gap
  Float_t fdAlu; // half Alu width  
  Float_t fZ;    // Z position (cm)
  Int_t   fnsec; // number of semented cathode planes
  Float_t frMin; // innermost sensitive radius
  Float_t frMax; // outermost sensitive radius
  Float_t fCurrentCorrel; //! charge correlation for current hit.

  TObjArray              *fSegmentation;    // pointer to segmentation
  AliMUONResponse        *fResponse;        // pointer to response
  AliMUONGeometryModule  *fGeometry;        // pointer to geometry
  ClassDef(AliMUONChamber,3) // Muon tracking chamber class
};

#endif
