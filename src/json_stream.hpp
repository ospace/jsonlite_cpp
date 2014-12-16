#ifndef __JS_JSON_STREAM_HPP_20140928__
#define __JS_JSON_STREAM_HPP_20140928__

#include <string>
#include <sstream>

#include "jsonlite.hpp"

namespace jslite {

typedef enum {
	SUCCESS = 0,
	ERR_NO_MEMORY, //error
	ERR_QUOTES, //double quotes are needed for string
	ERR_ESC_CHAR, // unknown a escape character of string
	ERR_UNICODE, // wrong unicode string
	ERR_JSON_TYPE, // not supported a type of json data
	ERR_UTF8, // wrong utf8 string
	ERR_VALUE, // invalid json value
	ERR_OVERFLOW, // overflow a depth of json data
	ERR_ARRAY_END, // a end of array("]") is required
	ERR_OBJECT_KEY, // a type of key at object is required a string
	ERR_OBJECT_SEP, // colon for seperation of oject is requried
	ERR_OBJECT_END, // a end of object("}") is required
	ERR_NUMBER, // wrong number string
} ErrnoNo;

class JsonTokenzier;

class JsonStream {
public:
    JsonStream();
    ~JsonStream();

    //in operation
    int32_t Print(const Json& json);

    void set_obj_sep(const std::string& sep);
    void set_array_sep(const std::string& sep);
    void set_indent_sep(const std::string& sep);
    void set_comma_sep(const std::string& sep);
    void set_colon_sep(const std::string& sep);

    //out operating
    int32_t Parse(Json& json);

    //others
    std::string str() const;
    void str(const std::string& s);
    JsonStream& operator << (const std::string& s);
    JsonStream& operator << (const char *s);

	std::string strerror(int32_t err);

protected:
    //in operation
    int32_t PrintValue(const Json& json);
    int32_t PrintObject(const Json& json);
    int32_t PrintArray(const Json& json);
    int32_t PrintString(const Json& json);

    void FormattingBegin(const std::string& sep);
    void FormattingIndent();
    void FormattingComma();
    void FormattingEnd(const std::string& sep);

    //out operating
    int32_t ParseValue(Json &json, size_t depth = 0);
    int32_t ParseArray(Json &json, size_t depth);
    int32_t ParseObject(Json &json, size_t depth);
    void TrimSpace();

private:
    JsonTokenzier *tokenizer_;
    
	uint32_t    indent_;
    std::string obj_sep_;
    std::string array_sep_;
    std::string indent_sep_;
    std::string comma_sep_;
    std::string colon_sep_;

    std::string str_;
	std::ostringstream oss_;
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
JsonStream& default_sep(JsonStream& jstm);


//////////////////////////////////////////////////////////////
// addtional operators
JsonStream& operator << (JsonStream& jstm, const Json& json);
JsonStream& operator << (JsonStream& jstm, JsonStream& (__cdecl *func)(JsonStream&));
JsonStream& operator << (JsonStream& jstm, const JOpt& opt);
JsonStream& operator >> (JsonStream& jstm, Json& json);

std::ostream& operator << (std::ostream& os, const Json& json);

} //namespace jslite

#endif //__JS_JSON_STREAM_HPP_20140928__