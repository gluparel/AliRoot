//Author:        Anders Strand Vestbo
//Last Modified: 17.08.2001

#include <iostream.h>
#include "AliL3Logging.h"
#include "AliL3ClustFinderNew.h"
#include "AliL3DigitData.h"
#include "AliL3Transform.h"
#include "AliL3SpacePointData.h"

ClassImp(AliL3ClustFinderNew)

AliL3ClustFinderNew::AliL3ClustFinderNew()
{
  fMatch = 4;
  fThreshold =10;
  fDeconvPad = kTRUE;
  fDeconvTime = kTRUE;
}

AliL3ClustFinderNew::AliL3ClustFinderNew(AliL3Transform *transform)
{
  fTransform = transform;
  fMatch = 4;
  fThreshold =10;
  fDeconvTime = kTRUE;
  fDeconvPad = kTRUE;
}

AliL3ClustFinderNew::~AliL3ClustFinderNew()
{


}

void AliL3ClustFinderNew::InitSlice(Int_t slice,Int_t patch,Int_t firstrow, Int_t lastrow,Int_t nmaxpoints)
{
  fNClusters = 0;
  fMaxNClusters = nmaxpoints;
  fCurrentSlice = slice;
  fCurrentPatch = patch;
  fFirstRow = firstrow;
  fLastRow = lastrow;
  fDeconvTime = kTRUE;
  fDeconvPad = kTRUE;
}

void AliL3ClustFinderNew::InitSlice(Int_t slice,Int_t patch,Int_t nmaxpoints)
{
  fNClusters = 0;
  fMaxNClusters = nmaxpoints;
  fCurrentSlice = slice;
  fCurrentPatch = patch;
}

void AliL3ClustFinderNew::SetOutputArray(AliL3SpacePointData *pt)
{
  
  fSpacePointData = pt;
}


void AliL3ClustFinderNew::Read(UInt_t ndigits,AliL3DigitRowData *ptr)
{
  fNDigitRowData = ndigits;
  fDigitRowData = ptr;
  
}

void AliL3ClustFinderNew::ProcessDigits()
{
  //Loop over rows, and call processrow
  
  
  AliL3DigitRowData *tempPt = (AliL3DigitRowData*)fDigitRowData;
  
  for(Int_t i=fFirstRow; i<=fLastRow; i++)
    {
      fCurrentRow = i;
      ProcessRow(tempPt);
      Byte_t *tmp = (Byte_t*)tempPt;
      Int_t size = sizeof(AliL3DigitRowData) + tempPt->fNDigit*sizeof(AliL3DigitData);
      tmp += size;
      tempPt = (AliL3DigitRowData*)tmp;
    }
}



void AliL3ClustFinderNew::ProcessRow(AliL3DigitRowData *tempPt)
{

  UInt_t last_pad = 123456789;

  ClusterData *pad1[2500]; //2 lists for internal memory=2pads
  ClusterData *pad2[2500]; //2 lists for internal memory=2pads
  ClusterData clusterlist[5000]; //Clusterlist

  ClusterData **currentPt; //List of pointers to the current pad
  ClusterData **previousPt; //List of pointers to the previous pad
  currentPt = pad2;
  previousPt = pad1;
  UInt_t n_previous=0,n_current=0,n_total=0;

  //Loop over sequences of this row:
  for(UInt_t bin=0; bin<tempPt->fNDigit; bin++)
    {
      
      AliL3DigitData *data = tempPt->fDigitData;
      if(data[bin].fPad != last_pad)
	{
	  //This is a new pad
	  
	  //Switch:
	  if(currentPt == pad2)
	    {
	      currentPt = pad1;
	      previousPt = pad2;
	      
	    }
	  else 
	    {
	      currentPt = pad2;
	      previousPt = pad1;
	    }
	  n_previous = n_current;
	  n_current = 0;
	  if(bin[data].fPad != last_pad+1)
	    {
	      //this happens if there is a pad with no signal.
	      n_previous = n_current = 0;
	    }
	  last_pad = data[bin].fPad;
	}

      Bool_t new_cluster = kTRUE;

      UInt_t seq_charge=0,seq_average=0;
      
      UInt_t last_charge=0,last_was_falling=0;
      Int_t new_bin=-1;
      if(fDeconvTime)
	{
	redo: //This is a goto.
	  if(new_bin > -1)
	    {
	      bin = new_bin;
	      new_bin = -1;
	    }
	  
	  last_charge=0;
	  last_was_falling = 0;
	}
      

      while(1) //Loop over current sequence
	{
	  //Get the current ADC-value
	  UInt_t charge = data[bin].fCharge;
	  
	  if(fDeconvTime)
	    {
	      //Check if the last pixel in the sequence is smaller than this
	      if(charge > last_charge)
		{
		  if(last_was_falling)
		    {
		      new_bin = bin;
		      break;
		    }
		}
	      else last_was_falling = 1; //last pixel was larger than this
	      last_charge = charge;
	    }
	  
	  //Sum the total charge of this sequence
	  seq_charge += charge;
	  seq_average += data[bin].fTime*charge;
	  
	  //Check where to stop:
	  if(data[bin+1].fPad != data[bin].fPad) //new pad
	    break; 
	  if(data[bin+1].fTime != data[bin].fTime+1) //end of sequence
	    break;
	  
	  bin++;
	}//end loop over sequence
      
      //Calculate mean of sequence:
      Int_t seq_mean=0;
      if(seq_charge)
	seq_mean = seq_average/seq_charge;
      else
	{
	  printf("\nCF: Should not happen\n");
	  seq_mean = 1;
	  seq_charge = 1;
	}
      
      //Calculate mean in pad direction:
      Int_t pad_mean = seq_charge*data[bin].fPad;

      //Compare with results on previous pad:
      for(UInt_t p=0; p<n_previous; p++)
	{
	  Int_t difference = seq_mean - previousPt[p]->fMean;
	  if(difference < -fMatch)
	    break;

	  if(difference <= fMatch)//There is a match here!!
	    {
	      
	      ClusterData *local = previousPt[p];
	      if(fDeconvPad)
		{
		  if(seq_charge > local->fLastCharge)
		    {
		      if(local->fChargeFalling)//The previous pad was falling
			{			
			  break;//create a new cluster
			}		    
		    }
		  else
		    local->fChargeFalling = 1;
		  local->fLastCharge = seq_charge;
		}
	      
	      //Don't create a new cluster, because we found a match
	      new_cluster = kFALSE;
	      
	      //Update cluster on current pad with the matching one:
	      //currentPt[n_current] = previousPt[p];
	      
	      local->fTotalCharge += seq_charge;
	      local->fPad += pad_mean;
	      local->fTime += seq_average;
	      local->fMean = seq_mean;
	      local->fFlags++; //means we have more than one pad 
	      
	      currentPt[n_current] = local;
	      n_current++;
	      
	      break;
	    }//Checking for match at previous pad
	}//Loop over results on previous pad.
      
      if(new_cluster)
	{
	  //Start a new cluster. Add it to the clusterlist, and update
	  //the list of pointers to clusters in current pad.
	  //current pad will be previous pad on next pad.

	  //Add to the clusterlist:
	  ClusterData *tmp = &clusterlist[n_total];
	  tmp->fTotalCharge = seq_charge;
	  tmp->fPad = pad_mean;
	  tmp->fTime = seq_average;
	  tmp->fMean = seq_mean;
	  tmp->fFlags = 0;  //flags for 1 pad clusters
	  if(fDeconvPad)
	    {
	      tmp->fChargeFalling = 0;
	      tmp->fLastCharge = seq_charge;
	    }

	  //Update list of pointers to previous pad:
	  currentPt[n_current] = &clusterlist[n_total];
	  n_total++;
	  n_current++;
	}
      if(fDeconvTime)
	if(new_bin >= 0) goto redo;
      
    }//Loop over digits on this padrow
  
  WriteClusters(n_total,clusterlist);


}

void AliL3ClustFinderNew::WriteClusters(Int_t n_clusters,ClusterData *list)
{
  Int_t thisrow,thissector;
  UInt_t counter = fNClusters;
  
  for(int j=0; j<n_clusters; j++)
    {
      if(!list[j].fFlags) continue; //discard 1 pad clusters
      if(list[j].fTotalCharge < fThreshold) continue; //noise cluster
      Float_t xyz[3];
      
      Float_t fpad=(Float_t)list[j].fPad/(Float_t)list[j].fTotalCharge;
      Float_t ftime=(Float_t)list[j].fTime/(Float_t)list[j].fTotalCharge;
      //printf("padrow %d number of pads %d totalcharge %d\n",fCurrentRow,list[j].fFlags,list[j].fTotalCharge);
      fTransform->Slice2Sector(fCurrentSlice,fCurrentRow,thissector,thisrow);
      fTransform->Raw2Local(xyz,thissector,thisrow,fpad,ftime);
      if(xyz[0]==0) LOG(AliL3Log::kError,"AliL3ClustFinder","Cluster Finder")
	<<AliL3Log::kDec<<"Zero cluster"<<ENDLOG;
      if(fNClusters >= fMaxNClusters)
	{
	  LOG(AliL3Log::kError,"AliL3ClustFinder::WriteClusters","Cluster Finder")
	    <<AliL3Log::kDec<<"Too many clusters"<<ENDLOG;
	  return;
	  }  
      fSpacePointData[counter].fX = xyz[0];
      fSpacePointData[counter].fY = xyz[1];
      fSpacePointData[counter].fZ = xyz[2];
      fSpacePointData[counter].fPadRow = fCurrentRow;
      fSpacePointData[counter].fXYErr = fXYErr;
      fSpacePointData[counter].fZErr = fZErr;
      fSpacePointData[counter].fID = counter
	+((fCurrentSlice&0x7f)<<25)+((fCurrentPatch&0x7)<<22);//uli
      
      fNClusters++;
      counter++;

    }

}
