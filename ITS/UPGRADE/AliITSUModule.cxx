/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id: AliITSUModule.cxx 53509 2011-12-10 18:55:52Z masera $ */

#include <TArrayI.h>

#include <stdlib.h>

#include "AliRun.h"
#include "AliITS.h"
#include "AliITSUModule.h"
#include "AliITSUGeomTGeo.h"
using namespace TMath;

ClassImp(AliITSUModule)

//_______________________________________________________________________
//
// Impementation of class AliITSUModule
//
// created by: A. Bouchm, W. Peryt, S. Radomski, P. Skowronski 
//             R. Barbers, B. Batyunia, B. S. Nilsen
// ver 1.0     CERN 16.09.1999
//_______________________________________________________________________
//________________________________________________________________________
// 
// Constructors and deconstructor
//________________________________________________________________________
//
AliITSUModule::AliITSUModule()
:  fHitsM(0)
  ,fGeomTG(0)
{
  // constructor
}

//_________________________________________________________________________
AliITSUModule::AliITSUModule(Int_t index, AliITSUGeomTGeo* tg)
  :fHitsM(new TObjArray())
  ,fGeomTG(tg)
{
  // constructor
  SetIndex(index);
}

//__________________________________________________________________________
AliITSUModule::~AliITSUModule() 
{
  // The destructor for AliITSUModule. Before destoring AliITSUModule
  // we must first destroy all of it's members.
  if (fHitsM) fHitsM->Clear();
  delete fHitsM;
}

//____________________________________________________________________________
AliITSUModule::AliITSUModule(const AliITSUModule &source)
 :TObject(source)
 ,fHitsM(source.fHitsM)
 ,fGeomTG(source.fGeomTG)
{
//     Copy Constructor 
}

//_____________________________________________________________________________
AliITSUModule& AliITSUModule::operator=(const AliITSUModule &source)
{
  //    Assignment operator 
  if (this!=&source) {
    this->~AliITSUModule();
    new(this) AliITSUModule(source);
  }
  return *this;
}

//___________________________________________________________________________
Double_t AliITSUModule::PathLength(const AliITSUHit *itsHit1,const AliITSUHit *itsHit2) 
{
  // path lenght
  Float_t  x1g,y1g,z1g;   
  Float_t  x2g,y2g,z2g;
  Double_t s;
  //
  itsHit1->GetPositionG(x1g,y1g,z1g);
  itsHit2->GetPositionG(x2g,y2g,z2g);
  //
  s = Sqrt( ((Double_t)(x2g-x1g)*(Double_t)(x2g-x1g)) +
	    ((Double_t)(y2g-y1g)*(Double_t)(y2g-y1g)) +
	    ((Double_t)(z2g-z1g)*(Double_t)(z2g-z1g)) );
   return s;
}

//___________________________________________________________________________
void AliITSUModule::PathLength(Float_t x,Float_t y,Float_t z,Int_t status,Int_t &nseg,Float_t &x1,Float_t &y1,Float_t &z1,
			      Float_t &dx1,Float_t &dy1,Float_t &dz1,Int_t &flag) const
{
  // path length
  static Float_t x0,y0,z0;
  //
  if ((status&0x0002)!=0){ // entering
    x0 = x;
    y0 = y;
    z0 = z;
    nseg = 0;
    flag = 1;
  }else{
    x1 = x0;
    y1 = y0;
    z1 = z0;
    dx1 = x-x1;
    dy1 = y-y1;
    dz1 = z-z1;
    nseg++;
    if ((status&0x0004)!=0) flag = 0; //exiting
    if ((status&0x0001)!=0) flag = 2; // inside
    else flag = 2; //inside ?
    x0 = x;
    y0 = y;
    z0 = z;
  } // end if
}

//___________________________________________________________________________
Bool_t AliITSUModule::LineSegmentL(Int_t hitindex,Double_t &a,Double_t &b,Double_t &c,Double_t &d,
				  Double_t &e,Double_t &f,Double_t &de)
{
  // line segment
  AliITSUHit *h1;
  Double_t t;
  //  
  if(hitindex>= fHitsM->GetEntriesFast()) return kFALSE;
  //
  h1 = (AliITSUHit *) (fHitsM->At(hitindex));
  if(h1->StatusEntering()) return kFALSE; // if track entering volume, get index for next step
  de = h1->GetIonization();
  h1->GetPositionL0(a,c,e,t);
  h1->GetPositionL(b,d,f);
  b = b - a;
  d = d - c;
  f = f - e;
  return kTRUE;
}

//___________________________________________________________________________
Bool_t AliITSUModule::LineSegmentG(Int_t hitindex,Double_t &a,Double_t &b,Double_t &c,Double_t &d,
				  Double_t &e,Double_t &f,Double_t &de)
{
  // line segment
  AliITSUHit *h1;
  Double_t t;
  //
  if(hitindex>= fHitsM->GetEntriesFast()) return kFALSE;
  h1 = (AliITSUHit *) (fHitsM->At(hitindex));
  if(h1->StatusEntering()) return kFALSE; // if track entering volume, get index for next step
  de = h1->GetIonization();
  h1->GetPositionG0(a,c,e,t);
  h1->GetPositionG(b,d,f);
  b = b - a;
  d = d - c;
  f = f - e;
  return kTRUE;
}

//___________________________________________________________________________
Bool_t AliITSUModule::LineSegmentL(Int_t hitindex,Double_t &a,Double_t &b,Double_t &c,Double_t &d,
				  Double_t &e,Double_t &f,Double_t &de,Int_t &track)
{
  // line segmente
  AliITSUHit *h1;
  Double_t t;
  //
  if(hitindex>= fHitsM->GetEntriesFast()) return kFALSE;
  //
  h1 = (AliITSUHit *) (fHitsM->At(hitindex));
  if(h1->StatusEntering()){ // if track entering volume, get index for next
    // step
    track = h1->GetTrack();
    return kFALSE;
  } // end if StatusEntering()
    // else stepping
  de = h1->GetIonization();
  h1->GetPositionL0(a,c,e,t);
  h1->GetPositionL(b,d,f);
  b = b - a;
  d = d - c;
  f = f - e;
  track = h1->GetTrack();
  return kTRUE;
}

//___________________________________________________________________________
Bool_t AliITSUModule::LineSegmentG(Int_t hitindex,Double_t &a,Double_t &b,Double_t &c,Double_t &d,
				  Double_t &e,Double_t &f,Double_t &de,Int_t &track)
{
  // line segment
  AliITSUHit *h1;
  Double_t t;
  //
  if(hitindex>= fHitsM->GetEntriesFast()) return kFALSE;
  //
  h1 = (AliITSUHit *) (fHitsM->At(hitindex));
  if(h1->StatusEntering()){ // if track entering volume, get index for next
    // step
    track = h1->GetTrack();
    return kFALSE;
  } // end if StatusEntering()
  de = h1->GetIonization();
  h1->GetPositionG0(a,c,e,t);
  h1->GetPositionG(b,d,f);
  b = b - a;
  d = d - c;
  f = f - e;
  track = h1->GetTrack();
  return kTRUE;
}

//______________________________________________________________________
Bool_t AliITSUModule::MedianHitG(AliITSUHit *h1,AliITSUHit *h2,Float_t &x,Float_t &y,Float_t &z)
{
  // Computes the mean hit location for a set of hits that make up a track
  // passing through a volume. Returns kFALSE untill the the track leaves
  // the volume.
  // median hit
  Float_t x1l=0.,y1l=0.,z1l=0.;
  Float_t x2l=0.,y2l=0.,z2l=0.;
  Float_t xMl,yMl=0,zMl;
  Double_t l[3], g[3];
  
  h1->GetPositionG(x1l,y1l,z1l);
  h2->GetPositionG(x2l,y2l,z2l);
  
  if((y2l*y1l)<0.) {
    xMl = (-y1l / (y2l-y1l))*(x2l-x1l) + x1l;
    zMl = (-y1l / (y2l-y1l))*(z2l-z1l) + z1l;
  } else {
    xMl = 0.5*(x1l+x2l);
    zMl = 0.5*(z1l+z2l);
  }
  
  l[0] = xMl;
  l[1] = yMl;
  l[2] = zMl;
  fGeomTG->LocalToGlobal(h1->GetModule(),l,g);
  x = g[0];
  y = g[1];
  z = g[2];
  return kTRUE;
}

//___________________________________________________________________________
void AliITSUModule::MedianHitG(Int_t index,Float_t hitx1,Float_t hity1,Float_t hitz1,Float_t hitx2,Float_t hity2,Float_t hitz2,
				 Float_t &xMg, Float_t &yMg, Float_t &zMg)
{
  // median hit
  Float_t x1l,y1l,z1l;
  Float_t x2l,y2l,z2l;
  Float_t xMl,yMl=0,zMl;
  Double_t l[3], g[3];
  
  g[0] = hitx1;
  g[1] = hity1;
  g[2] = hitz1;
  fGeomTG->GlobalToLocal(index,g,l);
  x1l = l[0];
  y1l = l[1];
  z1l = l[2];
  
  g[0] = hitx2;
  g[1] = hity2;
  g[2] = hitz2;
  fGeomTG->GlobalToLocal(index,g,l);
  x2l = l[0];
  y2l = l[1];
  z2l = l[2];
  
  if((y2l*y1l)<0.) {
    xMl = (-y1l / (y2l-y1l))*(x2l-x1l) + x1l;
    zMl = (-y1l / (y2l-y1l))*(z2l-z1l) + z1l;
  } else {
    xMl = 0.5*(x1l+x2l);
    zMl = 0.5*(z1l+z2l);
  }
  
  l[0] = xMl;
  l[1] = yMl;
  l[2] = zMl;
  fGeomTG->LocalToGlobal(index,l,g);
  xMg = g[0];
  yMg = g[1];
  zMg = g[2];
}

//___________________________________________________________________________
Bool_t AliITSUModule::MedianHitL( AliITSUHit *itsHit1,AliITSUHit *itsHit2, Float_t &xMl, Float_t &yMl, Float_t &zMl) const
{
  // median hit
  Float_t x1l,y1l,z1l;
  Float_t x2l,y2l,z2l;
  
  itsHit1->GetPositionL(x1l,y1l,z1l);
  itsHit2->GetPositionL(x2l,y2l,z2l);
  
  yMl = 0.0;
  if((y2l*y1l)<0.) {
    xMl = (-y1l / (y2l-y1l))*(x2l-x1l) + x1l;
    zMl = (-y1l / (y2l-y1l))*(z2l-z1l) + z1l;	     
  } else {
    xMl = 0.5*(x1l+x2l);
    zMl = 0.5*(z1l+z2l);
  }
  return kTRUE;
}

//___________________________________________________________________________
void AliITSUModule::MedianHit(Int_t index,Float_t xg,Float_t yg,Float_t zg,Int_t status,
			     Float_t &xMg,Float_t &yMg,Float_t &zMg,Int_t &flag)
{
  // median hit
  static Float_t x1,y1,z1;
  //
  if ((status&0x0002)!=0){ // entering
    x1 = xg;
    y1 = yg;
    z1 = zg;
    flag = 1;
  } else if ((status&0x0004)!=0){ // exiting
    MedianHitG(index,x1,y1,z1,xg,yg,zg,xMg,yMg,zMg);
    flag = 0;
  } // end if
  else  flag = 1;
}

