#include <iostream>
#include <string>

#include "jep/reader.h"

using namespace std;

#define test(var) \
  cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << endl;

void usage(const char* prog) {
  cout << "Usage:" << endl;
  cout << "(1) " << prog << " input_file output_file" << endl;
  cout << "(2) " << prog << " input_file" << endl;
}

int main(int argc, char** argv)
{
  string fin, fout;

  if (argc==1) {
    usage(argv[0]);
    return 0;
  } else if (argc==2) {
    fin = argv[1];
    if (fin.size()<5 || fin.substr(fin.size()-4,4).compare(".jep")) {
      fout = fin+".txt";
    } else {
      fout = fin.substr(0,fin.size()-4)+".txt";
    }
  } else if (argc==3) {
    fin  = argv[1];
    fout = argv[2];
  } else {
    usage(argv[0]);
    return 1;
  }

  jep::reader* r = new jep::reader(fin.c_str());
  r->to_ascii(fout.c_str());
  delete r;

  return 0;
}
