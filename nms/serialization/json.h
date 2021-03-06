#pragma once

#include <nms/serialization/base.h>
#include <nms/serialization/node.h>

namespace nms::serialization::json
{

NMS_API Tree parse(StrView s);
NMS_API void formatImpl(String& buf, const NodeEx& tree, StrView fmt);

}
