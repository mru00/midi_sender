#pragma once

#include <vector>
#include <map>
#include <string>
#include <iostream>


class Signal;
class Controller;

typedef unsigned char uchar_t;
typedef unsigned int uint_t;

class signal_types_t {
public:
  enum  {
	ST_CONTROL_DIGITAL = 1,
	ST_CONTROL_ANALOG,
	ST_STATUS,
	ST_NOT_IMPL,
  };
};


extern int yyerror(char*);
extern int yylineno;

class toString {
public:
  std::string val;
  toString(int x) {
	char buffer[512];
	sprintf(buffer, "%d", x);
	val = buffer;
  }
  operator const char* () {
	return val.c_str();
  }
  const char* str() {
	return val.c_str();
  }
};

class InvalidSignalParameter : public std::exception {

  std::string error;
public:
  InvalidSignalParameter(std::string par, std::string name)  throw () {
	error = "Invalid parameter: '" + par + "' for signal " + name  + " ("+toString(yylineno).str()+")";
  }

  virtual ~InvalidSignalParameter() throw () {};
  virtual const char* what() const throw () { return error.c_str(); };
};


class DuplicatedToken : public std::exception {

  std::string error;
public:
  DuplicatedToken()  throw () {
	error = error + "Type was already used ("+toString(yylineno).str()+")";
  }

  virtual ~DuplicatedToken() throw () {};
  virtual const char* what() const throw () { return error.c_str(); };
};


class Signal {
protected:
  std::map<std::string, int> values;
  std::string signal_name;
public:

  Signal(std::string name) : signal_name(name) {};

  void addParam(std::string name, int value) {
	if ( values.find(name) == values.end() ) {
	  throw InvalidSignalParameter(name, signal_name);
	}
	values[name] = value;
  }
  virtual int fill_buffer(uchar_t* buffer, int of) = 0;
};



class Controller {
public:

  typedef std::map<int, Signal*> SignalMap;
  SignalMap signals;

  Signal* addSignal(int num, Signal* prot) {
	if ( signals.find(num) != signals.end() )
	  throw DuplicatedToken();
	signals[num] = prot;
	return prot;
  }

  int fill_buffer(uchar_t* buffer, int of) {

	int len = of+1;

	for ( SignalMap::iterator it = signals.begin();
		  it != signals.end();
		  it ++ ) {
	  
	  buffer[len++] = it->first;
	  len = it->second->fill_buffer(buffer, len);

	}
	buffer[of] = len - of -1;

	return len;

  }

  void clear() {
	signals.clear();
  }
};


class File {

public:
  typedef std::map<int,Controller> ControllerMap;
  ControllerMap controllers;

  void addController(int num, const Controller& contr) {
	if ( controllers.find(num) != controllers.end() )
	  throw DuplicatedToken();
	controllers[num] = contr;
  }

  int fill_buffer(uchar_t* buffer) {
	
	int len = 1;

	for ( ControllerMap::iterator it = controllers.begin();
		  it != controllers.end();
		  it ++ ) {

	  buffer[len++] = it->first;
	  len = it->second.fill_buffer(buffer, len);
	}
	
	buffer[0] = len;
	return len;
  }
};

class SignalStatus : public Signal{
public:
  SignalStatus() : Signal("status") {
  }

  int fill_buffer(uchar_t* buffer, int of) {
	buffer[of++] = signal_types_t::ST_STATUS;
	return of;
  }
};



class SignalAnalogControl : public Signal{
public:
  SignalAnalogControl() : Signal("analog control") {
	values["id"] = 0;
	values["ch"] = 0;
  }

  int fill_buffer(uchar_t* buffer, int of) {

	buffer[of++] = signal_types_t::ST_CONTROL_ANALOG;
	buffer[of++] = values["ch"];
	buffer[of++] = values["id"];

	return of;
  }
};


class SignalDigitalControl : public Signal{
public:
  SignalDigitalControl() : Signal("digital control") {
	values["id"] = 0;
	values["ch"] = 0;
  }

  int fill_buffer(uchar_t* buffer, int of) {
	buffer[of++] = signal_types_t::ST_CONTROL_DIGITAL;
	buffer[of++] = values["ch"];
	buffer[of++] = values["id"];
	return of;
  }
};


class SignalNoteOn : public Signal{
public:
  SignalNoteOn()  : Signal("note on") {
	values["ch"] = 0;
	values["note"] = 0;
	values["velocity"] = 0;
  }

  int fill_buffer(uchar_t* buffer, int of) {

	buffer[of++] = signal_types_t::ST_NOT_IMPL;
	buffer[of++] = values["ch"];
	buffer[of++] = values["note"];
	buffer[of++] = values["velocity"];

	return of;
  }

};

class SignalNoteOff : public Signal{
public:
  SignalNoteOff() : Signal("note off")  {
	values["ch"] = 0;
	values["note"] = 0;
	values["velocity"] = 0;
  }


  int fill_buffer(uchar_t* buffer, int of) {

	buffer[of++] = signal_types_t::ST_NOT_IMPL;
	buffer[of++] = values["ch"];
	buffer[of++] = values["note"];
	buffer[of++] = values["velocity"];

	return of;
  }
};

class SignalAfterTouch : public Signal{
public:
  SignalAfterTouch() : Signal("after touch")  {
	values["ch"] = 0;
	values["note"] = 0;
	values["pressure"] = 0;
  }

  int fill_buffer(uchar_t* buffer, int of) {

	buffer[of++] = signal_types_t::ST_NOT_IMPL;
	buffer[of++] = values["ch"];
	buffer[of++] = values["note"];
	buffer[of++] = values["pressure"];

	return of;
  }
};

class SignalProgramChange : public Signal{
public:
  SignalProgramChange() : Signal("program change")  {
	values["ch"] = 0;
	values["program"] = 0;
  }

  int fill_buffer(uchar_t* buffer, int of) {

	buffer[of++] = signal_types_t::ST_NOT_IMPL;
	buffer[of++] = values["ch"];
	buffer[of++] = values["program"];

	return of;
  }
};

class SignalChannelPressure : public Signal{
public:
  SignalChannelPressure() : Signal("channel pressure")  {
	values["id"] = 0;
	values["ch"] = 0;
  }

  int fill_buffer(uchar_t* buffer, int of) {

	buffer[of++] = signal_types_t::ST_NOT_IMPL;
	buffer[of++] = values["id"];
	buffer[of++] = values["ch"];

	return of;
  }
};

class SignalPitchWheel : public Signal{
public:
  SignalPitchWheel() : Signal("pitch wheel")  {
	values["id"] = 0;
	values["ch"] = 0;
  }

  int fill_buffer(uchar_t* buffer, int of) {

	buffer[of++] = signal_types_t::ST_NOT_IMPL;
	buffer[of++] = values["id"];
	buffer[of++] = values["ch"];

	return of;
  }
};
