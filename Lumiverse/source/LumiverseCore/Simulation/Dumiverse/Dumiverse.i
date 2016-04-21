%module dumiverse_swig

%{
#include "Dumiverse.h"
%}

extern void init(const char *jsonPatchStr, const char *filename);
extern void close();
extern float *getFrameBuffer();
extern int renderWrapper(const char *jsonDevicesStr, const char *jsonSettingsStr);
extern void interrupt();
extern int getWidth();
extern int getHeight();
float getPercentage();

