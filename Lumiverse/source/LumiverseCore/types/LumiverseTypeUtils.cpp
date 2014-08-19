#include "LumiverseTypeUtils.h"

namespace Lumiverse {
 
LumiverseType* LumiverseTypeUtils::copy(LumiverseType* data) {
  if (data == nullptr)
    return nullptr;

  if (data->getTypeName() == "float")
    return (LumiverseType*)(new LumiverseFloat(data));
  else if (data->getTypeName() == "enum")
    return (LumiverseType*)(new LumiverseEnum(data));
  else if (data->getTypeName() == "color")
    return (LumiverseType*)(new LumiverseColor(data));
  else if (data->getTypeName() == "orientation")
	  return (LumiverseType*)(new LumiverseOrientation(data));
  else
    return nullptr;
}

void LumiverseTypeUtils::copyByVal(LumiverseType* source, LumiverseType* target) {
  if (!LumiverseTypeUtils::areSameType(source, target))
    return;

  if (source->getTypeName() == "float") {
    *((LumiverseFloat*)target) = *((LumiverseFloat*)source);
  }
  else if (source->getTypeName() == "enum") {
    *((LumiverseEnum*)target) = *((LumiverseEnum*)source);
  }
  else if (source->getTypeName() == "color") {
    *((LumiverseColor*)target) = *((LumiverseColor*)source);
  }
  else if (source->getTypeName() == "orientation") {
	  *((LumiverseOrientation*)target) = *((LumiverseOrientation*)source);
  }
  else {
    return;
  }
}

bool LumiverseTypeUtils::equals(LumiverseType* lhs, LumiverseType* rhs) {
  if (!LumiverseTypeUtils::areSameType(lhs, rhs))
    return false;

  // At this point we can use just the lhs to determine type
  if (lhs->getTypeName() == "float")
    return (*((LumiverseFloat*)lhs) == *((LumiverseFloat*)rhs));
  else if (lhs->getTypeName() == "enum")
    return (*((LumiverseEnum*)lhs) == *((LumiverseEnum*)rhs));
  else if (lhs->getTypeName() == "color")
    return (*((LumiverseColor*)lhs) == *((LumiverseColor*)rhs));
  else if (lhs->getTypeName() == "orientation")
	  return (*((LumiverseOrientation*)lhs) == *((LumiverseOrientation*)rhs));
  else
    return false;
}

int LumiverseTypeUtils::cmp(LumiverseType* lhs, LumiverseType* rhs) {
  if (!LumiverseTypeUtils::areSameType(lhs, rhs))
    return -2;

  // At this point we can use just the lhs to determine type
  if (lhs->getTypeName() == "float")
  {
    if (*((LumiverseFloat*)lhs) == *((LumiverseFloat*)rhs))
      return 0;
    else if (*((LumiverseFloat*)lhs) < *((LumiverseFloat*)rhs))
      return -1;
    else
      return 1;
  }
  else if (lhs->getTypeName() == "enum") {
    if (*((LumiverseEnum*)lhs) == *((LumiverseEnum*)rhs))
      return 0;
    else if (*((LumiverseEnum*)lhs) < *((LumiverseEnum*)rhs))
      return -1;
    else
      return 1;
  }
  else if (lhs->getTypeName() == "color") {
    return (*((LumiverseColor*)lhs)).cmpHue(*((LumiverseColor*)rhs));
  }
  else if (lhs->getTypeName() == "orientation")
  {
	  if (*((LumiverseOrientation*)lhs) == *((LumiverseOrientation*)rhs))
		  return 0;
	  else if (*((LumiverseOrientation*)lhs) < *((LumiverseOrientation*)rhs))
		  return -1;
	  else
		  return 1;
  }
  else
    return -2;
}

shared_ptr<LumiverseType> LumiverseTypeUtils::lerp(LumiverseType* lhs, LumiverseType* rhs, float t) {
  if (!LumiverseTypeUtils::areSameType(lhs, rhs))
    return nullptr;

  if (lhs->getTypeName() == "float") {
    // Defaults and other meta-stuff are taken from lhs. Generally you should lerp
    // things that have the same defaults, etc.
    LumiverseFloat* ret = new LumiverseFloat();
    *ret = ((*(LumiverseFloat*)lhs) * (1 - t)) + ((*(LumiverseFloat*)rhs) * t);
    return shared_ptr<LumiverseType>((LumiverseType *)ret);
  }
  else if (lhs->getTypeName() == "enum") {
    // Redirect to lerp function within LumiverseEnum
    return ((LumiverseEnum*)lhs)->lerp((LumiverseEnum*)rhs, t);
  }
  else if (lhs->getTypeName() == "color") {
    // Redirect to lerp function within LumiverseColor
    return ((LumiverseColor*)lhs)->lerp((LumiverseColor*)rhs, t);
  }
  else if (lhs->getTypeName() == "orientation") {
	  LumiverseOrientation* ret = new LumiverseOrientation();
	  *ret = ((*(LumiverseOrientation*)lhs) * (1 - t)) + ((*(LumiverseOrientation*)rhs) * t);
	  return shared_ptr<LumiverseType>((LumiverseType *)ret);
  }
  else
    return nullptr;
}

inline bool LumiverseTypeUtils::areSameType(LumiverseType* lhs, LumiverseType* rhs) {
  if (lhs == nullptr || rhs == nullptr)
    return false;
  if (lhs->getTypeName() != rhs->getTypeName())
    return false;

  return true;
}

Eigen::Matrix3f LumiverseTypeUtils::getRotationMatrix(Eigen::Vector3f lookat, Eigen::Vector3f up, 
	LumiverseOrientation pan, LumiverseOrientation tilt) {
	Eigen::Matrix3f ret;

	lookat.normalize();
	up.normalize();

	// Gets axis for tilt
	Eigen::Vector3f tilt_axis = up.cross(lookat);
	tilt_axis.normalize();

	// Rotates to lookat (zero position)
	// By default, the light looks at -y.
	Eigen::Vector3f def_lookat(0.0f, -1.0f, 0.0f);
	Eigen::Vector3f def_axis = lookat.cross(def_lookat);
	def_axis.normalize();
	Eigen::AngleAxisf reset_rot = Eigen::AngleAxisf(std::acosf(def_lookat.dot(lookat)), def_axis);

	// Pan
	Eigen::Vector3f pan_reset_axis = up.cross(lookat);
	pan_reset_axis.normalize();
	Eigen::AngleAxisf pan_reset_rot = Eigen::AngleAxisf(std::acosf(up.dot(lookat)), pan_reset_axis);
	up = reset_rot * up;
	Eigen::AngleAxisf pan_rot = Eigen::AngleAxisf(pan.asUnit("radian"), up);

	// Tilt
	tilt_axis = reset_rot * tilt_axis;
	Eigen::AngleAxisf tilt_rot = Eigen::AngleAxisf(tilt.asUnit("radian"), tilt_axis);

	ret = tilt_rot * pan_rot * reset_rot;

	return ret;
}

bool LumiverseTypeUtils::lessThan(LumiverseType* lhs, LumiverseType* rhs) {
  // Nullptr is automatically less than anything (except nullptr)
  if (lhs == nullptr && rhs != nullptr)
    return true;

  return (cmp(lhs, rhs) == -1);
}

LumiverseType* LumiverseTypeUtils::loadFromJSON(JSONNode node) {
  bool err = false;

  auto type = node.find("type");
  if (type != node.end()) {
    // Add loading support for new types here
    if (type->as_string() == "float") {
      auto valNode = node.find("val");
      auto defNode = node.find("default");
      auto maxNode = node.find("max");
      auto minNode = node.find("min");

      if (valNode != node.end() && defNode != node.end()) {
        LumiverseFloat* param;

        if (maxNode != node.end() && minNode != node.end()) {
          param = new LumiverseFloat(valNode->as_float(), defNode->as_float(), maxNode->as_float(), minNode->as_float());
        }
        else {
          param = new LumiverseFloat(valNode->as_float(), defNode->as_float());
        }
        return  (LumiverseType*)param;
      }
      else {
        err = true;
      }
    }
    else if (type->as_string() == "enum") {
      auto activeNode = node.find("active");
      auto tweakNode = node.find("tweak");
      auto modeNode = node.find("mode");
      auto defaultNode = node.find("default");
      auto rangeNode = node.find("rangeMax");
      auto keysNode = node.find("keys");
      auto interpModeNode = node.find("interpMode");

      // If any of the above are missing we should abort
      if (modeNode != node.end() && defaultNode != node.end() &&
        rangeNode != node.end() && keysNode != node.end() && interpModeNode != node.end()) {
        // Get the keys into a map.
        map<string, int> enumKeys;
        JSONNode::const_iterator k = keysNode->begin();

        while (k != keysNode->end()) {
          JSONNode keyData = *k;

          enumKeys[keyData.name()] = keyData.as_int();
          k++;
        }

        // Make the enum
        LumiverseEnum* param = new LumiverseEnum(enumKeys, modeNode->as_string(), interpModeNode->as_string(),
          rangeNode->as_int(), defaultNode->as_string());

        if (activeNode != node.end())
          param->setVal(activeNode->as_string());
        if (tweakNode != node.end())
          param->setTweak(tweakNode->as_float());

        return (LumiverseType*)param;
      }
      else {
        err = true;
      }
    }
    else if (type->as_string() == "color") {
      auto channelsNode = node.find("channels");
      auto basisNode = node.find("basis");
      auto weightNode = node.find("weight");
      auto modeNode = node.find("mode");

      if (channelsNode != node.end() && basisNode != node.end() &&
        weightNode != node.end() && modeNode != node.end())
      {
        // Get the channel data into a map
        map<string, double> channels;
        JSONNode::const_iterator c = channelsNode->begin();

        while (c != channelsNode->end()) {
          JSONNode channelData = *c;

          channels[channelData.name()] = channelData.as_float();
          c++;
        }

        // Get the vector data into a map
        map<string, Eigen::Vector3d> basis;
        JSONNode::const_iterator b = basisNode->begin();

        while (b != basisNode->end()) {
          JSONNode basisData = *b;

          // This is an array of three values
          Eigen::Vector3d basisVector(basisData[0].as_float(), basisData[1].as_float(), basisData[2].as_float());
          basis[basisData.name()] = basisVector;
          b++;
        }

        LumiverseColor* color = new LumiverseColor(channels, basis, StringToColorMode[modeNode->as_string()], weightNode->as_float());

        return (LumiverseType*)color;
      }
      else {
        err = true;
      }
    }
	else if (type->as_string() == "orientation") {
		auto valNode = node.find("val");
		auto unitNode = node.find("unit");
		auto defNode = node.find("default");
		auto maxNode = node.find("max");
		auto minNode = node.find("min");

		if (valNode != node.end() && defNode != node.end()) {
			LumiverseOrientation* param;

			if (maxNode != node.end() && minNode != node.end()) {
				param = new LumiverseOrientation(valNode->as_float(), unitNode->as_string(), defNode->as_float(), maxNode->as_float(), minNode->as_float());
			}
			else {
				param = new LumiverseOrientation(valNode->as_float(), unitNode->as_string(), defNode->as_float());
			}
			return  (LumiverseType*)param;
		}
		else {
			err = true;
		}
	}
    else {
      stringstream ss;
      ss << "Unsupported type " << type->as_string() << " found when trying to load data.";
      Logger::log(WARN, ss.str());
    }
  }
  else {
    err = true;
  }

  return nullptr;
}

}