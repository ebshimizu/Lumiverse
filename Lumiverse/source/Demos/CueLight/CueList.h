#ifndef _CUELIST_H_
#define _CUELIST_H_

#pragma once

#include <LumiverseCore.h>
#include "Cue.h"

// A cue list is a list of cues. This class maintains the relationships
// between cues and performs update operations.
// This system will optionally track through changes to the cue, meaning
// that if you change a parameter value in a cue, it'll look forward
// through the cue list and update the values of that parameter if
// the parameter was the same in the previous cue.
class CueList
{
public:
  // Make a new empty cue list
  CueList();

  // Delete the cue list
  ~CueList();

  // Stores a cue in the list.
  // Returns false if there's already a cue with the given number in the map.
  // Set overwrite to true to force.
  bool storeCue(float num, Cue cue, bool overwrite = false);

  // Delets a cue.
  void deleteCue(float num);

  // Updates a cue and tracks changes.
  // Set track to false if no tracking desired.
  void update(float num, Rig* rig, bool track = true);

  // Gets the list of cues
  const map<float, Cue>* getCueList() { return &m_cues; }

  // Gets the number of the first cue.
  float getFirstCueNum() { return m_cues.begin()->first; }

  // Gets the current cue in this particular cue list
  float getCurrentCue() { return m_currentCue; }

  // Sets the current cue in the cue list
  void setCurrentCue(float val) { m_currentCue = val; }

  // Gets a cue and allows user to modify it.
  Cue* getCue(float num) {
    if (m_cues.count(num) > 0) {
      return &(m_cues[num]);
    }
    else {
      return nullptr;
    }
  }

  // Gets the next cue in the list
  // If the given cue number isn't in the list, returns nullptr
  Cue* getNextCue(float num);

  // Same as getNextCue but returns the cue number.
  // If the given cue number isn't in the list, returns -1
  float getNextCueNum(float num);
private:
  // List of cues. Cue numbers can be floats.
  map<float, Cue> m_cues;

  // The last cue executed in the cue list.
  float m_currentCue;
};

#endif