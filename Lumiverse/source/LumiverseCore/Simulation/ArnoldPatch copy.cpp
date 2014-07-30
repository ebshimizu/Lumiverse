
#include "ArnoldPatch.h"
#include "types/LumiverseFloat.h"

namespace Lumiverse {
    
ArnoldPatch::ArnoldPatch() {

}

/*!
* \brief Construct DMXPatch from JSON data.
*
* \param data JSONNode containing the DMXPatch object data.
*/
ArnoldPatch::ArnoldPatch(const JSONNode data) {
	loadJSON(data);
}

void ArnoldPatch::loadJSON(const JSONNode data) {
	string patchName = data.name();

	// Load options for raytracer application. (window, ray tracer, filter)
	JSONNode::const_iterator i = data.begin();

	// Two rounds. In the first round, initialize the size of output (window), so it's
	// possible to allocate memory for each light.
	while (i != data.end()) {
		std::string nodeName = i->name();

		if (nodeName == "sceneFile") {
          JSONNode fileName = *i;
          m_ass_file = fileName.as_string();
		}
        
        if (nodeName == "lights") {
			JSONNode lights = *i;
			JSONNode::const_iterator light = lights.begin();
			while (light != lights.end()) {
				//loadLight(*light);
				std::string light_name = light->name();
                
				m_lights[light_name] = NULL;
				//m_light_params[light_name] = LightParam();
                
				Logger::log(INFO, "Added light");
                
				light++;
			}
		}
        
        if (nodeName == "arnoldParamMaps") {
			JSONNode params = *i;
			JSONNode::const_iterator param = params.begin();
			while (param != params.end()) {
				//loadLight(*light);
				std::string param_name = param->name();
                
                ArnoldParam arnold_param;
                loadArnoldParam(*param, arnold_param);
				m_arnold_params[param_name] = arnold_param;
                
				Logger::log(INFO, "Added param");
                
				param++;
			}
		}

		i++;
	}

}
    
void ArnoldPatch::loadArnoldParam(const JSONNode data, ArnoldParam &param) {
    JSONNode::const_iterator i = data.begin();
    
	// Two rounds. In the first round, initialize the size of output (window), so it's
	// possible to allocate memory for each light.
	while (i != data.end()) {
		std::string nodeName = i->name();
        
		if (nodeName == "dimension") {
            JSONNode dimension = *i;
            param.dimension = dimension.as_int();
		}
        
        if (nodeName == "arnoldType") {
			JSONNode type = *i;
            param.arnoldTypeName = type.as_string();
		}
        
		i++;
	}
}

template<size_t D, typename T>
void ArnoldPatch::parseArnoldParameter(const std::string &value, ArnoldParameterVector<D, T> &vector) const {
    T element;
    std::string value_spaceless = value;
    std::string pattern;
    
    
    std::string::iterator end_pos = std::remove_if(value_spaceless.begin(), value_spaceless.end(),
                                                   [](char x){return std::isspace(x);});
    value_spaceless.erase(end_pos, value_spaceless.end());
    
    if (typeid(T) == typeid(int) || typeid(T) == typeid(bool)) {
        pattern = "%d";
    }
    else if (typeid(T) == typeid(float)) {
        pattern = "%f";
    }
    
    size_t offset = 0;
    for (size_t i = 0; i < D; i++) {
        sscanf(value_spaceless.c_str() + offset, pattern.c_str(), &element);
        vector[i] = element;
        
        offset = value_spaceless.find(",", offset) + 1;
        
        if (offset == std::string::npos)
            break;
    }
}

template<size_t D, typename T>
    void ArnoldPatch::setSingleParameter(AtNode *node, const std::string &paramName, const std::string &value,
                                         union AiNodeSet<T> aiNodeSet) const {
    ArnoldParameterVector<D, T> paramVector;
    parseArnoldParameter<D, T>(value, paramVector);
    
    if (D == 1)
        aiNodeSet.AiNodeSet1D(node, paramName.c_str(), paramVector[0]);
    else if (D == 2)
        aiNodeSet.AiNodeSet2D(node, paramName.c_str(), paramVector[0], paramVector[1]);
    else if (D == 3)
        aiNodeSet.AiNodeSet3D(node, paramName.c_str(), paramVector[0], paramVector[1], paramVector[2]);
    else if (D == 4)
        aiNodeSet.AiNodeSet4D(node, paramName.c_str(), paramVector[0], paramVector[1], paramVector[2], paramVector[3]);
}

template<size_t D, typename T, class C>
void ArnoldPatch::setArrayParameter(AtNode *node, const std::string &paramName, const std::string &value,
                                    bool (*AiArraySet) (AtArray*, AtUInt32, C, const char*, int),
                                    const int AiType) const {
    ArnoldParameterVector<D, T> paramVector;
    std::vector<ArnoldParameterVector<D, T>> array;
    size_t offset = 0;
    while (1) {
        parseArnoldParameter<D, T>(value.substr(offset), paramVector);
        array.push_back(paramVector);
        
        offset = value.find(";", offset);
        
        if (offset == std::string::npos)
            break;
        
        offset++;
    }
    
    AtArray *array_ptr = AiArray(array.size(), 1, AiType);
    
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
    
void ArnoldPatch::setArrayParameter(AtNode *light_ptr, const std::string &paramName, const std::string &value) {
    ArnoldParam param = m_arnold_params[paramName];
    
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
    
void ArnoldPatch::setParameter(AtNode *light_ptr, const std::string &paramName, const std::string &value) {
    ArnoldParam param = m_arnold_params[paramName];
    
    const AtNodeEntry *entry_ptr = AiNodeGetNodeEntry(light_ptr);
    const AtParamEntry *param_ptr = AiNodeEntryLookUpParameter(entry_ptr, paramName.c_str());
    if (AiParamGetType(param_ptr) == AI_TYPE_ARRAY) {
        setArrayParameter(light_ptr, paramName, value);
        return ;
    }
    
    switch (param.dimension) {
        case 1: {
            if (param.arnoldTypeName == "int") {
                union AiNodeSet<int> aiNodeSet;
                aiNodeSet.AiNodeSet1D = AiNodeSetInt;
                setSingleParameter<1, int>(light_ptr, paramName, value, aiNodeSet);
            }
            else if (param.arnoldTypeName == "uint") {
                union AiNodeSet<unsigned int> aiNodeSet;
                aiNodeSet.AiNodeSet1D = AiNodeSetUInt;
                setSingleParameter<1, unsigned int>(light_ptr, paramName, value, aiNodeSet);
                
                /*
                ArnoldParameterVector<1, unsigned int> paramVector;
                parseArnoldParameter<1, unsigned int>(value, paramVector);
                
                AiNodeSetUInt(light_ptr, paramName.c_str(), paramVector[0]);
                 */
            }
            else if (param.arnoldTypeName == "bool") {
                union AiNodeSet<bool> aiNodeSet;
                aiNodeSet.AiNodeSet1D = AiNodeSetBool;
                setSingleParameter<1, bool>(light_ptr, paramName, value, aiNodeSet);
            }
            else if (param.arnoldTypeName == "float") {
                union AiNodeSet<float> aiNodeSet;
                aiNodeSet.AiNodeSet1D = AiNodeSetFlt;
                setSingleParameter<1, float>(light_ptr, paramName, value, aiNodeSet);
            }
        
            break;
        }
        case 2: {
            if (param.arnoldTypeName == "point2") {
                ArnoldParameterVector<2, float> paramVector;
                parseArnoldParameter<2, float>(value, paramVector);
                
                AiNodeSetPnt2(light_ptr, paramName.c_str(), paramVector[0], paramVector[1]);
            }

            break;
        }
        case 3: {
            ArnoldParameterVector<3, float> paramVector;
            parseArnoldParameter<3, float>(value, paramVector);
            
            if (param.arnoldTypeName == "rgb") {
                AiNodeSetRGB(light_ptr, paramName.c_str(),
                             paramVector[0], paramVector[1], paramVector[2]);
            }
            else if (param.arnoldTypeName == "vector") {
                AiNodeSetVec(light_ptr, paramName.c_str(),
                             paramVector[0], paramVector[1], paramVector[2]);
            }
            else if (param.arnoldTypeName == "point") {
                AiNodeSetPnt(light_ptr, paramName.c_str(),
                             paramVector[0], paramVector[1], paramVector[2]);
            }
            
            break;
        }
        case 4: {
            ArnoldParameterVector<4, float> paramVector;
            parseArnoldParameter<4, float>(value, paramVector);
            
            if (param.arnoldTypeName == "rgba") {
                AiNodeSetRGBA(light_ptr, paramName.c_str(),
                             paramVector[0], paramVector[1], paramVector[2], paramVector[3]);
            }

            break;
        }
        default:
            break;
    }

}
    
void ArnoldPatch::loadLight(Device *d_ptr) {
	std::string light_name = d_ptr->getId();
	std::string type = d_ptr->getType();
	AtNode *light_ptr;
    
    if (m_lights[light_name] == NULL) {
        light_ptr = AiNode(type.c_str());
        AiNodeSetStr(light_ptr, "name", light_name.c_str());
    }
    else {
        light_ptr = m_lights[light_name];
    }

    // TODO: mesh_light
    for (std::string meta : d_ptr->getMetadataKeyNames()) {
        std::string value;
        d_ptr->getMetadata(meta, value);
        setParameter(light_ptr, meta, value);
    }

	m_lights[light_name] = light_ptr;
}

/*!
* \brief Destroys the object.
*/
ArnoldPatch::~ArnoldPatch() {

}

bool ArnoldPatch::updateLight(set<Device *> devices) {
    bool rerender_rep = false;
    
	for (Device* d : devices) {
		std::string name = d->getId();
		if (m_lights.count(name) == 0)
			continue;
		
        if (d->needsUpdate()) {
            rerender_rep = true;
            loadLight(d);
        }
	}
    
    return rerender_rep;
}

/*!
* \brief Updates the values sent to the DMX network given the list of devices
* in the rig.
*
* The list of devices should be maintained outside of this class.
*/
// TODO: Relationship between Device and Light??
// (Simulation light)
void ArnoldPatch::update(set<Device *> devices) {
    //AiBegin();
    
	bool render_req = updateLight(devices);

    if (render_req) {
        //std::cout << "Loading ass ..." << AiASSLoad(m_ass_file.c_str(), AI_NODE_ALL & ~AI_NODE_LIGHT) << std::endl;
    }
    
    AtNodeIterator *itr_ptr = AiUniverseGetNodeIterator(AI_NODE_ALL);
    
    while (!AiNodeIteratorFinished(itr_ptr)) {
        AtNode *node = AiNodeIteratorGetNext(itr_ptr);
        std::cout << AiNodeGetName(node) << std::endl;
    }
    
    AiNodeIteratorDestroy(itr_ptr);
    
    AiRender(AI_RENDER_MODE_CAMERA);
    
    //AiEnd();
}

/*!
* \brief Initializes connections and other network settings for the patch.
*
* Call this AFTER all interfaces have been assigned. May need to call again
* if interfaces change.
*/
void ArnoldPatch::init() {
    AiBegin();
    AiASSLoad(m_ass_file.c_str(), AI_NODE_ALL & ~AI_NODE_LIGHT);
    
    AtNode *options = AiUniverseGetOptions();
    int width = AiNodeGetInt(options, "xres");
    int height = AiNodeGetInt(options, "yres");
    
    AtNode *driver = AiNode("driver_sdl");
    AiNodeSetStr(driver, "name", "sdl_driver");
    AiNodeSetInt(driver, "width", width);
    AiNodeSetInt(driver, "height", height);
    
    // TODO: better
    AtArray *outputs_array = AiArrayAllocate(1, 1, AI_TYPE_STRING);
    AiArraySetStr(outputs_array, 0, "RGB RGB filter sdl_driver");
    AiNodeSetArray(options, "outputs", outputs_array);
    //AiEnd();
}

/*!
* \brief Closes connections to the interfaces.
*/
void ArnoldPatch::close() {
    AiEnd();
}

/*!
* \brief Exports a JSONNode with the data in this patch
*
* \return JSONNode containing the DMXPatch object
*/
JSONNode ArnoldPatch::toJSON() {
	return JSONNode("test", 0);
}

}
