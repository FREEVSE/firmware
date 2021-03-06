/*
  Automaton.h - Reactive State Machine Framework for Arduino.
  Published under the MIT License (MIT), Copyright (c) 2015-2016, J.P. van der Landen
*/

#pragma once

#include "Arduino.h"

typedef int8_t state_t;

const uint8_t ATM_SLEEP_FLAG = B00000001;
const uint8_t ATM_CYCLE_FLAG = B00000010;
const uint8_t ATM_SLINK_FLAG = B00000100;
const uint8_t ATM_USR1_FLAG = B00010000;
const uint8_t ATM_USR2_FLAG = B00100000;
const uint8_t ATM_USR3_FLAG = B01000000;
const uint8_t ATM_USR4_FLAG = B10000000;
const uint8_t ATM_USR_FLAGS = B11110000;
const uint8_t ATM_BROADCAST = B10000000;

#define read_state( addr ) ( state_t ) pgm_read_byte_near( addr )

#define B2INT( hibyte, lobyte ) ((int)( hibyte << 8 | lobyte ))
#define B4INT( byte4, byte3, byte2, byte1) ((uint32_t)( (uint32_t)byte4 << 24 | (uint32_t)byte3 << 16 | (uint32_t)byte2 << 8 | (uint32_t)byte1 ))

class Machine;
class Automaton;

extern Automaton automaton;

typedef void ( *swcb_sym_t )( Stream* stream, Machine& machine, const char label[], const char current[], const char next[], const char trigger[],
                              uint32_t runtime, uint32_t cycles );

const uint8_t ATM_UP = 1;
const uint8_t ATM_DOWN = 0;

const state_t ATM_NOP = -1;
const state_t ATM_SLEEP = -2;
const state_t ATM_ON_SWITCH = -3;
const state_t ATM_ON_ENTER = 0;
const state_t ATM_ON_LOOP = 1;
const state_t ATM_ON_EXIT = 2;

const uint32_t ATM_TIMER_OFF = 0xffffffff;  // This timer value never expires
const uint16_t ATM_COUNTER_OFF = 0xffff;    // This counter value never expires

class Automaton {
 public:
  Automaton& add( Machine& machine, bool force = true );
  Automaton& delay( uint32_t time );
  Automaton& run( void );

 private:
  Machine* inventory_root;
  void runTiny( void );
};

// For backward compatibility with 'the old way': Appliance app/app.component()/app.run()

class Appliance : Automaton {
 public:
  Appliance& component( Machine& machine );
  Appliance& run( uint32_t time = 0 );
};

class Factory : Automaton {
 public:
  Factory& add( Machine& machine );
  Factory& cycle( uint32_t time = 0 );
};

// Support classes

#include <atm_connector.hpp>
#include <atm_timer_millis.hpp>
#include <atm_counter.hpp>
#include <atm_serial_debug.hpp>

// Main framework classes

#include <Machine.hpp>
