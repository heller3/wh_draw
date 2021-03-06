
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <unistd.h>
#include <getopt.h>

#include "TError.h"
#include "TColor.h"

#include "core/baby.hpp"
#include "core/process.hpp"
#include "core/named_func.hpp"
#include "core/plot_maker.hpp"
#include "core/plot_opt.hpp"
#include "core/palette.hpp"
#include "core/table.hpp"
#include "core/event_scan.hpp"
#include "core/hist1d.hpp"
#include "core/hist2d.hpp"
#include "core/utilities.hpp"
#include "core/functions.hpp"
#include "core/wh_functions.hpp"

using namespace std;
using namespace PlotOptTypes;
using namespace WH_Functions;

namespace{
  bool single_thread = false;
}


int main(){
  gErrorIgnoreLevel = 6000;
  

  double lumi = 140;

  string data_dir = "/home/users/rheller/wh_babies/babies_v31_1_2019_04_03/";
  string mc_dir = "/home/users/rheller/wh_babies/babies_v30_9_2019_04_03/";
  string signal_dir = "/home/users/rheller/wh_babies/babies_signal_2019_04_03/";

  string basic_cuts =            "nvetoleps==1&&PassTrackVeto&&PassTauVeto&&ngoodjets==2";

  string hostname = execute("echo $HOSTNAME");
  if(Contains(hostname, "cms") || Contains(hostname,"compute-")){
    signal_dir = "/net/cms29";
  }
 
  Palette colors("txt/colors.txt", "default");

  auto data = Process::MakeShared<Baby_full>("Data", Process::Type::data, colors("data"),
    {data_dir+"*data_2016*.root"},"pass&&(HLT_SingleEl==1||HLT_SingleMu==1)");
  auto tt1l = Process::MakeShared<Baby_full>("t#bar{t} (1l)", Process::Type::background, colors("tt_1l"),
    {mc_dir+"*TTJets_1lep*ext1*.root"});
  auto tt2l = Process::MakeShared<Baby_full>("t#bar{t} (2l)", Process::Type::background, colors("tt_2l"),
    {mc_dir+"*TTJets_2lep*ext1*.root"});
  auto wjets_low_nu = Process::MakeShared<Baby_full>("W+jets, nu pT $<$ 200", Process::Type::background, colors("qcd"),
    {mc_dir+"*slim_W*JetsToLNu_s16v3*.root"},NHighPtNu==0.);
  auto wjets_high_nu = Process::MakeShared<Baby_full>("W+jets, nu pT $>=$ 200", Process::Type::background, colors("wjets"),
    {mc_dir+"*slim_W*Jets_NuPt200*.root"});
  auto single_t = Process::MakeShared<Baby_full>("Single t", Process::Type::background, colors("single_t"),
    {mc_dir+"*_ST_*.root"});
  auto ttv = Process::MakeShared<Baby_full>("t#bar{t}V", Process::Type::background, colors("ttv"),
    {mc_dir+"*_TTWJets*.root", mc_dir+"*_TTZ*.root"});
  auto diboson = Process::MakeShared<Baby_full>("Diboson", Process::Type::background, colors("other"),
    {mc_dir+"*WW*.root", mc_dir+"*WZ*.root",mc_dir+"*ZZ*.root"});
  auto other = Process::MakeShared<Baby_full>("Other", Process::Type::background, colors("dy"),
    {mc_dir+"*_TTWJets*.root", mc_dir+"*_TTZ*.root", mc_dir+"*WW*.root", mc_dir+"*WZ*.root",mc_dir+"*ZZ*.root"});

   auto tchiwh_225_75 = Process::MakeShared<Baby_full>("TChiWH(225,75)", Process::Type::signal, colors("t1tttt"),
    {signal_dir+"*SMS-TChiWH_WToLNu_HToBB_TuneCUETP8M1_13TeV-madgraphMLM-pythia8*.root"},"mass_stop==225&&mass_lsp==75");

   auto tchiwh_250_1 = Process::MakeShared<Baby_full>("TChiWH(250,1)", Process::Type::signal, colors("t1tttt"),
    {signal_dir+"*SMS-TChiWH_WToLNu_HToBB_TuneCUETP8M1_13TeV-madgraphMLM-pythia8*.root"},"mass_stop==250&&mass_lsp==1");

   auto tchiwh_350_100 = Process::MakeShared<Baby_full>("TChiWH(350,100)", Process::Type::signal, colors("t1tttt"),
    {signal_dir+"*SMS-TChiWH_WToLNu_HToBB_TuneCUETP8M1_13TeV-madgraphMLM-pythia8*.root"},"mass_stop==350&&mass_lsp==100");

  auto tchiwh_nc = Process::MakeShared<Baby_full>("TChiWH(500,1)", Process::Type::signal, colors("t1tttt"),
    {signal_dir+"*SMS-TChiWH_WToLNu_HToBB_TuneCUETP8M1_13TeV-madgraphMLM-pythia8*.root"},"mass_stop==500&&mass_lsp==1");
  tchiwh_nc->SetMarkerStyle(21);
  tchiwh_nc->SetMarkerSize(0.9);
  auto tchiwh_c = Process::MakeShared<Baby_full>("TChiWH(500,125)", Process::Type::signal, colors("t1tttt"),
    {signal_dir+"*SMS-TChiWH_WToLNu_HToBB_TuneCUETP8M1_13TeV-madgraphMLM-pythia8*.root"},"mass_stop==500&&mass_lsp==125");
  auto tchiwh_700_1 = Process::MakeShared<Baby_full>("TChiWH(700,1)", Process::Type::signal, colors("t1tttt"),
    {signal_dir+"*SMS-TChiWH_WToLNu_HToBB_TuneCUETP8M1_13TeV-madgraphMLM-pythia8*.root"},"mass_stop==700&&mass_lsp==1");


  vector<shared_ptr<Process> > sample_list = {ttv,single_t,diboson,wjets_low_nu, wjets_high_nu,tt1l,tt2l,tchiwh_225_75,tchiwh_700_1};


  PlotOpt log_lumi("txt/plot_styles.txt", "CMSPaper");
  log_lumi.Title(TitleType::preliminary)
    .Bottom(BottomType::ratio)
    .YAxis(YAxisType::log)
    .Stack(StackType::data_norm);
  PlotOpt lin_lumi = log_lumi().YAxis(YAxisType::linear);
  PlotOpt log_shapes = log_lumi().Stack(StackType::shapes)
    .ShowBackgroundError(false);
  PlotOpt lin_shapes = log_shapes().YAxis(YAxisType::linear);
  PlotOpt log_lumi_info = log_lumi().Title(TitleType::info);
  PlotOpt lin_lumi_info = lin_lumi().Title(TitleType::info);
  PlotOpt log_shapes_info = log_shapes().Title(TitleType::info);
  PlotOpt lin_shapes_info = lin_shapes().Title(TitleType::info);
  vector<PlotOpt> all_plot_types = {log_lumi, lin_lumi, log_shapes, lin_shapes,
                                    log_lumi_info, lin_lumi_info, log_shapes_info, lin_shapes_info};
  PlotOpt style2D("txt/plot_styles.txt", "Scatter");
  vector<PlotOpt> bkg_hist = {style2D().Stack(StackType::data_norm).Title(TitleType::preliminary)};
  vector<PlotOpt> bkg_pts = {style2D().Stack(StackType::lumi_shapes).Title(TitleType::info)};
  
  PlotMaker pm;



  Table & cutflow = pm.Push<Table>("cutflow", vector<TableRow>{
      TableRow("2b Region",basic_cuts&&WHLeptons==1&&HasMedMedDeepCSV&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150"),
      TableRow("2c Region",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150"),
      TableRow("2b Region, mbb>200",basic_cuts&&WHLeptons==1&&HasMedMedDeepCSV&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200"),
      TableRow("2c Region, mbb>200",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200")

        }, sample_list,false);
  
  Table & cutflow_CvB = pm.Push<Table>("cutflow_CvB", vector<TableRow>{
      TableRow("2b Region, $<$2 medium b",basic_cuts&&WHLeptons==1&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150"),
      TableRow("2c Region, 2 tight c",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150"),
      TableRow("2c Region, 2 medium c",basic_cuts&&WHLeptons==1&&nDeepMedCTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150"),
      TableRow("2c Region, 2 tight CvB",basic_cuts&&WHLeptons==1&&nDeepTightCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150"),
      TableRow("2c Region, 2 medium CvB",basic_cuts&&WHLeptons==1&&nDeepMedCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150"),
      TableRow("2c Region, 2 tight c, 2 tight CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==2.&&nDeepTightCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150"),
      TableRow("2c Region, 2 tight c, 2 medium CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==2.&&nDeepMedCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150"),
      TableRow("2c Region, 1 tight+1 medium c, 2 tight CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==1.&&nDeepMedCTagged==1.&&nDeepTightCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150"),
      TableRow("2c Region, 1 tight+1 medium c, 2 medium CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==1.&&nDeepMedCTagged==1.&&nDeepMedCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150"),
      TableRow("2c Region, 2 medium c, 2 tight CvB",basic_cuts&&WHLeptons==1&&nDeepMedCTagged==2.&&nDeepTightCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150"),
      TableRow("2c Region, 2 medium c, 2 medium CvB",basic_cuts&&WHLeptons==1&&nDeepMedCTagged==2.&&nDeepMedCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150")

        }, sample_list,false);

  Table & cutflow_CvB_highMbb = pm.Push<Table>("cutflow_CvB_highMbb", vector<TableRow>{
      TableRow("2b Region, $<$2 medium b",basic_cuts&&WHLeptons==1&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200"),
      TableRow("2c Region, 2 tight c",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200"),
      TableRow("2c Region, 2 medium c",basic_cuts&&WHLeptons==1&&nDeepMedCTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200"),
      TableRow("2c Region, 2 tight CvB",basic_cuts&&WHLeptons==1&&nDeepTightCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200"),
      TableRow("2c Region, 2 medium CvB",basic_cuts&&WHLeptons==1&&nDeepMedCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200"),
      TableRow("2c Region, 2 tight c, 2 tight CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==2.&&nDeepTightCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200"),
      TableRow("2c Region, 2 tight c, 2 medium CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==2.&&nDeepMedCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200"),
      TableRow("2c Region, 1 tight+1 medium c, 2 tight CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==1.&&nDeepMedCTagged==1.&&nDeepTightCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200"),
      TableRow("2c Region, 1 tight+1 medium c, 2 medium CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==1.&&nDeepMedCTagged==1.&&nDeepMedCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200"),
      TableRow("2c Region, 2 medium c, 2 tight CvB",basic_cuts&&WHLeptons==1&&nDeepMedCTagged==2.&&nDeepTightCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200"),
      TableRow("2c Region, 2 medium c, 2 medium CvB",basic_cuts&&WHLeptons==1&&nDeepMedCTagged==2.&&nDeepMedCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200")

        }, sample_list,false);

  Table & cutflow_CvB_noMbb = pm.Push<Table>("cutflow_CvB_noMbb", vector<TableRow>{
      TableRow("2b Region, $<$2 medium b",basic_cuts&&WHLeptons==1&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150"),
      TableRow("2c Region, 2 tight c",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150"),
      TableRow("2c Region, 2 medium c",basic_cuts&&WHLeptons==1&&nDeepMedCTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150"),
      TableRow("2c Region, 2 tight CvB",basic_cuts&&WHLeptons==1&&nDeepTightCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150"),
      TableRow("2c Region, 2 medium CvB",basic_cuts&&WHLeptons==1&&nDeepMedCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150"),
      TableRow("2c Region, 2 tight c, 2 tight CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==2.&&nDeepTightCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150"),
      TableRow("2c Region, 2 tight c, 2 medium CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==2.&&nDeepMedCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150"),
      TableRow("2c Region, 1 tight+1 medium c, 2 tight CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==1.&&nDeepMedCTagged==1.&&nDeepTightCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150"),
      TableRow("2c Region, 1 tight+1 medium c, 2 medium CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==1.&&nDeepMedCTagged==1.&&nDeepMedCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150"),
      TableRow("2c Region, 2 medium c, 2 tight CvB",basic_cuts&&WHLeptons==1&&nDeepMedCTagged==2.&&nDeepTightCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150"),
      TableRow("2c Region, 2 medium c, 2 medium CvB",basic_cuts&&WHLeptons==1&&nDeepMedCTagged==2.&&nDeepMedCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150")

        }, sample_list,false);

  Table & cutflow_CvB_v2 = pm.Push<Table>("cutflow_CvB_v2", vector<TableRow>{
      TableRow("2b Region, $<$2 medium b",basic_cuts&&WHLeptons==1&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150"),
      TableRow("2c Region, 2 tight c",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150"),
      TableRow("2c Region, 2 medium c",basic_cuts&&WHLeptons==1&&nDeepMedCTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150"),
      TableRow("2c Region, 2 tight CvB",basic_cuts&&WHLeptons==1&&nDeepTightCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150"),
      TableRow("2c Region, 2 medium CvB",basic_cuts&&WHLeptons==1&&nDeepMedCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150"),
      TableRow("2c Region, 2 loose CvB",basic_cuts&&WHLeptons==1&&nDeepLooseCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150"),
      TableRow("2c Region, 2 tight c, 2 tight CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==2.&&nDeepTightCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150"),
      TableRow("2c Region, 2 tight c, 2 medium CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==2.&&nDeepMedCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150"),
      TableRow("2c Region, 2 tight c, 2 loose CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==2.&&nDeepLooseCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150"),
      TableRow("2c Region, 1 tight+1 medium c, 2 tight CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==1.&&nDeepMedCTagged==2.&&nDeepTightCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150"),
      TableRow("2c Region, 1 tight+1 medium c, 2 medium CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==1.&&nDeepMedCTagged==2.&&nDeepMedCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150"),
      TableRow("2c Region, 1 tight+1 medium c, 2 loose CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==1.&&nDeepMedCTagged==2.&&nDeepLooseCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>90&&mbb<150")

        }, sample_list,false);

  Table & cutflow_CvB_highMbb_v2 = pm.Push<Table>("cutflow_CvB_highMbb_v2", vector<TableRow>{
      TableRow("2b Region, $<$2 medium b",basic_cuts&&WHLeptons==1&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200"),
      TableRow("2c Region, 2 tight c",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200"),
      TableRow("2c Region, 2 medium c",basic_cuts&&WHLeptons==1&&nDeepMedCTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200"),
      TableRow("2c Region, 2 tight CvB",basic_cuts&&WHLeptons==1&&nDeepTightCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200"),
      TableRow("2c Region, 2 medium CvB",basic_cuts&&WHLeptons==1&&nDeepMedCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200"),
      TableRow("2c Region, 2 loose CvB",basic_cuts&&WHLeptons==1&&nDeepLooseCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200"),
      TableRow("2c Region, 2 tight c, 2 tight CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==2.&&nDeepTightCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200"),
      TableRow("2c Region, 2 tight c, 2 medium CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==2.&&nDeepMedCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200"),
      TableRow("2c Region, 2 tight c, 2 loose CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==2.&&nDeepLooseCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200"),
      TableRow("2c Region, 1 tight+1 medium c, 2 tight CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==1.&&nDeepMedCTagged==2.&&nDeepTightCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200"),
      TableRow("2c Region, 1 tight+1 medium c, 2 medium CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==1.&&nDeepMedCTagged==2.&&nDeepMedCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200"),
      TableRow("2c Region, 1 tight+1 medium c, 2 loose CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==1.&&nDeepMedCTagged==2.&&nDeepLooseCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150&&mbb>200")

        }, sample_list,false);

  Table & cutflow_CvB_noMbb_v2 = pm.Push<Table>("cutflow_CvB_noMbb_v2", vector<TableRow>{
      TableRow("2b Region, $<$2 medium b",basic_cuts&&WHLeptons==1&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150"),
      TableRow("2c Region, 2 tight c",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150"),
      TableRow("2c Region, 2 medium c",basic_cuts&&WHLeptons==1&&nDeepMedCTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150"),
      TableRow("2c Region, 2 tight CvB",basic_cuts&&WHLeptons==1&&nDeepTightCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150"),
      TableRow("2c Region, 2 medium CvB",basic_cuts&&WHLeptons==1&&nDeepMedCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150"),
      TableRow("2c Region, 2 loose CvB",basic_cuts&&WHLeptons==1&&nDeepLooseCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150"),
      TableRow("2c Region, 2 tight c, 2 tight CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==2.&&nDeepTightCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150"),
      TableRow("2c Region, 2 tight c, 2 medium CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==2.&&nDeepMedCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150"),
      TableRow("2c Region, 2 tight c, 2 loose CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==2.&&nDeepLooseCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150"),
      TableRow("2c Region, 1 tight+1 medium c, 2 tight CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==1.&&nDeepMedCTagged==2.&&nDeepTightCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150"),
      TableRow("2c Region, 1 tight+1 medium c, 2 medium CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==1.&&nDeepMedCTagged==2.&&nDeepMedCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150"),
      TableRow("2c Region, 1 tight+1 medium c, 2 loose CvB",basic_cuts&&WHLeptons==1&&nDeepTightCTagged==1.&&nDeepMedCTagged==2.&&nDeepLooseCvBTagged==2.&&nDeepMedBTagged<2.&&"pfmet>200&&mct>200&&mt_met_lep>150")

        }, sample_list,false);

  if(single_thread) pm.multithreaded_ = false;
  pm.MakePlots(lumi);

  vector<GammaParams> yields = cutflow.BackgroundYield(lumi);
  for(const auto &yield: yields){
    cout << yield << endl;
  }

  vector<GammaParams> yields_CvB = cutflow_CvB.BackgroundYield(lumi);
  for(const auto &yield: yields_CvB){
    cout << yield << endl;
  }

  vector<GammaParams> yields_CvB_highMbb = cutflow_CvB_highMbb.BackgroundYield(lumi);
  for(const auto &yield: yields_CvB_highMbb){
    cout << yield << endl;
  }
  
  vector<GammaParams> yields_CvB_noMbb = cutflow_CvB_noMbb.BackgroundYield(lumi);
  for(const auto &yield: yields_CvB_noMbb){
    cout << yield << endl;
  }

    vector<GammaParams> yields_CvB_v2 = cutflow_CvB_v2.BackgroundYield(lumi);
  for(const auto &yield: yields_CvB_v2){
    cout << yield << endl;
  }

  vector<GammaParams> yields_CvB_highMbb_v2 = cutflow_CvB_highMbb_v2.BackgroundYield(lumi);
  for(const auto &yield: yields_CvB_highMbb_v2){
    cout << yield << endl;
  }
  
  vector<GammaParams> yields_CvB_noMbb_v2 = cutflow_CvB_noMbb_v2.BackgroundYield(lumi);
  for(const auto &yield: yields_CvB_noMbb_v2){
    cout << yield << endl;
  }

}
