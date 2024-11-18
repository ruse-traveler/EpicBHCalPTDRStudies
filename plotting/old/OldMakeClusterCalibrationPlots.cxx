// ----------------------------------------------------------------------------
// 'OldMakeCalibrationPlots.C'
// Derek Anderson
// 09.13.2023
//
// Macro to plot histograms from 'TrainAndEvaluateBHCalCalibration.C'
//
// NOTE: this an OLD macro and is not maintained!
// ----------------------------------------------------------------------------

// c utilties
#include <vector>
#include <string>
#include <iostream>
// root classes
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TFile.h"
#include "TCanvas.h"

using namespace std;

// global constants
static const UInt_t NTxt(3);
static const UInt_t NVtx(4);
static const UInt_t NHist(4);
static const UInt_t NRange(2);
static const UInt_t NEneBins(7);
static const UInt_t NTmvaVar(28);
static const UInt_t NTmvaSpec(1);



void OldMakeClusterCalibrationPlots() {

  // emit deprecation warning
  cerr << "WARNING: this an old macro that's been deprecated! Use at your own risk!" << endl;

  // announce start
  cout << "\n  Beginning energy plotting macro..." << endl;

  // i/o parameters
  const string sInFile("forBHCalOnlyCheck.evt5Ke120pim_central.d31m10y2024.hists.root");
  const string sOutFile("bhcalOnlyCheck_sumClustEne.evt5Ke120pim_central.d31m10y2024.root");

  // histogram parameters
  const uint32_t fColEneBin[NEneBins] = {799, 809, 899, 909, 879, 889, 859};
  const uint32_t fMarEneBin[NEneBins] = {25,  27,  32,  26,  29,  30,  25};
  const string   sHCalEne[NEneBins] = {
    "hESumBHCal_Ene1",
    "hESumBHCal_Ene2",
    "hESumBHCal_Ene5",
    "hESumBHCal_Ene7",
    "hESumBHCal_Ene10",
    "hESumBHCal_Ene15",
    "hESumBHCal_Ene20"
  };
  const string sEneTitleX("#SigmaE_{clust}^{BHCal} [GeV]");
  const string sTitleY("a. u.");

  // style parameters
  const UInt_t fFil(0);
  const UInt_t fLin(1);
  const UInt_t fTxt(42);
  const UInt_t fAln(12);
  const UInt_t fCenter(1);
  const float  fOffX(1.2);
  const float  fOffY(1.3);
  const string sTitle("");

  // text parameters
  const string sHeader("");
  const string sTxt[NTxt] = {
    "#bf{ePIC} simulation [24.10.0]",
    "single #pi^{-}",
    "#bf{BHCal only}"
  };
  const string sLabel[NEneBins] = {
    "E_{par} = 1 GeV",
    "E_{par} = 2 GeV",
    "E_{par} = 5 GeV",
    "E_{par} = 7 GeV",
    "E_{par} = 10 GeV",
    "E_{par} = 15 GeV",
    "E_{par} = 20 GeV"
  };

  // open files
  TFile* fInput  = new TFile(sInFile.data(), "read");
  TFile* fOutput = new TFile(sOutFile.data(), "recreate");
  if (!fInput || !fOutput) {
    cerr << "PANIC: can't open a file!\n"
         << "       input  = " << fInput << "\n"
         << "       output = " << fOutput
         << endl;
    return;
  }
  cout << "    Opened files." << endl;

  // grab histograms
  TH1D* hHCalEneBin[NEneBins];
  for (UInt_t iEneBin = 0; iEneBin < NEneBins; iEneBin++) {
    hHCalEneBin[iEneBin] = (TH1D*) fInput -> Get(sHCalEne[iEneBin].data());
    if (!hHCalEneBin[iEneBin]) {
      cerr << "PANIC: couldn't grab histogram '" << sHCalEne << "'!" << endl;
      return;
    }
  }
  cout << "    Grabbed histograms." << endl;

  // set histogram styles
  for (UInt_t iEneBin = 0; iEneBin < NEneBins; iEneBin++) {
    hHCalEneBin[iEneBin] -> SetMarkerColor(fColEneBin[iEneBin]);
    hHCalEneBin[iEneBin] -> SetMarkerStyle(fMarEneBin[iEneBin]);
    hHCalEneBin[iEneBin] -> SetLineColor(fColEneBin[iEneBin]);
    hHCalEneBin[iEneBin] -> SetLineStyle(fLin);
    hHCalEneBin[iEneBin] -> SetFillColor(fColEneBin[iEneBin]);
    hHCalEneBin[iEneBin] -> SetFillStyle(fFil);
    hHCalEneBin[iEneBin] -> SetTitle(sTitle.data());
    hHCalEneBin[iEneBin] -> SetTitleFont(fTxt);
    hHCalEneBin[iEneBin] -> GetXaxis() -> SetTitle(sEneTitleX.data());
    hHCalEneBin[iEneBin] -> GetXaxis() -> SetTitleFont(fTxt);
    hHCalEneBin[iEneBin] -> GetXaxis() -> SetTitleOffset(fOffX);
    hHCalEneBin[iEneBin] -> GetXaxis() -> CenterTitle(fCenter);
    hHCalEneBin[iEneBin] -> GetYaxis() -> SetTitle(sTitleY.data());
    hHCalEneBin[iEneBin] -> GetYaxis() -> SetTitleFont(fTxt);
    hHCalEneBin[iEneBin] -> GetYaxis() -> SetTitleOffset(fOffY);
    hHCalEneBin[iEneBin] -> GetYaxis() -> CenterTitle(fCenter);
  }
  cout << "    Set styles of resolution histograms." << endl;

  // make legend
  const UInt_t fColLeg      = 0;
  const UInt_t fFilLeg      = 0;
  const UInt_t fLinLeg      = 0;
  const float  hObjLeg      = NEneBins * 0.05;
  const float  yObjLeg      = 0.1 + hObjLeg;
  const float  fLegXY[NVtx] = {0.1, 0.1, 0.3, yObjLeg};

  TLegend *leg = new TLegend(fLegXY[0], fLegXY[1], fLegXY[2], fLegXY[3], sHeader.data());
  leg -> SetFillColor(fColLeg);
  leg -> SetFillStyle(fFilLeg);
  leg -> SetLineColor(fColLeg);
  leg -> SetLineStyle(fLinLeg);
  leg -> SetTextFont(fTxt);
  leg -> SetTextAlign(fAln);
  for (UInt_t iEneBin = 0; iEneBin < NEneBins; iEneBin++) {
    leg -> AddEntry(hHCalEneBin[iEneBin], sLabel[iEneBin].data(), "pf");
  }
  cout << "    Made legend." << endl;

  // make text
  const UInt_t fColTxt      = 0;
  const UInt_t fFilTxt      = 0;
  const UInt_t fLinTxt      = 0;
  const float  hObjTxt      = NTxt * 0.05;
  const float  yObjTxt      = 0.1 + hObjTxt;
  const float  fTxtXY[NVtx] = {0.3, 0.1, 0.5, yObjTxt};

  TPaveText *txt = new TPaveText(fTxtXY[0], fTxtXY[1], fTxtXY[2], fTxtXY[3], "NDC NB");
  txt -> SetFillColor(fColTxt);
  txt -> SetFillStyle(fFilTxt);
  txt -> SetLineColor(fColTxt);
  txt -> SetLineStyle(fLinTxt);
  txt -> SetTextFont(fTxt);
  txt -> SetTextAlign(fAln);
  for (UInt_t iTxt = 0; iTxt < NTxt; iTxt++) {
    txt -> AddText(sTxt[iTxt].data());
  }
  cout << "    Made text." << endl;

  // plot fit distributions
  const UInt_t width(750);
  const UInt_t height(750);
  const UInt_t fMode(0);
  const UInt_t fBord(2);
  const UInt_t fGrid(0);
  const UInt_t fTick(1);
  const UInt_t fLogX(0);
  const UInt_t fLogY(1);
  const UInt_t fFrame(0);
  const float  fMarginL(0.15);
  const float  fMarginR(0.02);
  const float  fMarginT(0.02);
  const float  fMarginB(0.15);

  TCanvas *cResoEne = new TCanvas("cResoEne", "", width, height);
  cResoEne       -> SetGrid(fGrid, fGrid);
  cResoEne       -> SetTicks(fTick, fTick);
  cResoEne       -> SetBorderMode(fMode);
  cResoEne       -> SetBorderSize(fBord);
  cResoEne       -> SetFrameBorderMode(fFrame);
  cResoEne       -> SetLeftMargin(fMarginL);
  cResoEne       -> SetRightMargin(fMarginR);
  cResoEne       -> SetTopMargin(fMarginT);
  cResoEne       -> SetBottomMargin(fMarginB);
  cResoEne       -> SetLogx(fLogX);
  cResoEne       -> SetLogy(fLogY);
  cResoEne       -> cd();
  hHCalEneBin[0] -> Draw();
  for (UInt_t iEneBin = 1; iEneBin < NEneBins; iEneBin++) {
    hHCalEneBin[iEneBin] -> Draw("same");
  }
  leg      -> Draw();
  txt      -> Draw();
  fOutput  -> cd();
  cResoEne -> Write();
  cResoEne -> Close();
  cout << "    Made resolution plots." << endl;

}

// end ------------------------------------------------------------------------
