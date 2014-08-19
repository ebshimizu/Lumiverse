
#include "ArnoldMemoryFrameManager.h"

namespace Lumiverse {

ArnoldMemoryFrameManager::~ArnoldMemoryFrameManager() {
    // Releases memory.
    clear();
}

void ArnoldMemoryFrameManager::dump(time_t time, float *frame, size_t width, size_t height) {
    FrameData fd;

    // Assume we are using RGBA.
    fd.time = time;
    fd.buffer = new float[width * height * 4];
    memcpy(fd.buffer, frame, sizeof(float) * width * height * 4);

    m_buffer.lock();
    m_bufferSet.insert(fd);
    m_buffer.unlock();
}

float *ArnoldMemoryFrameManager::getCurrentFrameBuffer() const {
    // moves to the current position
    // potentially inefficient
    size_t count = 0;
    for (FrameData fd : m_bufferSet) {
	if (count == m_current.load())
	    return fd.buffer;
	count++;
    }

    return NULL;
}

time_t ArnoldMemoryFrameManager::getCurrentTime() const {
    // moves to the current position
    // potentially inefficient
    size_t count = 0;
    for (FrameData fd : m_bufferSet) {
	if (count == m_current.load())
	    return fd.time;
	count++;
    }

    return -1;
}

time_t ArnoldMemoryFrameManager::getNextTime() const {
    if (hasNext()) {
        size_t count = 0;
        for (FrameData fd : m_bufferSet) {
            if (count == m_current.load() + 1)
                return fd.time;
            count++;
        }
    }
    
    return -1;
}
    
bool ArnoldMemoryFrameManager::hasNext() const {
    if (m_current.load() + 1 < m_bufferSet.size())
	return true;
    return false;
}

void ArnoldMemoryFrameManager::clear() {
    reset();
    for (FrameData fd : m_bufferSet) {
        delete [] fd.buffer;
    }
    m_bufferSet.clear();
}

}
