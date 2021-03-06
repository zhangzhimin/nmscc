#pragma once

#include <nms/serialization/base.h>
#include <nms/serialization/node.h>

namespace nms::serialization::xml
{

NMS_API void formatImpl(String& buf, const NodeEx& tree, StrView fmt);

}
