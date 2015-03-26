/*
  Test program for Lumiverse. Run when changing parts of the core library
  to make sure you didn't break thigs, or just run to prove to yourself that
  this thing works.

  The various categories of tests are broken out into classes for easy editing.
  See the individual files for details.
*/

#include <LumiverseCore.h>
#include <thread>
#include <algorithm>
#include <vector>

#include "Cue.h"
#include "CueList.h"
#include "Layer.h"
#include "Playback.h"

#include "RegressiveTest.h"

void RegressiveTest::runTest() {
// TODO: Re-write this test using the updated Timeline framework.


//	if (!m_testRig) {
//		m_testRig = new Rig("../../data/Jules/jules.rig.json");
//		m_testRig->init();
//	}
//
//	initCues();
//
//	m_playback->start();
//	m_testRig->run();
//
//	m_playback->getProgrammer()->clearAndReset();
//
//	m_playback->save("../../data/Jules/jules_photo.pb.json");
//
//#ifdef USE_ARNOLD
//	ArnoldAnimationPatch *aap = (ArnoldAnimationPatch*)m_testRig->getSimulationPatch("ArnoldAnimationPatch");
//	// TODO: add flag to check if interface is open
//	aap->reset();
//	aap->setRenderSamples(3);
//	aap->startRecording();
//	aap->rerender();
//#endif
//
//	cout << "Start show\n";
//	Layer *layer = m_playback->getLayer("layer1").get();
//	// One by one in given order
//	// Question: how can I get number of cues in a cuelist/layer?
//	int num_cues = layer->getCueList()->getCueList().size();
//	for (int i = 0; i < num_cues; i++) {
//		layer->go();
//		this_thread::sleep_for(chrono::milliseconds(500));
//	}
//
//	// Random
//	std::vector<int> cue_ind;
//
//	// Randomly shuffle cues with one light on
//	for (int i = 1; i < num_cues - 1; ++i) 
//		cue_ind.push_back(i);
//	std::random_shuffle(cue_ind.begin(), cue_ind.end());
//
//	for (int cue_num : cue_ind) {
//		layer->goToCue(cue_num);
//		this_thread::sleep_for(chrono::milliseconds(500));
//	}
//
//	// Back to dark
//	layer->goToCue(0);
//	this_thread::sleep_for(chrono::milliseconds(800));
//
//	// Light all
//	layer->goToCue(num_cues - 1);
//	this_thread::sleep_for(chrono::milliseconds(1000));
//
//#ifdef USE_ARNOLD
//	cout << "Start rendering\n";
//	aap->rerender();
//	aap->endRecording();
//
//	while (aap->getMode() == RENDERING)
//		;
//#endif
	
}

void RegressiveTest::initCues() {
	//if (!m_testRig)
	//	return;
	//if (m_playback)
	//	delete m_playback;

	//m_playback = new Playback(m_testRig);
	//m_playback->attachToRig();
	//shared_ptr<CueList> list1(new CueList("list1"));
	//shared_ptr<Layer> layer1(new Layer(m_testRig, "layer1", 1));

	//layer1->setMode(Layer::BLEND_OPAQUE);
	//layer1->activate();

	//// Assume all lights are with default intensities of zero
	//m_testRig->getAllDevices().reset();
	//Cue cue1(m_testRig, 0.5f, 0.5f, 0.0f);
	//list1->storeCue(0, cue1);

	//// Light one by one
	//int count = 1;
	//for (string id : m_testRig->getAllDevices().getIds()) {
	//	DeviceSet dev = m_testRig->query(id);

	//	dev.setParam("intensity", 1.f);
	//	Cue cue(m_testRig, 0.5f, 0.5f, 0.0f);
	//	list1->storeCue(count++, cue);

	//	dev.setParam("intensity", 0.f);
	//}

	//// Light all up
	//for (string id : m_testRig->getAllDevices().getIds()) {
	//	DeviceSet dev = m_testRig->query(id);

	//	dev.setParam("intensity", 1.f);
	//}

	//Cue cue2(m_testRig, 0.5f, 0.5f, 0.0f);
	//list1->storeCue(count++, cue2);

	//m_testRig->getAllDevices().reset();

	//m_playback->addCueList(list1);
	//m_playback->addLayer(layer1);
	//m_playback->addCueListToLayer("list1", "layer1");
}
