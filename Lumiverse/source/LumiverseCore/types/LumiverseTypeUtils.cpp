#include "LumiverseTypeUtils.h"

LumiverseType* LumiverseTypeUtils::copy(LumiverseType* data) {
  if (data == nullptr)
    return nullptr;

  if (data->getTypeName() == "float")
    return (LumiverseType*)(new LumiverseFloat(data));
  else
    return nullptr;
}

bool LumiverseTypeUtils::equals(LumiverseType* lhs, LumiverseType* rhs) {
  if (lhs == nullptr || rhs == nullptr)
    return false;

  if (lhs->getTypeName() != rhs->getTypeName())
    return false;

  // At this point we can use just the lhs to determine type
  if (lhs->getTypeName() == "float")
    return (*((LumiverseFloat*)lhs) == *((LumiverseFloat*)rhs));
  else
    return false;
}