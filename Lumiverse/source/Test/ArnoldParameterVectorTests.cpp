#include "ArnoldParameterVectorTests.h"

int ArnoldParameterVectorTests::runTests() {
  int numPassed = 0;

  (runTest([=]{ return this->parseTests(); }, "parseTests", 1)) ? numPassed++ : numPassed;
  (runTest([=]{ return this->vectorTests(); }, "vectorTests", 2)) ? numPassed++ : numPassed;

  return numPassed;
}

bool ArnoldParameterVectorTests::runTest(std::function<bool()> t, string testName, int testNum) {
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

bool ArnoldParameterVectorTests::parseTests() {
  ArnoldParameterVector<3, int> vector1;
  if (!parseArnoldParameter<3, int>("1, -2, 3,", vector1)) {
	  cout << "Error handling correct input dimension. Int3." << endl;
	  return false;
  }

  if (vector1[0] != 1 ||
	  vector1[1] != -2 || 
	  vector1[2] != 3) {
	  cout << "Error parsing. Int3." << endl;
	  return false;
  }

  ArnoldParameterVector<4, float> vector2;
  if (!parseArnoldParameter<4, float>("1, 2, 3.02, -23.1", vector2)) {
	  cout << "Error handling correct input dimension. Float4." << endl;
	  return false;
  }

  if (abs(vector2[0] - 1) > 1e-4 ||
	  abs(vector2[1] - 2) > 1e-4 ||
	  abs(vector2[2] - 3.02) > 1e-4 ||
	  abs(vector2[3] + 23.1) > 1e-4) {
	  cout << "Error parsing. Float4." << endl;
	  return false;
  }

  ArnoldParameterVector<1, bool> vector3;
  if (!parseArnoldParameter<1, bool>("1", vector3)) {
	  cout << "Error handling correct input dimension. Bool1." << endl;
	  return false;
  }

  if (!vector3[0]) {
	  cout << "Error parsing. Bool1." << endl;
	  return false;
  }

  ArnoldParameterVector<3, int> vector4;
  if (parseArnoldParameter<3, int>("1, -2", vector4)) {
	  cout << "Error reporting mismatched input dimension. Int3." << endl;
	  return false;
  }

  if (vector4[0] != 1 ||
	  vector4[1] != -2 ||
	  vector4[2] != 0) {
	  cout << "Error parsing. Int3." << endl;
	  return false;
  }

  return true;
}

bool ArnoldParameterVectorTests::vectorTests() {
	int values[] = { 1, 1, 1 };
	ArnoldParameterVector<3, int> vector1(values);

	if (vector1[0] != 1 ||
		vector1[1] != 1 ||
		vector1[2] != 1) {
		cout << "Error initialization with values." << endl;
		return false;
	}

	ArnoldParameterVector<3, int> vector2;

	if (vector2[0] != 0 ||
		vector2[1] != 0 ||
		vector2[2] != 0) {
		cout << "Error initialization without values." << endl;
		return false;
	}

	vector2 = vector1;
	if (vector2[0] != 1 ||
		vector2[1] != 1 ||
		vector2[2] != 1) {
		cout << "Error assignment." << endl;
		return false;
	}

	vector2 *= vector1;
	if (vector2 != vector1) {
		cout << "Error *=." << endl;
		return false;
	}

	vector2 -= vector1;
	if (vector2[0] != 0 ||
		vector2[1] != 0 ||
		vector2[2] != 0) {
		cout << "Error -=." << endl;
		return false;
	}

	vector2 += vector1;
	if (vector2 != vector1) {
		cout << "Error +=." << endl;
		return false;
	}

	vector2 = vector2 * 2;
	if (vector2[0] != 2 ||
		vector2[1] != 2 ||
		vector2[2] != 2) {
		cout << "Error *." << endl;
		return false;
	}

	ArnoldParameterVector<3, int> vector3;
	vector3 = vector2 / vector2;
	if (vector3 != vector1) {
		cout << "Error /." << endl;
		return false;
	}

	vector3 = vector2 + vector1;
	if (vector3 != vector1 * 3) {
		cout << "Error +." << endl;
		return false;
	}

	vector3 = vector2 - vector1;
	if (vector3 != vector1) {
		cout << "Error -." << endl;
		return false;
	}

	vector2 /= (vector1 * 2);
	if (vector2 != vector1) {
		cout << "Error /=." << endl;
		return false;
	}

	if (vector2 != vector1) {
		cout << "Error ==." << endl;
		return false;
	}

	if (vector2 >= vector1 * 2) {
		cout << "Error >=." << endl;
		return false;
	}

	return true;
}
