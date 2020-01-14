/*
                               __
                           .--()°'.'
 Author: Nathalia Cardona '|, . ,'
                           !_-(_\
 Cuts for single particles
*/

#include "../ROOTFunctions.h"
#include "../DelphesFunctions.h"
#include "./Physics.h"

bool noFilter(ExRootTreeReader *treeReader,
              map<string, TClonesArray *> branchDict,
              int entry,
              vector<bool> &vbfCutsArr,
              vector<bool> &cutsArr)
{
  return true;
}

bool vbfCut(ExRootTreeReader *treeReader,
            map<string, TClonesArray *> branchDict,
            int entry,
            vector<bool> &vbfCutsArr,
            vector<bool> &cutsArr)

{
  bool ans = false;

  // mjj > 700 #
  // eta_lead * eta_subLead < 0 #
  // |dEta| >5.5
  // pt both jets > 30
  // |eta|<5 for both jets

  treeReader->ReadEntry(entry);

  if(cutsArr[entry]){

    // MINIMUM 2 JETS!!

    bool min2JetsBool = min2Jets(treeReader, branchDict, entry);

    if (min2JetsBool)
    {

      Jet *leadingJet = (Jet *)branchDict["Jet"]->At(0);
      Jet *subLeadingJet = (Jet *)branchDict["Jet"]->At(1);

      bool mjjBool = mjj(treeReader, branchDict, entry) > 700;
      bool deltaMultipl = (leadingJet->Eta) * (subLeadingJet->Eta) < 0;
      bool deltaEtaBool = abs(deltaEta(leadingJet, subLeadingJet) )> 5.5;
      bool pTBothBool = leadingJet->PT > 30.0 && subLeadingJet->PT > 30.0;
      bool etaBelow5 = abs(leadingJet->Eta) < 5.0 && abs(subLeadingJet->Eta) < 5.0;

      ans = mjjBool && deltaMultipl && deltaEtaBool && pTBothBool && etaBelow5;
    }
  
  }

  vbfCutsArr[entry] = ans;
  return ans;
}

bool cuts(ExRootTreeReader *treeReader,
          map<string, TClonesArray *> branchDict,
          int entry,
          vector<bool> &vbfCutsArr,
          vector<bool> &cutsArr)
{
  // needs to meet vbf
  // met>100
  // # bjets = 0

  treeReader->ReadEntry(entry);
  bool ans = false;

  // if (vbfCutsArr[entry])
  // {
  bool metBool = met(treeReader, branchDict, entry) > 100;

  if (metBool)
  {
    bool bJetsBool = true;

    for (int leaf = 0; leaf < branchDict["Jet"]->GetEntries(); leaf++)
    {
      Jet *jet = (Jet *)branchDict["Jet"]->At(leaf);
      if (jet->BTag == 1)
      {
        bJetsBool = false;
        break;
      }
    }

    ans = bJetsBool;
  }
  // }

  cutsArr[entry] = ans;
  return ans;
}

bool met(ExRootTreeReader *treeReader,
          map<string, TClonesArray *> branchDict,
          int entry,
          vector<bool> &vbfCutsArr,
          vector<bool> &cutsArr)
{
  // met>150
  
  treeReader->ReadEntry(entry);

  bool metBool = met(treeReader, branchDict, entry) > 150;

  cutsArr[entry] = metBool;
  return metBool;
}

bool bjets(ExRootTreeReader *treeReader,
          map<string, TClonesArray *> branchDict,
          int entry,
          vector<bool> &vbfCutsArr,
          vector<bool> &cutsArr)
{
  // # bjets = 0

  treeReader->ReadEntry(entry);
  
  bool bJetsBool = true;

  if(cutsArr[entry]){


    for (int leaf = 0; leaf < branchDict["Jet"]->GetEntries(); leaf++)
    {
      Jet *jet = (Jet *)branchDict["Jet"]->At(leaf);
      if (jet->BTag == 1)
      {
        bJetsBool = false;
        break;
      }
    }  

  }
  else{
    bJetsBool = false;
  }
  cutsArr[entry] = bJetsBool;
  return bJetsBool;
}
bool nParticle(ExRootTreeReader *treeReader,
                    map<string, TClonesArray *> branchDict,
                    int entry,
                    int n_electrons,
                    int n_muon,
                    int n_tau,
                    vector<bool> &vbfCutsArr,
                    vector<bool> &cutsArr)
{

  /*
    must comply with VBF cuts  & cuts
    n_electrons electron with:
      pt>8 
      abs(eta) < 2.4
    n_tau taus with:
      pt>20
      abs(eta)<2.4
    n_muon muons with:
      pt>5
      abs(eta)<2.4

  */

  treeReader->ReadEntry(entry);

  // vbfcut & cuts
  if (vbfCutsArr[entry])
  {
    // verify electron condition
    int nElectrons = 0;
    int i = 0;
    while (nElectrons <= n_electrons && i < branchDict["Electron"]->GetEntries())
    {
      Electron *elec = (Electron *)branchDict["Electron"]->At(i);
      if (elec->PT >= 8 && elec->PT <= 40 && abs(elec->Eta) < 2.4)
      {
        nElectrons++;
      }
      i++;
    }

    if (nElectrons == n_electrons)
    {
      //verify number of muons
      int nMuons = 0;
      int i = 0;
      while (nMuons <= n_muon && i < branchDict["Muon"]->GetEntries())
      {
        Muon *muon = (Muon *)branchDict["Muon"]->At(i);
        if (muon->PT >= 5 && muon->PT <= 40 && abs(muon->Eta) < 2.4)
        {
          nMuons++;
        }
        i++;
      }

      if (nMuons == n_muon)
      {
        //verify number of taus
        int nTaus = 0;
        int i = 0;
        while (nTaus <= n_tau && i < branchDict["Jet"]->GetEntries())
        {
          Jet *jet = (Jet *)branchDict["Jet"]->At(i);
          if (jet->TauTag == 1)
          {
            if (jet->PT >= 20.0 && abs(jet->Eta) < 2.4)
            {
              nTaus++;
            }
          }
          i++;
        }
        return nTaus == n_tau;
      }
      else
      {
        return false;
      }
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
}

bool cut_e(ExRootTreeReader *treeReader,
           map<string, TClonesArray *> branchDict,
           int entry,
           vector<bool> &vbfCutsArr,
           vector<bool> &cutsArr)
{
  return nParticle(treeReader, branchDict, entry, 1, 0, 0, vbfCutsArr, cutsArr);
}

bool cut_mu(ExRootTreeReader *treeReader,
            map<string, TClonesArray *> branchDict,
            int entry,
            vector<bool> &vbfCutsArr,
            vector<bool> &cutsArr)
{
  return nParticle(treeReader, branchDict, entry, 0, 1, 0, vbfCutsArr, cutsArr);
}

bool cut_tau(ExRootTreeReader *treeReader,
             map<string, TClonesArray *> branchDict,
             int entry,
             vector<bool> &vbfCutsArr,
             vector<bool> &cutsArr)
{
  return nParticle(treeReader, branchDict, entry, 0, 0, 1, vbfCutsArr, cutsArr);
}
