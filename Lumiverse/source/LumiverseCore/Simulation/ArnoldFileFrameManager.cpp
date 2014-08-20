
#include "ArnoldFileFrameManager.h"

namespace Lumiverse {
ArnoldFileFrameManager::~ArnoldFileFrameManager() {
	delete []m_buffer;
}

void ArnoldFileFrameManager::dump(time_t time, float *frame, size_t width, size_t height) {
	size_t num_frames = (time - m_prev_time) * m_fps / 1000;
	size_t offset = (m_prev_time < 0) ? 0 : m_prev_time * m_fps / 1000;
	unsigned char *bytes = new unsigned char[width * height * 4];
	floats_to_bytes(bytes, frame, width, height);
	char digits[7];

	for (size_t i = 0; i < num_frames; i++) {
		sprintf(digits, "%06d", offset + i);

		std::stringstream ss;
		ss << "\\" << digits << ".png";
		std::string file = m_frame_path;
		file.append(ss.str());
		
		if (!imageio_save_image(file.c_str(), bytes, width, height)) {
			std::stringstream err_ss;
			err_ss << "Error to write png: " << file;
			Logger::log(ERR, err_ss.str());
		}
	}

	m_prev_time = (time - m_prev_time) * m_fps / m_fps;
}

bool ArnoldFileFrameManager::fileExists(std::string fileName) const {
	FILE *fp = fopen(fileName.c_str(), "rb");
	if (!fp)
		return false;
	fclose(fp);
	return true;
}

float *ArnoldFileFrameManager::getCurrentFrameBuffer() {
	char digits[7];
	sprintf(digits, "%06d", m_current);

	std::stringstream ss;
	ss << "\\" << digits << ".png";
	std::string file = m_frame_path;
	file.append(ss.str());
	if (!fileExists(file))
		return NULL;

	int width, height;
	unsigned char *buffer = imageio_load_image(file.c_str(), &width, &height);

	if (!m_buffer)
		m_buffer = new float[width * height * 4];
	bytes_to_floats(m_buffer, buffer, width, height);
	delete []buffer;

	return m_buffer;
}

time_t ArnoldFileFrameManager::getCurrentTime() const {
	return m_current * 1000.f / m_fps;
}

time_t ArnoldFileFrameManager::getNextTime() const {
	return (m_current + 1) * 1000.f / m_fps;
}
    
bool ArnoldFileFrameManager::hasNext() const {
	char digits[7];
	sprintf(digits, "%06d", m_current + 1);

	std::stringstream ss;
	ss << "\\" << digits << ".png";
	std::string file = m_frame_path;
	file.append(ss.str());

	return fileExists(file);
}

void ArnoldFileFrameManager::clear() {

}

bool ArnoldFileFrameManager::isEmpty() const {
	std::stringstream ss;
	ss << "\\" << "000000.png";
	std::string file = m_frame_path;
	file.append(ss.str());

	return !fileExists(file);
}

size_t ArnoldFileFrameManager::getFrameNum() const {
	return 0;
}

}
