#include <string>
#include <sstream>
#include <utility>
#include <stdexcept>

#include <boost/program_options.hpp>

#include "Pythia8/Pythia.h"

using namespace std;
using namespace Pythia8;
namespace po = boost::program_options;

template<class T>
string opt(const string& str, const T& val) {
  stringstream ss;
  ss << str << " = " << val;
  return ss.str();
}

enum process_t { quark, gluon, higgs };
istream& operator>>(istream& in, process_t& p) {
  string str;
  in >> str;
  if (!str.compare("quark")) p = quark;
  else if (!str.compare("gluon")) p = gluon;
  else if (!str.compare("higgs")) p = higgs;
  else throw runtime_error("Unrecognized process "+str);
  return in;
}
namespace std {
  template<class A, class B>
  istream& operator>>(istream& in, pair<A,B>& p) {
    string str;
    in >> str;
    const size_t sep = str.find(':');
    if (sep!=string::npos) {
      p.first  = atof( str.substr(0,sep).c_str() );
      p.second = atof( str.substr(sep+1).c_str() );
    } else throw runtime_error("Delimeter : expected in "+str);
    return in;
  }
}

int main(int argc, char **argv)
{
  // START OPTIONS **************************************************
  string output;
  double Ecm;
  pair<double,double> pT;
  long num_events;
  int max_errs;
  process_t process;

  try {
    // General Options ------------------------------------
    po::options_description all_opt("Options");
    all_opt.add_options()
      ("help,h", "produce help message")
      ("output,o", po::value<string>(&output)->required(),
       "output LHE file with generated events")
      ("process", po::value<process_t>(&process)->required(),
       "quark, gluon, higgs")
      ("Ecm", po::value<double>(&Ecm)->default_value(14000.),
       "Center of mass energy in GeV")
      ("pt", po::value< pair<double,double> >(&pT)->required(),
       "pT cut in GeV, min:max")
      ("num-events,n", po::value<long>(&num_events)->default_value(1000.),
       "number of events to generate")
      ("max-errs", po::value<int>(&max_errs)->default_value(100),
       "number of aborts before run stops")
    ;

    po::positional_options_description pos_opt;
    pos_opt.add("process",1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv)
              .options(all_opt)
              .positional(pos_opt).run(), vm);
    if (argc == 1 || vm.count("help")) {
      cout << "Usage example: " << argv[0] << " higgs --pt=500:500 -o higgs.lhe" << endl;
      cout << all_opt << endl;
      exit(0);
    }
    po::notify(vm);
  }
  catch(exception& e) {
    cerr << "\033[31mError: " <<  e.what() <<"\033[0m"<< endl;
    exit(1);
  }
  // END OPTIONS ****************************************************

  // Generator
  Pythia pythia(XMLDIR);

  // Settings related to output in init(), next(), and stat()
  pythia.readString("Init:showChangedSettings = on");      // list changed settings
  pythia.readString("Init:showChangedParticleData = off"); // list changed particle data
  pythia.readString("Next:numberCount = 1000");            // print message every n events
  pythia.readString("Next:numberShowInfo = 3");            // print event information n times
  pythia.readString("Next:numberShowProcess = 0");         // print process record n times
  pythia.readString("Next:numberShowEvent = 0");           // print event record n times

  // Settings for the Cuts
  pythia.readString(opt("PhaseSpace:pTHatMin",pT.first));  // Minimum invariant pT
  pythia.readString(opt("PhaseSpace:pTHatMax",pT.second)); // Maximum invariant pT

  // Settings for Showering
  pythia.readString("PartonLevel:MPI = off");              // multiparton interactions
  pythia.readString("PartonLevel:ISR = on");               // initial-state radiation
  pythia.readString("PartonLevel:FSR = on");               // final-state radiation
  pythia.readString("HadronLevel:Hadronize = off");        // hadronization

  switch (process) {
    case quark:
      pythia.readString("HardQCD:gg2qqbar = on");
    break;
    case gluon:
      pythia.readString("HardQCD:gg2gg = on");
    break;
    case higgs:
      pythia.readString("HiggsSM:gg2Hg(l:t) = on");
      pythia.readString("25:m0 = 125.0");
      pythia.readString("25:mWidth = 0.00403");
      pythia.readString("25:onMode = off");
      pythia.readString("25:onIfAny = 5");
    break;
  }

  // Initialize
  pythia.init(2212,2212,14000.); // Initialise pp (PDG 2212) at 14 TeV

  // Create an LHAup object that can access relevant information in pythia.
  LHAupFromPYTHIA8 myLHA(&pythia.process, &pythia.info);

  // Open a file on which LHEF events should be stored, and write header.
  myLHA.openLHEF(output);

  // Store initialization info in the LHAup object.
  myLHA.setInit();

  // Write out this initialization info on the file.
  myLHA.initLHEF();

  Hist pTH("dN/dpTH", 100, 400, 600);

  // Begin event loop.
  for (long event=0; event<num_events; ++event) {
    // Generate events. Quit if too many failures
    if (!pythia.next()) {
        static int num_errs = 0;
        if (++num_errs == max_errs) {
          cerr << "\033[31mAborted after " << num_errs << " errors\033[0m" << endl;
          // exit(1);
        }
    }

    // Store event info in the LHAup object.
    myLHA.setEvent();

    // Write out this event info on the file.
    // With optional argument (verbose =) false the file is smaller.
    myLHA.eventLHEF();

  } // End of event loop

  // pythia.stat();

  // Update the cross section info based on Monte Carlo integration during run.
  myLHA.updateSigma();

  // Write endtag. Overwrite initialization info with new cross sections.
  myLHA.closeLHEF(true);

  return 0;
}
