
#include "ArnoldPatch.h"
#include "types/LumiverseVector.h"
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

		i++;
	}

}

void ArnoldPatch::setParameter(AtNode *light_ptr, const std::string &paramName, LumiverseType *val_ptr) {
    const AtNodeEntry *entry_ptr = AiNodeGetNodeEntry(light_ptr);
    const AtParamEntry *param_ptr = AiNodeEntryLookUpParameter(entry_ptr, paramName.c_str());
    
    // TODO: handle array
    if (AiParamGetType(param_ptr) == AI_TYPE_ARRAY) {
        if (val_ptr->getTypeName() == "float") {
            
        }
        if (val_ptr->getTypeName() == "vector3") {
            AtArray *array_ptr = AiArray(1, 1, AI_TYPE_POINT);
            LumiverseVector<3> *pnt_v = (LumiverseVector<3> *)val_ptr;
            AtPoint point;
            point.x = pnt_v->getVal(0);
            point.y = pnt_v->getVal(1);
            point.z = pnt_v->getVal(2);
            AiArraySetPnt(array_ptr, 0, point);
            AiNodeSetArray(light_ptr, paramName.c_str(), array_ptr);
        }
    }
    
    // TODO: add more lumiverse types?
    switch (AiParamGetType(param_ptr)) {
        case AI_TYPE_FLOAT:
            if (val_ptr->getTypeName() == "float")
                AiNodeSetFlt(light_ptr, paramName.c_str(), ((LumiverseFloat*)val_ptr)->getVal());
            break;
        case AI_TYPE_INT:
            if (val_ptr->getTypeName() == "float")
                AiNodeSetInt(light_ptr, paramName.c_str(), (int)((LumiverseFloat*)val_ptr)->getVal());
            break;
        case AI_TYPE_BOOLEAN:
            if (val_ptr->getTypeName() == "float")
                AiNodeSetBool(light_ptr, paramName.c_str(), (int)((LumiverseFloat*)val_ptr)->getVal() > 1e-3);
            break;
        case AI_TYPE_RGB:
            if (val_ptr->getTypeName() == "vector3") {
                LumiverseVector<3> *rgb_v = (LumiverseVector<3> *)val_ptr;
                AiNodeSetRGB(light_ptr, paramName.c_str(), rgb_v->getVal(0), rgb_v->getVal(1), rgb_v->getVal(2));
            }
            break;
        case AI_TYPE_POINT:
            if (val_ptr->getTypeName() == "vector3") {
                LumiverseVector<3> *pnt_v = (LumiverseVector<3> *)val_ptr;
                AiNodeSetPnt(light_ptr, paramName.c_str(), pnt_v->getVal(0), pnt_v->getVal(1), pnt_v->getVal(2));
            }
            break;
        case AI_TYPE_VECTOR:
            if (val_ptr->getTypeName() == "vector3") {
                LumiverseVector<3> *vec_v = (LumiverseVector<3> *)val_ptr;
                AiNodeSetVec(light_ptr, paramName.c_str(), vec_v->getVal(0), vec_v->getVal(1), vec_v->getVal(2));
            }
            break;
        case AI_TYPE_RGBA:
            if (val_ptr->getTypeName() == "vector4") {
                LumiverseVector<4> *rgba_v = (LumiverseVector<4> *)val_ptr;
                AiNodeSetRGBA(light_ptr, paramName.c_str(),
                              rgba_v->getVal(0), rgba_v->getVal(1), rgba_v->getVal(2), rgba_v->getVal(3));
            }
            break;
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
    for (std::string param : d_ptr->getParamNames()) {
        setParameter(light_ptr, param, d_ptr->getParam(param));
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
    AiBegin();
    
	bool render_req = updateLight(devices);

    if (render_req) {
        std::cout << AiASSLoad(m_ass_file.c_str(), AI_NODE_ALL & ~AI_NODE_LIGHT) << std::endl;
    }
    
    AtNodeIterator *itr_ptr = AiUniverseGetNodeIterator(AI_NODE_ALL);
    
    while (!AiNodeIteratorFinished(itr_ptr)) {
        AtNode *node = AiNodeIteratorGetNext(itr_ptr);
        std::cout << AiNodeGetName(node) << std::endl;
    }
    
    AiNodeIteratorDestroy(itr_ptr);
    
    AiRender(AI_RENDER_MODE_CAMERA);
    
    AiEnd();
}

/*!
* \brief Initializes connections and other network settings for the patch.
*
* Call this AFTER all interfaces have been assigned. May need to call again
* if interfaces change.
*/
void ArnoldPatch::init() {

}

/*!
* \brief Closes connections to the interfaces.
*/
void ArnoldPatch::close() {

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
