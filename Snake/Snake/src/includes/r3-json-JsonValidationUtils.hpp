
#include "../jsoncpp/json/json.h"
#pragma once

namespace r3 {

	namespace json {

		namespace ValidationUtils {

			bool requiredString(const Json::Value& jsonValue, const char* propertyName);
			bool optionalString(const Json::Value& jsonValue, const char* propertyName);

			bool requiredInt(const Json::Value& jsonValue, const char* propertyName);

			bool requiredArray(const Json::Value& jsonValue, const char* propertyName);

		}

	}

}
