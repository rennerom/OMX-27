#define NUM_STEPS 16

// the MIDI channel number to send messages
int midiChannel = 1;

int ticks = 0;            // A tick of the clock
bool clockSource = 0;     // Internal clock (0), external clock (1)
bool playing = 0;         // Are we playing?
bool paused = 0;          // Are we paused?
bool stopped = 1;         // Are we stopped? (Must init to 1)
byte songPosition = 0;    // A place to store the current MIDI song position
int playingPattern = 0;  // The currently playing pattern, 0-7
bool seqResetFlag = 1;

word stepCV;
int seq_velocity = 100;
int seq_acc_velocity = 127;

int seqPos[8] = {0, 0, 0, 0, 0, 0, 0, 0};				// What position in the sequence are we in?
bool patternMute[8] = {false, false, false, false, false, false, false, false};     
int patternLength[8] = {16, 16, 16, 16, 16, 16, 16, 16};
int patternStart[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int pattLen[8] = {patternLength[0],patternLength[1],patternLength[2],patternLength[3],patternLength[4],patternLength[5],patternLength[6],patternLength[7]};

int patternDefaultNoteMap[8] = {36, 38, 37, 39, 42, 46, 49, 51}; // default to GM Drum Map for now


// Determine how to play a step
// -1: restart
// 0: mute
// 1: play
// 2: accent

int stepPlay[8][16] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

// default to GM Drum Map for now
int stepNoteP[8][16][8] = { 		// {notenum,vel,len,p1,p2,p3,p4,p5}
	{ {36, 100, 1, -1, -1, -1, -1, -1}, {36, 100, 1, -1, -1, -1, -1, -1},{36, 100, 1, -1, -1, -1, -1, -1},{36, 100, 1, -1, -1, -1, -1, -1},{36, 100, 1, -1, -1, -1, -1, -1},{36, 100, 1, -1, -1, -1, -1, -1},{36, 100, 1, -1, -1, -1, -1, -1},{36, 100, 1, -1, -1, -1, -1, -1},{36, 100, 1, -1, -1, -1, -1, -1},{36, 100, 1, -1, -1, -1, -1, -1},{36, 100, 1, -1, -1, -1, -1, -1},{36, 100, 1, -1, -1, -1, -1, -1},{36, 100, 1, -1, -1, -1, -1, -1},{36, 100, 1, -1, -1, -1, -1, -1},{36, 100, 1, -1, -1, -1, -1, -1},{36, 100, 1, -1, -1, -1, -1, -1} },
	{ {38, 100, 1, -1, -1, -1, -1, -1}, {38, 100, 1, -1, -1, -1, -1, -1},{38, 100, 1, -1, -1, -1, -1, -1},{38, 100, 1, -1, -1, -1, -1, -1},{38, 100, 1, -1, -1, -1, -1, -1},{38, 100, 1, -1, -1, -1, -1, -1},{38, 100, 1, -1, -1, -1, -1, -1},{38, 100, 1, -1, -1, -1, -1, -1},{38, 100, 1, -1, -1, -1, -1, -1},{38, 100, 1, -1, -1, -1, -1, -1},{38, 100, 1, -1, -1, -1, -1, -1},{38, 100, 1, -1, -1, -1, -1, -1},{38, 100, 1, -1, -1, -1, -1, -1},{38, 100, 1, -1, -1, -1, -1, -1},{38, 100, 1, -1, -1, -1, -1, -1},{38, 100, 1, -1, -1, -1, -1, -1} },
	{ {37, 100, 1, -1, -1, -1, -1, -1}, {37, 100, 1, -1, -1, -1, -1, -1},{37, 100, 1, -1, -1, -1, -1, -1},{37, 100, 1, -1, -1, -1, -1, -1},{37, 100, 1, -1, -1, -1, -1, -1},{37, 100, 1, -1, -1, -1, -1, -1},{37, 100, 1, -1, -1, -1, -1, -1},{37, 100, 1, -1, -1, -1, -1, -1},{37, 100, 1, -1, -1, -1, -1, -1},{37, 100, 1, -1, -1, -1, -1, -1},{37, 100, 1, -1, -1, -1, -1, -1},{37, 100, 1, -1, -1, -1, -1, -1},{37, 100, 1, -1, -1, -1, -1, -1},{37, 100, 1, -1, -1, -1, -1, -1},{37, 100, 1, -1, -1, -1, -1, -1},{37, 100, 1, -1, -1, -1, -1, -1} },
	{ {39, 100, 1, -1, -1, -1, -1, -1}, {39, 100, 1, -1, -1, -1, -1, -1},{39, 100, 1, -1, -1, -1, -1, -1},{39, 100, 1, -1, -1, -1, -1, -1},{39, 100, 1, -1, -1, -1, -1, -1},{39, 100, 1, -1, -1, -1, -1, -1},{39, 100, 1, -1, -1, -1, -1, -1},{39, 100, 1, -1, -1, -1, -1, -1},{39, 100, 1, -1, -1, -1, -1, -1},{39, 100, 1, -1, -1, -1, -1, -1},{39, 100, 1, -1, -1, -1, -1, -1},{39, 100, 1, -1, -1, -1, -1, -1},{39, 100, 1, -1, -1, -1, -1, -1},{39, 100, 1, -1, -1, -1, -1, -1},{39, 100, 1, -1, -1, -1, -1, -1},{39, 100, 1, -1, -1, -1, -1, -1} },
	{ {42, 100, 1, -1, -1, -1, -1, -1}, {42, 100, 1, -1, -1, -1, -1, -1},{42, 100, 1, -1, -1, -1, -1, -1},{42, 100, 1, -1, -1, -1, -1, -1},{42, 100, 1, -1, -1, -1, -1, -1},{42, 100, 1, -1, -1, -1, -1, -1},{42, 100, 1, -1, -1, -1, -1, -1},{42, 100, 1, -1, -1, -1, -1, -1},{42, 100, 1, -1, -1, -1, -1, -1},{42, 100, 1, -1, -1, -1, -1, -1},{42, 100, 1, -1, -1, -1, -1, -1},{42, 100, 1, -1, -1, -1, -1, -1},{42, 100, 1, -1, -1, -1, -1, -1},{42, 100, 1, -1, -1, -1, -1, -1},{42, 100, 1, -1, -1, -1, -1, -1},{42, 100, 1, -1, -1, -1, -1, -1} },
	{ {46, 100, 1, -1, -1, -1, -1, -1}, {46, 100, 1, -1, -1, -1, -1, -1},{46, 100, 1, -1, -1, -1, -1, -1},{46, 100, 1, -1, -1, -1, -1, -1},{46, 100, 1, -1, -1, -1, -1, -1},{46, 100, 1, -1, -1, -1, -1, -1},{46, 100, 1, -1, -1, -1, -1, -1},{46, 100, 1, -1, -1, -1, -1, -1},{46, 100, 1, -1, -1, -1, -1, -1},{46, 100, 1, -1, -1, -1, -1, -1},{46, 100, 1, -1, -1, -1, -1, -1},{46, 100, 1, -1, -1, -1, -1, -1},{46, 100, 1, -1, -1, -1, -1, -1},{46, 100, 1, -1, -1, -1, -1, -1},{46, 100, 1, -1, -1, -1, -1, -1},{46, 100, 1, -1, -1, -1, -1, -1} },
	{ {49, 100, 1, -1, -1, -1, -1, -1}, {49, 100, 1, -1, -1, -1, -1, -1},{49, 100, 1, -1, -1, -1, -1, -1},{49, 100, 1, -1, -1, -1, -1, -1},{49, 100, 1, -1, -1, -1, -1, -1},{49, 100, 1, -1, -1, -1, -1, -1},{49, 100, 1, -1, -1, -1, -1, -1},{49, 100, 1, -1, -1, -1, -1, -1},{49, 100, 1, -1, -1, -1, -1, -1},{49, 100, 1, -1, -1, -1, -1, -1},{49, 100, 1, -1, -1, -1, -1, -1},{49, 100, 1, -1, -1, -1, -1, -1},{49, 100, 1, -1, -1, -1, -1, -1},{49, 100, 1, -1, -1, -1, -1, -1},{49, 100, 1, -1, -1, -1, -1, -1},{49, 100, 1, -1, -1, -1, -1, -1} },
	{ {51, 100, 1, -1, -1, -1, -1, -1}, {51, 100, 1, -1, -1, -1, -1, -1},{51, 100, 1, -1, -1, -1, -1, -1},{51, 100, 1, -1, -1, -1, -1, -1},{51, 100, 1, -1, -1, -1, -1, -1},{51, 100, 1, -1, -1, -1, -1, -1},{51, 100, 1, -1, -1, -1, -1, -1},{51, 100, 1, -1, -1, -1, -1, -1},{51, 100, 1, -1, -1, -1, -1, -1},{51, 100, 1, -1, -1, -1, -1, -1},{51, 100, 1, -1, -1, -1, -1, -1},{51, 100, 1, -1, -1, -1, -1, -1},{51, 100, 1, -1, -1, -1, -1, -1},{51, 100, 1, -1, -1, -1, -1, -1},{51, 100, 1, -1, -1, -1, -1, -1},{51, 100, 1, -1, -1, -1, -1, -1} }
};

int lastNote[8][16] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};
