
#include "ArnoldFileFrameManager.h"

namespace Lumiverse {
ArnoldFileFrameManager::~ArnoldFileFrameManager() {

}

void ArnoldFileFrameManager::dump(time_t time, float *frame, size_t width, size_t height) {
	size_t num_frames = (time - m_prev_time) * m_fps / 1000;
	size_t offset = (m_prev_time < 0) ? 0 : m_prev_time * m_fps / 1000;
	unsigned char *bytes = new unsigned char[width * height * 4];

	for (size_t i = 0; i < num_frames; i++) {
		std::stringstream ss;
		ss << "\\" << offset + i << ".png";
		std::string file = m_frame_path.append(ss.str());
		floats_to_bytes(bytes, frame, width, height);

		if (!imageio_save_image(file.c_str(), bytes, width, height)) {
			std::stringstream err_ss;
			err_ss << "Error to write png: " << file;
			Logger::log(ERR, err_ss.str());
		}
	}

	m_prev_time = (time - m_prev_time) * m_fps / m_fps;
}

float *ArnoldFileFrameManager::getCurrentFrameBuffer() const {
	return NULL;
}

time_t ArnoldFileFrameManager::getCurrentTime() const {
	return 0;
}

time_t ArnoldFileFrameManager::getNextTime() const {
	return 0;
}
    
bool ArnoldFileFrameManager::hasNext() const {
	return false;
}

void ArnoldFileFrameManager::clear() {

}

bool ArnoldFileFrameManager::isEmpty() const {
	return false;
}

size_t ArnoldFileFrameManager::getFrameNum() const {
	return 0;
}

}
