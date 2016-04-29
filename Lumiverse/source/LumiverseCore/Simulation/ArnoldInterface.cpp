
#include "ArnoldInterface.h"
#include "types/LumiverseFloat.h"
#include <sstream>

namespace Lumiverse {
    
void ArnoldInterface::loadArnoldParam(const JSONNode data) {
  ArnoldParam arnold_param;
  JSONNode::const_iterator i = data.begin();
    
	while (i != data.end()) {
		std::string nodeName = i->name();
        
    // Two values for parameter mapping.
		if (nodeName == "dimension") {
      JSONNode dimension = *i;
      arnold_param.dimension = dimension.as_int();
		}
        
    if (nodeName == "arnoldType") {
			JSONNode type = *i;
      arnold_param.arnoldTypeName = type.as_string();
		}
        
		i++;
	}
    
  m_arnold_params[data.name()] = arnold_param;
}

template<size_t D, typename T>
void ArnoldInterface::setSingleParameter(AtNode *node, const std::string &paramName, const std::string &value,
                                     union AiNodeSetter<T> aiNodeSetter) const
{
  ArnoldParameterVector<D, T> paramVector;
  parseArnoldParameter<D, T>(value, paramVector);

  // Calls APIs with different dimension.
  if (D == 1)
    aiNodeSetter.AiNodeSetter1D(node, paramName.c_str(), paramVector[0]);
  else if (D == 2)
    aiNodeSetter.AiNodeSetter2D(node, paramName.c_str(), paramVector[0], paramVector[1]);
  else if (D == 3)
    aiNodeSetter.AiNodeSetter3D(node, paramName.c_str(), paramVector[0], paramVector[1], paramVector[2]);
  else if (D == 4)
    aiNodeSetter.AiNodeSetter4D(node, paramName.c_str(), paramVector[0], paramVector[1], paramVector[2], paramVector[3]);
}

template<size_t D, typename T, class C>
void ArnoldInterface::setArrayParameter(AtNode *node, const std::string &paramName, const std::string &value,
                                    bool (*AiArraySet) (AtArray*, AtUInt32, C, const char*, int),
                                    const int AiType) const {
    ArnoldParameterVector<D, T> paramVector;
    std::vector<ArnoldParameterVector<D, T>> array;
    size_t offset = 0;

    // Cuts input with ;
    while (1) {
        parseArnoldParameter<D, T>(value.substr(offset), paramVector);
        array.push_back(paramVector);
        
        offset = value.find(";", offset);
        
        if (offset == std::string::npos)
            break;
        
        offset++;
    }
    
    AtArray *array_ptr = AiArray(array.size(), 1, AiType);
    
    // Sets elements of the array
    for (size_t i = 0; i < array.size(); i++) {
        C element;
        T *ele_ptr = (T*)&element;
        
        for (size_t j = 0; j < D; j++) {
            *(ele_ptr + j) = array[i][j];
        }
        
        AiArraySet(array_ptr, i, element, __AI_FILE__, __AI_LINE__);
    }
    
    AiNodeSetArray(node, paramName.c_str(), array_ptr);
}
    
void ArnoldInterface::setArrayParameter(AtNode *light_ptr, const std::string &paramName, const std::string &value) {
    ArnoldParam param = m_arnold_params[paramName];
    
    // Handles different function calls
    if (param.arnoldTypeName == "int") {
        setArrayParameter<1, int, int>(light_ptr, paramName, value, AiArraySetIntFunc, AI_TYPE_INT);
    }
    else if (param.arnoldTypeName == "uint") {
        setArrayParameter<1, unsigned int, unsigned int>(light_ptr, paramName, value, AiArraySetUIntFunc, AI_TYPE_UINT);
    }
    else if (param.arnoldTypeName == "bool") {
        setArrayParameter<1, bool, bool>(light_ptr, paramName, value, AiArraySetBoolFunc, AI_TYPE_BOOLEAN);
    }
    else if (param.arnoldTypeName == "float") {
        setArrayParameter<1, float, float>(light_ptr, paramName, value, AiArraySetFltFunc, AI_TYPE_FLOAT);
    }
    else if (param.arnoldTypeName == "point2") {
        setArrayParameter<2, float, AtPoint2>(light_ptr, paramName, value, AiArraySetPnt2Func, AI_TYPE_POINT2);
    }
    else if (param.arnoldTypeName == "rgb") {
        setArrayParameter<3, float, AtRGB>(light_ptr, paramName, value, AiArraySetRGBFunc, AI_TYPE_RGB);
    }
    else if (param.arnoldTypeName == "vector") {
        setArrayParameter<3, float, AtVector>(light_ptr, paramName, value, AiArraySetVecFunc, AI_TYPE_VECTOR);
    }
    else if (param.arnoldTypeName == "point") {
        setArrayParameter<3, float, AtPoint>(light_ptr, paramName, value, AiArraySetPntFunc, AI_TYPE_POINT);
    }
    else if (param.arnoldTypeName == "rgba") {
        setArrayParameter<4, float, AtRGBA>(light_ptr, paramName, value, AiArraySetRGBAFunc, AI_TYPE_RGBA);
    }
}

void ArnoldInterface::setOptionParameter(const std::string & paramName, int val)
{
  AtNode* options = AiUniverseGetOptions();
  AiNodeSetInt(options, paramName.c_str(), val);
}

void ArnoldInterface::setOptionParameter(const std::string & paramName, float val)
{
  AtNode* options = AiUniverseGetOptions();
  AiNodeSetInt(options, paramName.c_str(), val);
}

int ArnoldInterface::getOptionParameter(const std::string & paramName)
{
  AtNode* options = AiUniverseGetOptions();
  return AiNodeGetInt(options, paramName.c_str());
}
    
bool ArnoldInterface::setDims(int w, int h)
{
  if (m_open) {
    // Adjust global options
    AtNode* options = AiUniverseGetOptions();
    AiNodeSetInt(options, "xres", w);
    AiNodeSetInt(options, "yres", h);

    m_width = w;
    m_height = h;

    if (m_buffer != nullptr) {
      delete[] m_buffer;
      m_buffer = new float[m_width * m_height * 4];

      AtNode* driverBuf = AiNodeLookUpByName(m_bufDriverName.c_str());
      if (driverBuf != nullptr) {
        AiNodeSetPtr(driverBuf, "buffer_pointer", m_buffer);
        AiNodeSetInt(driverBuf, "width", m_width);
        AiNodeSetInt(driverBuf, "height", m_height);
      }
    }

    return true;
  }

  return false;
}

void ArnoldInterface::setParameter(string lightName, string param, int val)
{
  AtNode* light = AiNodeLookUpByName(lightName.c_str());

  if (light == NULL)
    return;

  AiNodeSetInt(light, param.c_str(), val);
}

void ArnoldInterface::setParameter(string lightName, string param, float val)
{
  AtNode* light = AiNodeLookUpByName(lightName.c_str());

  if (light == NULL)
    return;

  AiNodeSetFlt(light, param.c_str(), val);
}

void ArnoldInterface::setParameter(string lightName, string param, Eigen::Matrix3f rot, Eigen::Vector3f trans)
{
  AtNode* light = AiNodeLookUpByName(lightName.c_str());

  if (light == NULL)
    return;

  AtMatrix transform;
  transform[0][0] = rot(0, 0);
  transform[0][1] = rot(0, 1);
  transform[0][2] = rot(0, 2);
  transform[0][3] = 0;
  transform[1][0] = rot(1, 0);
  transform[1][1] = rot(1, 1);
  transform[1][2] = rot(1, 2);
  transform[1][3] = 0;
  transform[2][0] = rot(2, 0);
  transform[2][1] = rot(2, 1);
  transform[2][2] = rot(2, 2);
  transform[2][3] = 0;
  transform[3][0] = trans(0);
  transform[3][1] = trans(1);
  transform[3][2] = trans(2);
  transform[3][3] = 1;

  AiNodeSetMatrix(light, param.c_str(), transform);
}

void ArnoldInterface::setParameter(string lightName, string param, float x, float y, float z)
{
  AtNode* light = AiNodeLookUpByName(lightName.c_str());

  if (light == NULL)
    return;

  AiNodeSetRGB(light, param.c_str(), x, y, z);
}

void ArnoldInterface::setParameter(AtNode *light_ptr, const std::string &paramName, const std::string &value) {
  ArnoldParam param = m_arnold_params[paramName];

  // Calls another function if the parameter type is "array"
  const AtNodeEntry *entry_ptr = AiNodeGetNodeEntry(light_ptr);
  const AtParamEntry *param_ptr = AiNodeEntryLookUpParameter(entry_ptr, paramName.c_str());
  if (AiParamGetType(param_ptr) == AI_TYPE_ARRAY && paramName != "matrix") {
    setArrayParameter(light_ptr, paramName, value);
    return;
  }
    
  switch (param.dimension) {
    case 1: {
      if (param.arnoldTypeName == "int") {
        union AiNodeSetter<int> AiNodeSetter;
        AiNodeSetter.AiNodeSetter1D = AiNodeSetInt;
        setSingleParameter<1, int>(light_ptr, paramName, value, AiNodeSetter);
      }
      else if (param.arnoldTypeName == "uint") {
                union AiNodeSetter<unsigned int> AiNodeSetter;
                AiNodeSetter.AiNodeSetter1D = AiNodeSetUInt;
                setSingleParameter<1, unsigned int>(light_ptr, paramName, value, AiNodeSetter);
            }
            else if (param.arnoldTypeName == "bool") {
                union AiNodeSetter<bool> AiNodeSetter;
                AiNodeSetter.AiNodeSetter1D = AiNodeSetBool;
                setSingleParameter<1, bool>(light_ptr, paramName, value, AiNodeSetter);
            }
            else if (param.arnoldTypeName == "float") {
                union AiNodeSetter<float> AiNodeSetter;
                AiNodeSetter.AiNodeSetter1D = AiNodeSetFlt;
                setSingleParameter<1, float>(light_ptr, paramName, value, AiNodeSetter);
            }
            else if (param.arnoldTypeName == "string") {
				// First to see if the string is a path (if is a ies file).
				// If it is, converts to relative path.
				AiNodeSetStr(light_ptr, paramName.c_str(), toRelativePath(value).c_str());
            }
        
            break;
        }
        case 2: {
            if (param.arnoldTypeName == "point2") {
                union AiNodeSetter<float> AiNodeSetter;
                AiNodeSetter.AiNodeSetter2D = AiNodeSetPnt2;
                setSingleParameter<2, float>(light_ptr, paramName, value, AiNodeSetter);
            }

            break;
        }
        case 3: {
            union AiNodeSetter<float> AiNodeSetter;

            if (param.arnoldTypeName == "rgb") {
                AiNodeSetter.AiNodeSetter3D = AiNodeSetRGB;
                setSingleParameter<3, float>(light_ptr, paramName, value, AiNodeSetter);
            }
            else if (param.arnoldTypeName == "vector") {
                AiNodeSetter.AiNodeSetter3D = AiNodeSetVec;
                setSingleParameter<3, float>(light_ptr, paramName, value, AiNodeSetter);
            }
            else if (param.arnoldTypeName == "point") {
                AiNodeSetter.AiNodeSetter3D = AiNodeSetPnt;
                setSingleParameter<3, float>(light_ptr, paramName, value, AiNodeSetter);
            }
            
            break;
        }
        case 4: {
            union AiNodeSetter<float> AiNodeSetter;
            
            if (param.arnoldTypeName == "rgba") {
                AiNodeSetter.AiNodeSetter4D = AiNodeSetRGBA;
                setSingleParameter<4, float>(light_ptr, paramName, value, AiNodeSetter);
            }

            break;
        }
        case 16: {
            if (param.arnoldTypeName == "matrix") {
                ArnoldParameterVector<16, float> paramVector;
                parseArnoldParameter<16, float>(value, paramVector);
                AtMatrix matrix;
                
                AiM4Identity (matrix);
                
                for (size_t x = 0; x < 4; x++) {
                    for (size_t y = 0; y < 4; y++) {
                        matrix[x][y] = paramVector.getElements()[x * 4 + y];
                    }
                }
                
                AiNodeSetMatrix(light_ptr, paramName.c_str(), matrix);
            }
            
            break;
        }
        default:
            break;
    }

}

void ArnoldInterface::appendToOutputs(const std::string buffer_output) {
  // Append the new output to options (using the new filter)
  AtNode *options = AiUniverseGetOptions();
  AtArray *original = AiNodeGetArray(options, "outputs");
  AtUInt32 num_options = original->nelements + 1;
  
  AtArray *outputs_array = AiArrayAllocate(num_options, 1, AI_TYPE_STRING);
  
  for (size_t i = 0; i < num_options - 1; i++) {
    AiArraySetStr(outputs_array, i, AiArrayGetStr(original, i));
  }
  
  AiArraySetStr(outputs_array, num_options - 1, buffer_output.c_str());
  AiNodeSetArray(options, "outputs", outputs_array);
}

inline std::string ArnoldInterface::toRelativePath(std::string file) {
	if (!isRelativeFileName(file))
		return file;
	return m_default_path + file;
}

void ArnoldInterface::updateSurfaceColor(Eigen::Vector3d white) {
	Eigen::Matrix3d toxyz = RGBToXYZ[sRGB];
	// TODO: To query names of existing surface
	std::string names[] = {"left_srf", "right_srf", "bottom_srf", "top_srf", "back_srf", "character"};

	for (std::string srf : names) {
		AtNode *node = AiNodeLookUpByName(srf.c_str());
		AtRGBA rgba = AiNodeGetRGBA(node, "Kd_color");

		// To xyz
		Eigen::Vector3d xyz = RGBToXYZ[sRGB] * Eigen::Vector3d(rgba.r, rgba.g, rgba.b);

		// Multiplies by M_sharp
		Eigen::Vector3d sharp_rgb = RGBToXYZ[sharpRGB].inverse() * xyz;

		// White balance
		sharp_rgb = Eigen::Vector3d(sharp_rgb[0] / white[0], sharp_rgb[1] / white[1], sharp_rgb[2] / white[2]);

		AiNodeSetRGBA(node, "Kd_color", sharp_rgb[0], sharp_rgb[1], sharp_rgb[2], rgba.a);
	}
}

void ArnoldInterface::addGobo(AtNode *light_ptr, std::string file, float deg, float rot) {
	AtNode *gobo = AiNode("photometric_gobo");
	std::string name(AiNodeGetStr(light_ptr, "name"));
	AiNodeSetStr(gobo, "name", (name + "_gobo").c_str());

	AiNodeSetStr(gobo, "filename", toRelativePath(file).c_str());
	AiNodeSetFlt(gobo, "degree", deg);
	AiNodeSetFlt(gobo, "rotation", rot);

	AiNodeSetPtr(light_ptr, "filters", gobo);
}

void ArnoldInterface::setLogFileName(string filename, int flags)
{
  AiMsgSetLogFileName(filename.c_str());
  AiMsgSetLogFileFlags(flags);
}

map<string, AtNode*> ArnoldInterface::getLights()
{
  map<string, AtNode*> lights;

  // Iterate over lights
  AtNodeIterator* it = AiUniverseGetNodeIterator(AI_NODE_LIGHT);

  while (!AiNodeIteratorFinished(it)) {
    AtNode* l = AiNodeIteratorGetNext(it);
    lights[AiNodeGetName(l)] = l;
  }

  AiNodeIteratorDestroy(it);

  return lights;
}

void ArnoldInterface::setDriverFileName(string base, string filename)
{
  AtNode* exr = AiNodeLookUpByName("defaultArnoldDriver@driver_exr.RGBA");
  if (exr != nullptr) {
    AiNodeSetStr(exr, "filename", (base + "/exr/" + filename + ".exr").c_str());
  }

  AtNode* png = AiNodeLookUpByName("defaultArnoldDriver@driver_png.RGBA");
  if (png != nullptr) {
    AiNodeSetStr(png, "filename", (base + "/png/" + filename + ".png").c_str());
  }
}

bool ArnoldInterface::isDistributedOpen()
{
  return m_open;
}

void ArnoldInterface::init() {
  // TODO : to use env var (different apis for linux and win)
  // Make sure your environment variables are set properly to check out an arnold license.

  // Starts a arnold session
  AiBegin();

  setLogFileName("arnold.log");

	// Keeps directory of plugins absolute.
	AiLoadPlugins(m_plugin_dir.c_str());
    
  // Load everything from the scene file
	AiASSLoad(toRelativePath(m_ass_file).c_str(), AI_NODE_ALL);
    
  AtNode *options = AiUniverseGetOptions();
  m_width = AiNodeGetInt(options, "xres");
  m_height = AiNodeGetInt(options, "yres");
  m_samples = AiNodeGetInt(options, "AA_samples");
 
  // Set a driver to output result into a float buffer
  AtNode *driver;

  if (m_using_caching) {
	  driver = AiNode("cache_buffer");
	  m_bufDriverName = "cache_buffer";
  } else {
	  driver = AiNode("driver_buffer");
	  m_bufDriverName = "buffer_driver";
	  AiNodeSetFlt(driver, "gamma", m_gamma);
	  AiNodeSetBool(driver, "predictive", m_predictive);
  }
  
  std::stringstream ss;
  ss << chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() -
                                               chrono::system_clock::from_time_t(0)).count() % 1000;
  m_bufDriverName = m_bufDriverName.append(ss.str());
  
  AiNodeSetStr(driver, "name", m_bufDriverName.c_str());
  AiNodeSetInt(driver, "width", m_width);
  AiNodeSetInt(driver, "height", m_height);
    
  // Assume we are using RGBA
	delete[] m_buffer;
	m_buffer = NULL;
	delete[] m_bucket_pos;
	m_bucket_pos = NULL;
		
  m_buffer = new float[m_width * m_height * 4];
  AiNodeSetPtr(driver, "buffer_pointer", m_buffer);

	// Swapping threads more than hardware supports may cause problem.
	m_bucket_num = std::thread::hardware_concurrency();
	m_bucket_pos = new BucketPositionInfo[m_bucket_num];
	AiNodeSetPtr(driver, "bucket_pos_pointer", m_bucket_pos);

	AiNodeSetPtr(driver, "progress_pointer", &m_progress);
    
  // Create a filter
  AtNode *filter = AiNode("gaussian_filter");
  AiNodeSetStr(filter, "name", "filter");
  AiNodeSetFlt(filter, "width", 2);
    
  // Register the driver to the arnold options
  // The function keeps the output options from ass file
  std::string command("RGBA RGBA filter ");
  appendToOutputs(command.append(m_bufDriverName).c_str());

	m_open = true;
}

void ArnoldInterface::close() {
	// Cleans buffer
	delete[] m_buffer;
	m_buffer = NULL;
	delete[] m_bucket_pos;
	m_bucket_pos = NULL;

	// Couple up begin-end would avoid errors caused by a single call to end.
	if (m_open) {
		//AiBegin();

		AiEnd();
	}
	
	m_open = false;
}
    
void ArnoldInterface::setSamples(int samples)
{
  m_samples = samples;
  AtNode *options = AiUniverseGetOptions();
  AiNodeSetInt(options, "AA_samples", m_samples);
}

int ArnoldInterface::render() {
	if (!m_open)
		init();

  int code;

	// Sets the sampling rate with the current rate
	//setSamplesOption();

  Logger::log(INFO, "Rendering...");
  code = AiRender(AI_RENDER_MODE_CAMERA);

	std::stringstream ss;
	ss << "Done: " << code;
  Logger::log(INFO, ss.str().c_str());

  return code;
}
    
void ArnoldInterface::interrupt() {
  if (AiRendering()) {
    AiRenderInterrupt();
    Logger::log(INFO, "Aborted rendering to restart.");
  }
}

JSONNode ArnoldInterface::arnoldParameterToJSON() {
	JSONNode map;
	map.set_name("arnoldParamMaps");

	for (auto aparam : m_arnold_params) {
		JSONNode param;
		param.set_name(aparam.first);
		param.push_back(JSONNode("dimension", (int)aparam.second.dimension));
		param.push_back(JSONNode("arnoldType", aparam.second.arnoldTypeName));
		map.push_back(param);
	}

	return map;
}

}
