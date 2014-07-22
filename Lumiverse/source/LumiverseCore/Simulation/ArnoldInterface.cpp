
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

// TODO: handle illegal formats
template<size_t D, typename T>
void ArnoldInterface::parseArnoldParameter(const std::string &value, ArnoldParameterVector<D, T> &vector) const {
    T element;
    std::string value_spaceless = value;
    std::string pattern;
    
    // Removes spaces when the input type is not string
    if (typeid(std::string) != typeid(T))
        std::remove_if(value_spaceless.begin(), value_spaceless.end(),
                                                   [](char x){return std::isspace(x);});
    
    // Reads two different types
    if (typeid(T) == typeid(int) || typeid(T) == typeid(bool)) {
        pattern = "%d";
    }
    else if (typeid(T) == typeid(float)) {
        pattern = "%f";
    }
    
    // Format: "v1, v2, ..."
    size_t offset = 0;
    for (size_t i = 0; i < D; i++) {
        sscanf(value_spaceless.c_str() + offset, pattern.c_str(), &element);
        /*
        else {
            // If input type is string, find substrings divided by commas.
            int comma;
            if ((comma = value_spaceless.find(",", offset)) != std::string::npos)
                element = value_spaceless.substr(offset, comma - offset);
            else
                element = value_spaceless;
        }
         */
        vector[i] = element;
        
        offset = value_spaceless.find(",", offset);
        
        if (offset == std::string::npos)
            break;
        offset++;
    }
}

template<size_t D, typename T>
    void ArnoldInterface::setSingleParameter(AtNode *node, const std::string &paramName, const std::string &value,
                                         union AiNodeSetter<T> aiNodeSetter) const {
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
    
void ArnoldInterface::setParameter(AtNode *light_ptr, const std::string &paramName, const std::string &value) {
    ArnoldParam param = m_arnold_params[paramName];
    
    // Calls another function if the parameter type is "array"
    const AtNodeEntry *entry_ptr = AiNodeGetNodeEntry(light_ptr);
    const AtParamEntry *param_ptr = AiNodeEntryLookUpParameter(entry_ptr, paramName.c_str());
    if (AiParamGetType(param_ptr) == AI_TYPE_ARRAY) {
        setArrayParameter(light_ptr, paramName, value);
        return ;
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
                union AiNodeSetter<const char*> AiNodeSetter;
                AiNodeSetter.AiNodeSetter1D = AiNodeSetStr;
                setSingleParameter<1, const char*>(light_ptr, paramName, value, AiNodeSetter);
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
        default:
            break;
    }

}

void ArnoldInterface::init() {
    // Starts a arnold session
    AiBegin();
    
    // Doesn't read light node from the ass file
    AiASSLoad(m_ass_file.c_str(), AI_NODE_ALL & ~AI_NODE_LIGHT);
    AiLoadPlugins(m_plugin_dir.c_str());
    
    AtNode *options = AiUniverseGetOptions();
    m_width = AiNodeGetInt(options, "xres");
    m_height = AiNodeGetInt(options, "yres");
    
    // Set a driver to output result into a float buffer
    AtNode *driver = AiNode("driver_buffer");
    AiNodeSetStr(driver, "name", "buffer_driver");
    AiNodeSetInt(driver, "width", m_width);
    AiNodeSetInt(driver, "height", m_height);
    AiNodeSetFlt(driver, "gamma", m_gamma);
    
    // Assume we are using RGBA
    m_buffer = new float[m_width * m_height * 4];

    AiNodeSetPtr(driver, "buffer_pointer", m_buffer);
    
    // Register the driver to the arnold options
    // TODO: more drivers
    AtArray *outputs_array = AiArrayAllocate(1, 1, AI_TYPE_STRING);
    AiArraySetStr(outputs_array, 0, "RGB RGB filter buffer_driver");
    AiNodeSetArray(options, "outputs", outputs_array);
}

void ArnoldInterface::close() {
    AiEnd();
}

// TODO:
JSONNode ArnoldInterface::toJSON() {
	return JSONNode("test", 0);
}

}
