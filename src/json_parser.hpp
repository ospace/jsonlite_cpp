#ifndef __JS_JSON_PARSER_HPP_20130410__
#define __JS_JSON_PARSER_HPP_20130410__

#include "jsonlite.hpp"

namespace jslite {

class JsonTokenzier;

class JsonParser {
public:
	JsonParser() : tokenizer_(NULL) {}

	Json* Parse(const std::string& str);

	int32_t Parse(Json& json, const std::string& str);

	std::string error_str(int32_t errno) const;

protected:
	int32_t ParseValue(Json &json, size_t depth = 0);

	int32_t ParseArray(Json &json, size_t depth);
	
	int32_t ParseObject(Json &json, size_t depth);

	void TrimSpace();

private:
	JsonTokenzier *tokenizer_;
};

} // namespace jslite

#endif //__JS_JSON_PARSER_HPP_20130410__