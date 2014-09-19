#include "PlaybackTests.h"

int PlaybackTests::runTests() {
  int numPassed = 0;

  if (runTest([=]{ return this->playbackSetup(); }, "playbackSetup", 1)) {
    numPassed++;
  }
  else {
    return 0;
  }
  (runTest([=]{ return this->createLayer(); }, "createLayer", 2)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->createCueList(); }, "createCueList", 3)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->createCue(); }, "createCue", 4)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->checkCue(); }, "checkCue", 5)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->playCue(); }, "playCue", 6)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->layerToggle(); }, "layerToggle", 7)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->snapshot(); }, "snapshot", 8)) ? numPassed++ : numPassed;

  return numPassed;
}

bool PlaybackTests::runTest(std::function<bool()> t, string testName, int testNum) {
  bool pass;

  if (pass = t()) {
    cout << "[ OK ]";
  }
  else {
    cout << "[FAIL]";
  }
  cout << " (" << testNum << "/" << m_numTests << ") " << testName << "\n";
  return pass;
}

bool PlaybackTests::playbackSetup() {
  m_testRig = new Rig("../../source/Test/testRig.json");

  if (m_testRig->getNumDevices() != 50) {
    cout << "Failed to load rig file\n";
    return false;
  }
  m_testRig->init();
  m_testRig->run();
  m_pb = new Playback(m_testRig);
  if (!m_pb->attachToRig()) {
    cout << "Failed to attach playback to Rig\n";
    return false;
  }
  m_pb->start();

  return true;
}

bool PlaybackTests::createLayer() {
  shared_ptr<Layer> newLayer(new Layer(m_testRig, "Layer 1", 1));

  if (!m_pb->addLayer(newLayer)) {
    cout << "Failed to add layer to playback\n";
    return false;
  }

  return true;
}

bool PlaybackTests::createCueList() {
  shared_ptr<CueList> newCueList(new CueList("List 1"));

  if (!m_pb->addCueList(newCueList)) {
    cout << "Failed to add cue list to playback\n";
    return false;
  }

  if (!m_pb->addCueListToLayer("List 1", "Layer 1")) {
    cout << "Failed to add cue list to Layer 1\n";
    return false;
  }

  return true;
}

bool PlaybackTests::createCue() {
  auto& prog = m_pb->getProgrammer();

  if (m_pb->getCueList("List 1") == nullptr) {
    cout << "Failed to retrieve cue list List 1\n";
    return false;
  }

  m_pb->getCueList("List 1")->storeCue(1, prog->getCue());

  if (m_pb->getCueList("List 1")->getCue(1) == nullptr) {
    cout << "Failed to store cue 1 in List 1\n";
    return false;
  }

  prog->getDevice("s41")->setParam("intensity", 1.0);
  
  m_pb->getCueList("List 1")->storeCue(2, prog->getCue());

  if (m_pb->getCueList("List 1")->getCue(2) == nullptr) {
    cout << "Failed to store cue 2 in List 1\n";
    return false;
  }

  // CLEAR THE PROGRAMMER
  prog->clearAndReset();

  return true;
}

bool PlaybackTests::checkCue() {
  if (m_pb->getCueList("List 1") == nullptr) {
    cout << "Failed to retrieve cue list List 1\n";
    return false;
  }

  auto c = m_pb->getCueList("List 1")->getCue(2);

  auto val = c->getValueAtTime(m_pb->getCueList("List 1")->getCue(1), "s41", "intensity", 1.5);

  if (abs(((LumiverseFloat*)val.get())->getVal() - 0.5) > 0.00001) {
    cout << "Cue transition error. At time 1.5, s41 intensity expected: 0.5. Received: " << ((LumiverseFloat*)val.get())->getVal() << "\n";
    return false;
  }

  if (c->getValueAtCueTime(nullptr, "s41", "INVALID PARAMETER", 0) != nullptr) {
    cout << "Cue access failure. Able to get data for device with invalid parameter\n";
    return false;
  }

  return true;
}

bool PlaybackTests::playCue() {
  if (m_pb->getLayer("Layer 1") == nullptr) {
    cout << "failed to retrieve Layer 1\n";
    return false;
  }

  m_pb->getLayer("Layer 1")->activate();
  m_pb->getLayer("Layer 1")->goToCue(2);
  
  cout << "Going to Cue 2...\n";
  this_thread::sleep_for(chrono::milliseconds(3100));

  if (((LumiverseFloat*)m_testRig->getDevice("s41")->getParam("intensity"))->getVal() != 1.0) {
    cout << "Rig played back cue 2 but didn't end up at right value\n";
    return false;
  }

  return true;
}

bool PlaybackTests::layerToggle() {
  if (m_pb->getLayer("Layer 1") == nullptr) {
    cout << "failed to retrieve Layer 1\n";
    return false;
  }

  m_pb->getLayer("Layer 1")->deactivate();

  this_thread::sleep_for(chrono::milliseconds(100));

  if (((LumiverseFloat*)m_testRig->getDevice("s41")->getParam("intensity"))->getVal() != 0) {
    cout << "Layer deactivation failed.\n";
    return false;
  }

  return true;
}

bool PlaybackTests::snapshot() {
  m_pb->getProgrammer()->setParam("s41", "intensity", 1.0f);

  Snapshot state1(m_testRig, m_pb);

  m_pb->getProgrammer()->clearAndReset();

  state1.loadSnapshot(m_testRig, m_pb);

  this_thread::sleep_for(chrono::milliseconds(40));
  float val;
  m_testRig->getDevice("s41")->getParam("intensity", val);

  if (!m_pb->getProgrammer()->isCaptured("s41") || val != 1.0f) {
    cout << "Failed to restore programmer state\n";
    return false;
  }

  m_pb->deleteLayer("Layer 1");
  if (m_pb->getLayer("Layer 1") != nullptr) {
    cout << "Failed to delete Layer 1\n";
    return false;
  }

  state1.loadSnapshot(m_testRig, m_pb);

  if (m_pb->getLayer("Layer 1") == nullptr) {
    cout << "Failed to restore Layer 1\n";
    return false;
  }

  if (m_pb->getLayer("Layer 1")->getCueList() == nullptr) {
    cout << "Failed to restore Layer 1's cue list\n";
    return false;
  }

  return true;
}