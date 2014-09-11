#include "ArnoldFrameManagerTests.h"

int ArnoldFrameManagerTests::runTests() {
  int numPassed = 0;

  (runTest([=]{ return this->memDumpAndPlayTests(); }, "memDumpAndPlayTests", 1)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->memClearTests(); }, "memClearTests", 2)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->fileDumpAndPlayTests(); }, "fileDumpAndPlayTests", 3)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->fileClearTests(); }, "fileClearTests", 4)) ? numPassed++ : numPassed;

  return numPassed;
}

bool ArnoldFrameManagerTests::runTest(std::function<bool()> t, string testName, int testNum) {
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

bool ArnoldFrameManagerTests::dumpAndPlayTests(ArnoldFrameManager *fm) {
  bool ret = true;

  size_t width = 1; size_t height = 1;
  for (int i = 0; i < 10; i++) {
	  float num = i;
	  m_mem_frame_manager.dump(i, &num, width, height);

	  if (m_mem_frame_manager.getFrameNum() != i + 1) {
		  cout << "[ERROR] dump: Cannot save frame. Type: " << fm->getType() << endl;
		  return false;
	  }
  }

  m_mem_frame_manager.reset();
  for (int j = 0; j < 10; j++) {
	  if (m_mem_frame_manager.getCurrentTime() != j) {
		  cout << "[ERROR] dump: Disordered frame.Type: " << fm->getType() << endl;
		  return false;
	  }
	  if (abs(m_mem_frame_manager.getCurrentFrameBuffer()[0] - j) > 1e-4) {
		  cout << "[ERROR] dump: Corrupted frame content. Type: " << fm->getType() << endl;
		  return false;
	  }
		 
	  if (j < 9 && m_mem_frame_manager.hasNext())
		  m_mem_frame_manager.next();
	  else if (j == 9 && m_mem_frame_manager.hasNext()) {
		  cout << "[ERROR] dump: Wrong end. Type: " << fm->getType() << endl;
		  return false;
	  }
	  else if (j < 9) {
		  cout << "[ERROR] dump: Missing frame. Type: " << fm->getType() << endl;
		  return false;
	  }
  }

  return ret;
}

bool ArnoldFrameManagerTests::clearTests(ArnoldFrameManager *fm) {
  bool ret = true;

  size_t width = 1; size_t height = 1;
  for (int i = 0; i < 10; i++) {
	  float num = i;
	  m_mem_frame_manager.dump(i, &num, width, height);
  }

  if (m_mem_frame_manager.getFrameNum() != 10) {
	  cout << "[ERROR] dump: Wrong frame number. Type: " << fm->getType() << endl;
	  return false;
  }

  m_mem_frame_manager.clear();
  if (m_mem_frame_manager.getFrameNum() != 0) {
	  cout << "[ERROR] dump: Error clearing. Type: " << fm->getType() << endl;
	  return false;
  }

  if (!m_mem_frame_manager.isEmpty()) {
	  cout << "[ERROR] dump: Error isEmpty. Type: " << fm->getType() << endl;
	  return false;
  }

  return ret;
}

bool ArnoldFrameManagerTests::memDumpAndPlayTests() {
	return dumpAndPlayTests(&m_mem_frame_manager);
}

bool ArnoldFrameManagerTests::memClearTests() {
	return clearTests(&m_mem_frame_manager);
}

bool ArnoldFrameManagerTests::fileDumpAndPlayTests() {
	return dumpAndPlayTests(&m_file_frame_manager);
}

bool ArnoldFrameManagerTests::fileClearTests() {
	return clearTests(&m_file_frame_manager);
}