#ifndef XMLATTRIBUTE_HEADER_FILE_H
#define XMLATTRIBUTE_HEADER_FILE_H

#include <string>
#include <string_view>

namespace SimpleXml {

	struct XMLAttribute {
	public:
		std::string_view name;
		std::string_view value;

	};

}

#endif