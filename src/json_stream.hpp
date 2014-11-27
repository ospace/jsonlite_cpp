#ifndef __JS_JSON_STREAM_HPP_20140928__
#define __JS_JSON_STREAM_HPP_20140928__

#include <string>
#include "jsonlite.hpp"

namespace jslite {

class JsonStream {
public:
	JsonStream();
	~JsonStream();
	
	int Print(std::ostream* os, const Json& json);
	int Print(const Json& json);

	void set_obj_sep(const std::string& sep);
	void set_array_sep(const std::string& sep);
	void set_indent_sep(const std::string& sep);
	void set_comma_sep(const std::string& sep);
	void set_colon_sep(const std::string& sep);
	
	std::string str() const;

protected:
	void PrintValue(std::ostream* os, const Json& json);
	void PrintObject(std::ostream* os, const Json& json);
	void PrintArray(std::ostream* os, const Json& json);
	void PrintString(std::ostream* os, const Json& json);

	void FormattingBegin(std::ostream* os, const std::string& sep);
	void FormattingIndent(std::ostream* os);
	void FormattingComma(std::ostream* os);
	void FormattingEnd(std::ostream* os, const std::string& sep);

private:
	std::ostringstream oss_;

	std::string obj_sep_;
	std::string array_sep_;
	std::string indent_sep_;
	std::string comma_sep_;
	std::string colon_sep_;
	uint32_t    indent_;
};

struct JOpt {
	JOpt(void (*func)(JsonStream&, const std::string&), const std::string& val) : func_(func), val_(val) {}
	void (*func_)(JsonStream&, const std::string&);
	std::string val_;
};

JOpt obj_sep(const std::string& val);

JOpt array_sep(const std::string& val);

JOpt indent_sep(const std::string& val);

JOpt comma_sep(const std::string& val);

JsonStream& default_sep(JsonStream& printer);

std::ostream& operator << (std::ostream& os, const Json& value);

JsonStream& operator << (JsonStream& printer, const Json& json);

JsonStream& operator << (JsonStream& printer, JsonStream& (__cdecl *func)(JsonStream&));

JsonStream& operator << (JsonStream& printer, const JOpt& opt);

} //namespace jslite

#endif //__JS_JSON_STREAM_HPP_20140928__