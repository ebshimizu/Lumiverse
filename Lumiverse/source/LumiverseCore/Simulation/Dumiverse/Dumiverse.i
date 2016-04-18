%module dumiverse_swig

%{
#include "Dumiverse.h"
%}

extern void init(float m_gamma, bool m_predictive, const char *m_plugins, const char *filename);
extern void close();
extern float *getFrameBuffer();
extern int renderWrapper();
extern void interrupt();
extern int getWidth();
extern int getHeight();
float getPercentage();

