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
  (runTest([=]{ return this->createTimelines(); }, "createCueList", 3)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->programTimelines(); }, "createCue", 4)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->checkTimeline(); }, "checkCue", 5)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->playCue(); }, "playCue", 6)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->layerToggle(); }, "layerToggle", 7)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->snapshot(); }, "snapshot", 8)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->groups(); }, "groups", 9)) ? numPassed++ : numPassed;

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
  shared_ptr<Layer> newLayer(new Layer(m_testRig, m_pb, "Layer 1", 1));

  if (!m_pb->addLayer(newLayer)) {
    cout << "Failed to add layer to playback\n";
    return false;
  }

  return true;
}

bool PlaybackTests::createTimelines() {
  shared_ptr<Timeline> tl(new Timeline());

  if (!m_pb->addTimeline("Timeline 1", tl)) {
    cout << "Failed to add timeline to playback\n";
    return false;
  }

  if (m_pb->addTimeline("Timeline 1", tl)) {
    cout << "Added a timeline to a Playback but the timeline already existed.\n";
    return false;
  }

  return true;
}

bool PlaybackTests::programTimelines() {
  auto& prog = m_pb->getProgrammer();

  if (m_pb->getTimeline("Timeline 1") == nullptr) {
    cout << "Failed to retrieve Timeline 1\n";
    return false;
  }

  prog->getDevice("s41")->setParam("intensity", 0.0);
  prog->writeToTimeline(m_pb->getTimeline("Timeline 1"), 0);

  prog->getDevice("s41")->setParam("intensity", 1.0);
  prog->writeToTimeline(m_pb->getTimeline("Timeline 1"), 5000);
  
  if (m_pb->getTimeline("Timeline 1")->getKeyframe("s41:intensity", 0).t != 0) {
    cout << "Failed to store keyframe in Timeline 1\n";
    return false;
  }

  // CLEAR THE PROGRAMMER
  prog->clearAndReset();

  return true;
}

bool PlaybackTests::checkTimeline() {
  if (m_pb->getTimeline("Timeline 1") == nullptr) {
    cout << "Failed to retrieve Timeline 1\n";
    return false;
  }

  auto val = m_pb->getTimeline("Timeline 1")->getValueAtTime(m_pb->getRig()->getDevice("s41"), "intensity", 2500, m_pb->getTimelines());

  if (abs(((LumiverseFloat*)val.get())->getVal() - 0.5) > 0.00001) {
    cout << "Timeline value at time error. At time 2500ms, s41 intensity expected: 0.5. Received: " << ((LumiverseFloat*)val.get())->getVal() << "\n";
    return false;
  }

  if (m_pb->getTimeline("Timeline 1")->getValueAtTime(nullptr, "INVALID PARAMETER", 0, m_pb->getTimelines()) != nullptr) {
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
  m_pb->getLayer("Layer 1")->play("Timeline 1");
  
  cout << "Playing back Timeline 1...\n";
  this_thread::sleep_for(chrono::milliseconds(5100));

  if (((LumiverseFloat*)m_testRig->getDevice("s41")->getParam("intensity"))->getVal() != 1.0) {
    cout << "Played back Timeline 1 but didn't end up at right value\n";
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
  m_pb->stop();

  state1.loadSnapshot(m_testRig, m_pb);

  m_pb->start();

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

  if (m_pb->getTimeline("Timeline 1") == nullptr) {
    cout << "Failed to restore Timeline 1.t\n";
    return false;
  }

  return true;
}

bool PlaybackTests::groups() {
  DeviceSet g1 = m_testRig->select("#1-10");

  if (!m_pb->storeGroup("my group", g1)) {
    cout << "Failed to save group in playback\n";
    return false;
  }

  if (!m_pb->getGroup("my group").hasSameDevices(g1)) {
    cout << "Failed to retrive group from playback\n";
    return false;
  }

  DeviceSet g2 = m_testRig->select("#1-5");
  if (m_pb->storeGroup("my group", g2)) {
    cout << "Failed to detect exiting group in playback\n";
    return false;
  }

  if (m_pb->getGroup("DNE").size() != 0) {
    cout << "Group that doesn't exist in playback has non-zero number of devices\n";
    return false;
  }

  if (!m_pb->storeGroup("my group", g2, true)) {
    cout << "Failed to overwrite existing group.";
    return false;
  }

  if (!m_pb->deleteGroup("my group")) {
    cout << "Failed to delete group\n";
    return false;
  }

  DynamicDeviceSet dg1 = DynamicDeviceSet(m_testRig, "#1-5");

  if (!m_pb->storeDynamicGroup("my group", dg1)) {
    cout << "Failed to save dynamic group in playback\n";
    return false;
  }

  if (!m_pb->getDynamicGroup("my group").hasSameDevices(dg1)) {
    cout << "Failed to retrive dynamic group from playback\n";
    return false;
  }

  DynamicDeviceSet dg2 = DynamicDeviceSet(m_testRig, "#1-10");
  if (m_pb->storeDynamicGroup("my group", dg2)) {
    cout << "Failed to detect exiting dynamic group in playback\n";
    return false;
  }

  if (m_pb->getDynamicGroup("DNE").size() != 0) {
    cout << "Dynamic group that doesn't exist in playback has non-zero number of devices\n";
    return false;
  }

  if (!m_pb->storeDynamicGroup("my group", dg2, true)) {
    cout << "Failed to overwrite existing dynamic group.";
    return false;
  }

  if (!m_pb->deleteDynamicGroup("my group")) {
    cout << "Failed to delete dynamic group\n";
    return false;
  }
  return true;
}