#include "json_stream.hpp"
#include <ostream>

namespace jslite {

JsonStream::JsonStream() : indent_(0) { }

JsonStream::~JsonStream() { }

void JsonStream::FormattingBegin(std::ostream* os, const std::string& sep) {
	*os << sep;
	if (std::string::npos != sep.find_first_of('\n')) {
		++indent_;
		FormattingIndent(os);
	}
}

void JsonStream::FormattingEnd(std::ostream* os, const std::string& sep) {
	*os << sep;
	if (std::string::npos != sep.find_first_of('\n')) {
		if (indent_) --indent_;
		FormattingIndent(os);
	}
}

void JsonStream::FormattingIndent(std::ostream* os) {
	for (uint32_t i = indent_; i; --i) *os << indent_sep_;
}

void JsonStream::FormattingComma(std::ostream* os) {
	*os << comma_sep_;
	for (uint32_t i = indent_; i; --i) *os << indent_sep_;
}

int JsonStream::Print(std::ostream* os, const Json& json) {
	PrintValue(os, json);
	return 0;
}

int JsonStream::Print(const Json& json) {
	PrintValue(&oss_, json);
	return 0;
}

void JsonStream::set_obj_sep(const std::string& sep) { obj_sep_ = sep; }

void JsonStream::set_array_sep(const std::string& sep) { array_sep_ = sep; }

void JsonStream::set_indent_sep(const std::string& sep) { indent_sep_ = sep; }

void JsonStream::set_comma_sep(const std::string& sep) { comma_sep_ = sep; }

void JsonStream::set_colon_sep(const std::string& sep) { colon_sep_ = sep; }

std::string JsonStream::str() const {
	return oss_.str();
}

void JsonStream::PrintValue(std::ostream* os, const Json& json) {
	if (json.IsNull()) {
		*os << "null";
	} else if (json.IsString()) {
		PrintString(os, json);
	} else if (json.IsInteger()) {
		*os << json.integer();
	} else if (json.IsUInteger()) {
		*os << json.uinteger();
	} else if (json.IsReal()) {
		*os << json.real();
	} else if (json.IsObject()) {
		PrintObject(os, json);
	} else if (json.IsArray()) {		
		PrintArray(os, json);
	} else if (json.IsBoolean()) {		
		*os << (json.boolean()?"true":"false");
	}
}

void JsonStream::PrintObject(std::ostream* os, const Json& json) {
	Json::Object::iterator begin(json.object().begin());
	Json::Object::iterator end(json.object().end());
	*os << "{";
	FormattingBegin(os, obj_sep_);
	for(Json::Object::iterator it(begin);it != end; ++it) {
		if (it != begin) {
			*os << ",";
			FormattingComma(os);
		}
		*os << "\"" << it->first << "\":" << colon_sep_;
		PrintValue(os, it->second);
	}
	FormattingEnd(os, obj_sep_);
	*os << "}";
}

void JsonStream::PrintArray(std::ostream* os, const Json& json) {
	*os << "[";
	FormattingBegin(os, array_sep_);
	for(size_t i=0; i < json.size(); ++i) {
		if (i != 0) {
			*os << ",";
			FormattingComma(os);
		}
		PrintValue(os, json[i]);
	}
	FormattingEnd(os, array_sep_);
	*os << "]";
}

void JsonStream::PrintString(std::ostream *os, const Json& json) {
	Json::String::const_iterator begin(json.string().begin());
	Json::String::const_iterator end(json.string().end());

	*os << "\"";

	for(;begin != end; ++begin) {
		if (!(0x80 & *begin)) { //0xxxxxxx
			switch(*begin) {
			case '"': *os << "\\\""; break;
			case '\\': *os << "\\\\"; break;
			case '\b': *os << "\\b"; break;
			case '\f': *os << "\\f"; break;
			case '\n': *os << "\\n"; break;
			case '\r': *os << "\\r"; break;
			case '\t': *os << "\\t"; break;
			default:
				if (0x1F < *begin) {
					*os << *begin;
				} else { //TODO error
					//return -1;
					return;
				}
				break;
			}
		} else {
			*os << *begin;
		}
	}

	*os << "\"";
}

JsonStream& operator << (JsonStream& printer, const Json& json) {
	printer.Print(json);
	return printer;
}

JsonStream& operator << (JsonStream& printer, JsonStream& (__cdecl *func)(JsonStream&)) {
	return func(printer);
}

std::ostream& operator << (std::ostream& os, const Json& value) {
	JsonStream printer;
	printer.Print(&os, value);
	return os;
}

JsonStream& operator << (JsonStream& printer, const JOpt& opt) {
	(*opt.func_)(printer, opt.val_);
	return printer;
}

JsonStream& default_sep(JsonStream& printer) {
	printer.set_indent_sep("    ");
	printer.set_obj_sep("\n");
	printer.set_array_sep("\n");
	printer.set_comma_sep("\n");
	printer.set_colon_sep(" ");
	return printer;
}

#define DEF_OPT(name) \
void name(JsonStream& printer, const std::string& val) {\
	printer.set_##name(val);\
}\
JOpt name(const std::string& val) {\
	return JOpt(name, val);\
}

DEF_OPT(obj_sep)
DEF_OPT(array_sep)
DEF_OPT(indent_sep)
DEF_OPT(comma_sep)

} //namespace jslite