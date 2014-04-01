#include "LumiverseTypeUtils.h"

LumiverseType* LumiverseTypeUtils::copy(LumiverseType* data) {
  if (data == nullptr)
    return nullptr;

  if (data->getTypeName() == "float")
    return (LumiverseType*)(new LumiverseFloat(data));
  else if (data->getTypeName() == "enum")
    return (LumiverseType*)(new LumiverseEnum(data));
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
  if (lhs->getTypeName() == "enum") {
    if (*((LumiverseEnum*)lhs) == *((LumiverseEnum*)rhs))
      return 0;
    else if (*((LumiverseEnum*)lhs) < *((LumiverseEnum*)rhs))
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
  if (lhs->getTypeName() == "enum") {
    // Redirect to lerp function within LumiverseEnum
    return ((LumiverseEnum*)lhs)->lerp((LumiverseEnum*)rhs, t);
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