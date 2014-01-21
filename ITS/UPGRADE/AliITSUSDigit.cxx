/**************************************************************************
 * Copyright(c) 1998-2004, ALICE Experiment at CERN, All rights reserved. *
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
#include <TMath.h>
#include "AliLog.h"
#include "AliITSUSDigit.h"
ClassImp(AliITSUSDigit)

using std::istream;
using std::swap;
using namespace TMath;

// Addapted from ITS/AliITSpListItem, ruben.shahoyan@cern.ch

//______________________________________________________________________
AliITSUSDigit::AliITSUSDigit() 
: fModule(0)
  ,fNTracks(0)
  ,fTsignal(0.0)
  ,fNoise(0.0)
  ,fSignalAfterElect(0.0)
{
  // Default constructor
}

//______________________________________________________________________
AliITSUSDigit::AliITSUSDigit(UInt_t module,UInt_t index,Double_t noise) 
  :fModule(module)
  ,fNTracks(0)
  ,fTsignal(0.0)
  ,fNoise(noise)
  ,fSignalAfterElect(0.0)
{
  // Standard noise constructor
  SetUniqueID(index);
  for (int i=kBuffSize;i--;) {
    fTrack[i] = -2;
    fHits[i] = -1;
    fSignal[i] = 0;
  }
}

//______________________________________________________________________
AliITSUSDigit::AliITSUSDigit(Int_t track,Int_t hit,UInt_t module,UInt_t index,Double_t signal)
  :fModule(module)
  ,fNTracks(1)
  ,fTsignal(signal)
  ,fNoise(0.0)
  ,fSignalAfterElect(0.0)
{
  // Standard signal constructor
  // Inputs:
  //    Int_t track     The track number which produced this signal
  //    Int_t hit       The hit number which produced this signal
  //    Int_t module    The module where this signal occurred
  //    Int_t index     The cell index where this signal occurred
  //    Double_t signal The value of the signal (ionization)
  SetUniqueID(index);
  fTrack[0]  = track;
  fHits[0]   = hit;
  fSignal[0] = signal;
  for (int i=1;i<kBuffSize;i++) {
    fTrack[i] = -2;
    fHits[i] = -1;
    fSignal[i] = 0;
  }
}

//______________________________________________________________________
AliITSUSDigit& AliITSUSDigit::operator=(const AliITSUSDigit &source)
{
  // = operator
  if (&source!=this) {
    this->~AliITSUSDigit();
    new(this) AliITSUSDigit(source);
  }
  return *this;
  //
}

//______________________________________________________________________
AliITSUSDigit::AliITSUSDigit(const AliITSUSDigit &source) 
  :TObject(source)
  ,fModule(source.fModule)
  ,fNTracks(source.fNTracks)
  ,fTsignal(source.fTsignal)
  ,fNoise(source.fNoise)
  ,fSignalAfterElect(source.fSignalAfterElect)
{
  // Copy operator
  for(Int_t i=kBuffSize;i--;) {
    fTrack[i]  = source.fTrack[i];
    fSignal[i] = source.fSignal[i];
    fHits[i]   = source.fHits[i];
  } // end if i
  //
}

//______________________________________________________________________
void AliITSUSDigit::AddSignal(Int_t track,Int_t hit,Double_t signal)
{
  // Adds this track number and signal to the pList and orders them
  // Inputs:
  //    Int_t track     The track number which produced this signal
  //    Int_t hit       The hit number which produced this signal
  //    Int_t module    The module where this signal occurred
  //    Int_t index     The cell index where this signal occurred
  //    Double_t signal The value of the signal (ionization)
  Int_t    i,j;
  Bool_t   flg=kFALSE;
  //
  if (Abs(signal)>2147483647.0) {
    //PH 2147483647 is the max. integer
    //PH This apparently is a problem which needs investigation
    AliWarning(Form("Too big or too small signal value %f",signal));
    signal = Sign((Double_t)2147483647,signal);
  }
  //
  fTsignal += signal; // Keep track of sum signal.
  for (i=fNTracks;i--;) {
    if ( track==fTrack[i]  ) {
      fSignal[i] += signal;
      flg = kTRUE;
      break;
    } // end for i & if.
  }
  //
  if (flg) {
    if (fNTracks>1) { // resort arrays.  
      for (i=1;i<fNTracks;i++) {
	j = i;
	while(j>0 && fSignal[j]>fSignal[j-1]) {
	  std::swap(fTrack[j-1],fTrack[j]);
	  std::swap(fHits[j-1] ,fHits[j]);
	  std::swap(fSignal[j-1],fSignal[j]);
	  j--;
	} // end while
      } // end if i
    } // end if added to existing and resorted array
    return;
  }
  //
  // new entry add it in order.
  // if this signal is <= smallest then don't add it.
  if (fNTracks==(kBuffSize-1) && signal <= fSignal[kBuffSize-1]) return;
  //
  for (i=fNTracks;i--;) {
    if (signal > fSignal[i]) {
      fSignal[i+1] = fSignal[i];
      fTrack[i+1]  = fTrack[i];
      fHits[i+1]   = fHits[i];
    } else {
      fSignal[i+1] = signal;
      fTrack[i+1]  = track;
      fHits[i+1]   = hit;
      fNTracks++;
      return; // put it in the right place, now exit.
    } //  end if
  } // end if; end for i
  //
  // Still haven't found the right place. Must be at top of list.
  fSignal[0] = signal;
  fTrack[0]  = track;
  fHits[0]   = hit;
  fNTracks++;
  return;
}

//______________________________________________________________________
void AliITSUSDigit::Add(const AliITSUSDigit *pl)
{
  // Adds the contents of pl to this
  // pl could come from different module and index 
  Double_t sigT = 0.0;
  for(int i=pl->GetNTracks();i--;) {
    double sig = pl->GetSignal(i); 
    AddSignal(pl->GetTrack(i),pl->GetHit(i),sig);
    sigT += sig;
  } // end for i
  fTsignal += (pl->fTsignal - sigT);
  fNoise   += pl->fNoise;
  return;
  //
}

//______________________________________________________________________
void AliITSUSDigit::AddTo(Int_t fileIndex,const AliITSUSDigit *pl) 
{
  // Adds the contents of pl to this with track number off set given by
  // fileIndex.
  // Inputs:
  //    Int_t fileIndex      track number offset value
  //    AliITSUSDigit *pl  an AliITSUSDigit to be added to this class.
  //
  for (int i=pl->GetNTracks();i--;) AddSignal(pl->GetTrack(i)+fileIndex,pl->GetHit(i),pl->GetSignal(i));
  fSignalAfterElect += (pl->fSignalAfterElect + pl->fNoise - fNoise);
  fNoise = pl->fNoise;
}

//______________________________________________________________________
void AliITSUSDigit::ShiftIndices(Int_t fileIndex)
{
  // Shift track numbers
  //
  for (int i=GetNTracks();i--;) fTrack[i] += fileIndex;
}

//______________________________________________________________________
Int_t AliITSUSDigit::Compare(const TObject* obj) const
{
  // compare objects
  if (GetUniqueID()<obj->GetUniqueID()) return -1;
  if (GetUniqueID()>obj->GetUniqueID()) return  1;
  return 0;
}

//______________________________________________________________________
void AliITSUSDigit::Print(Option_t*) const 
{
  // print itself
  printf("Mod: %4d Index:%7d Ntr:%2d | TotSignal:%.2e Noise:%.2e |",
	 fModule,GetUniqueID(),fNTracks,fTsignal,fNoise);
  for (int i=0;i<fNTracks;i++) printf("%d(%.2e) |",fTrack[i],fSignal[i]); printf("\n");
}