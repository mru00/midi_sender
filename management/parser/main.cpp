#include <iostream>
#include <fstream>
#include <map>
#include <vector>


using namespace std;

#include "usb.hpp"
#include "common.h"
#include "midi_events.hpp"

extern File this_file;


const char* yyin_filename;


void usage(char* filename) {
  // strip leading path
  char* pstr = filename;
  while ( *pstr != '\0' ) {
	if ( *pstr == '/' || *pstr == '\\' ) filename = pstr + 1;
	pstr++;
  }
  cout << filename << " [-o outfile] [-d] infile" << endl
	   << "-d .. debug" << endl
	   << "-o .. outfile (defaults to a.out)"<< endl
	   << "-s .. send file with usb"<< endl;
  
}

int main(int argc, char** argv) {

  string infile;
  string outfile = "a.out";
  opterr = 0;
  int c;
  int send = 0;
  int parseresult;
  ofstream os;
  uchar_t buffer[512];
  uint_t buffer_len = 0;

  yydebug = 0;

  while ( (c=getopt(argc, argv, "dso:")) != -1 ) {

	switch (c) {

	case 'o':
	  outfile = optarg;
	  break;

	case 'd':
	  yydebug = 1;
	  break;

	case 's':
	  send = 1;
	  break;

	case '?':
	  cerr << "unknown option" << endl;
	  usage(argv[0]);
	default:
	  abort();
	}
  }

  if (optind != argc-1) {
	cerr << "no file given!" << endl;
	usage(argv[0]);
	abort();
  }
  else {
	infile = argv[optind];
  }

  os.open (outfile.c_str(), ios::out);

  if ( !os.good() ) {
	fclose(yyin);
	cerr << "could not open file '" << outfile << "' for writing" << endl;
	abort();
  }
  
  parseresult = parse(infile.c_str());

  if ( parseresult == 0 ) {

	int len = this_file.fill_buffer(buffer);
	os.write((const char*)buffer, len);


	if ( send ) {

	  if ( send_data(buffer, len) == 0 ) 
		cout << "successfully configured device"<<endl;
	  else
		cerr << "error configuring device"<<endl;
	}
  }

  os.close();

  return parseresult;
};


int parse(const char* filename) {

  FILE* oldfile = yyin;
  int parseresult;

  // open new file
  yyin = fopen(filename, "r");
  yyin_filename = filename;

  if ( yyin == NULL ) {
	cerr << "could not open file '" << filename << "' for reading" << endl;
	abort();
  }
  
  // parse
  parseresult = yyparse();

  // restore old file
  if ( yyin != stdin )
	fclose(yyin);

  yyin = oldfile;

  return parseresult;
}





