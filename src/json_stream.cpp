#include "json_stream.hpp"
#include "json_util.hpp"
#include "json_tokenizer.hpp"
#include <ostream>

namespace jslite {

const size_t MAX_STACK_DEPTH = 100000;

static const char* jslite_strerror(int32_t err) {
	static const struct {
		int32_t err;
		const char *str;
	} str_errors[] = {
		{SUCCESS, "Success"},
		{ERR_QUOTES, "Double quotes are needed for string"},
		{ERR_ESC_CHAR, "Invalid a escape character of string"},
		{ERR_UNICODE_CHAR, "Invalid unicode string"},
		{ERR_UNICODE_LEN, "Less unicode string"},
		{ERR_JSON_TYPE, "Not supported a json type"},
		{ERR_UTF8, "Invalid utf8 string"},
		{ERR_VALUE, "Invalid json value"},
		{ERR_OVERFLOW, "Overflow a depth of json data"},
		{ERR_ARRAY_END, "End of array(\"]\") is required"},
		{ERR_OBJECT_KEY, "Need a string to an object key"},
		{ERR_OBJECT_SEP, "colon seperator is requried to object"},
		{ERR_OBJECT_END, "End of object (\"}\") is required"},
		{ERR_NUMBER, "Invalid number string"},
		{0, NULL}
	};

	for (int32_t i=0; str_errors[i].str; ++i) {
		if (str_errors[i].err == err) return str_errors[i].str;
	}

	return "Unknown error";
}

int32_t ParseString(const JsonTokenzier::Token& token, std::string& str) {
    str.clear();

    const char *it = token.begin;

    if ('"' != *it) return ERR_QUOTES;

    str.reserve(token.end - token.begin + 2);
    ++it;
    for(; it != token.end && *it != '"'; ++it) {
        if ('\\' == *it) {
            switch(*++it) {
            case '"': case '\\': case '/': str += *it; break;
            case 'b': str += '\b'; break;
            case 'f': str += '\f'; break;
            case 'n': str += '\n'; break;
            case 'r': str += '\r'; break;
            case 't': str += '\t'; break;
            case 'u': //u: 2chars(json only), U:4chars
                ++it;
                if (std::distance(it, token.end) < 4) return ERR_UNICODE_LEN;
                if (!(isxdigit(*it) && isxdigit(*(it+1)) && isxdigit(*(it+2)) && isxdigit(*(it+3)))) return ERR_UNICODE_CHAR;
                str += UnicodeToUTF8(asc2hex(*it, *(it+1)), asc2hex(*(it+2), *(it+3)));
                it += 3;
                break;
            default: return ERR_ESC_CHAR;
            }
        } else {
            str += *it;
        }
    }

    if ('"' != *it) return ERR_QUOTES;

    return 0;
}

int32_t ParseString(Json &json, const JsonTokenzier::Token& token) {
    std::string str;

    int32_t ret = ParseString(token, str);
    if (0 != ret) return ret;
    json = str;
    return 0;
}

int32_t ParseDouble(Json &json, const JsonTokenzier::Token& token) {
    ptrdiff_t len = token.end - token.begin;

    jslite::Json::Real num = 0.0;
    char *pt = NULL;

    if (32 > len) {
        char buf[32];
        memcpy(buf, token.begin, len);
        buf[len] = '\0';
        num = strtod(buf, &pt);
        if (pt != (buf + len)) return ERR_NUMBER;
    } else {
        char *buf = (char*)malloc(len+1);
        memcpy(buf, token.begin, len);
        buf[len] = '\0';
        num = strtod(buf, &pt);
        free(buf);
        if (pt != (buf + len)) return ERR_NUMBER;
    }

    json = num;

    return 0;
}

int32_t ParseInteger(Json &json, const JsonTokenzier::Token& token) {

    if (token.begin == token.end) {
        json = 0;
        return 0;
    }

    ptrdiff_t len = token.end - token.begin;

    char str[20] = {0,};
    char *pt = NULL;

    if (10 > len) {
        memcpy(str, token.begin, len);
        int32_t val = strtol(str, &pt, 10);
        if (str+len != pt) return ERR_NUMBER;
        json = val;
        return 0;
    }

    if ('-' == *token.begin) {
        --len;
        if (19 > len) {
            memcpy(str, token.begin, len);
            jslite::Json::Integer val = strtoll(str, &pt, 10);
            if (str+len != pt) return ERR_NUMBER;
            json  = val;
            return 0;
        }
    } else {
        if (20 > len) {
            memcpy(str, token.begin, len);
            jslite::Json::UInteger val = strtoull(str, &pt, 10);
            if (str+len != pt) return ERR_NUMBER;
            json = val;
            return 0;
        }
    }

    return ParseDouble(json, token);
}


////////////////////////////////////////////////////////////////////////////////////
//public methods

JsonStream::JsonStream() : indent_(0), tokenizer_(NULL) { }

JsonStream::~JsonStream() { }

void JsonStream::FormattingBegin(const std::string& sep) {
    ss_ << sep;
    if (std::string::npos  != sep.find_first_of('\n')) {
        ++indent_;
        FormattingIndent();
    }
}

void JsonStream::FormattingEnd(const std::string& sep) {
    ss_ << sep;
    if (std::string::npos != sep.find_first_of('\n')) {
        if (indent_) --indent_;
        FormattingIndent();
    }
}

void JsonStream::FormattingIndent() {
    for (uint32_t i = indent_; i; --i) ss_ << indent_sep_;
}

void JsonStream::FormattingComma() {
    ss_ << comma_sep_;
    for (uint32_t i = indent_; i; --i) ss_ << indent_sep_;
}

int32_t JsonStream::Print(const Json& json) {
    PrintValue(json);
    return 0;
}

void JsonStream::set_obj_sep(const std::string& sep) { obj_sep_ = sep; }

void JsonStream::set_array_sep(const std::string& sep) { array_sep_ = sep; }

void JsonStream::set_indent_sep(const std::string& sep) { indent_sep_ = sep; }

void JsonStream::set_comma_sep(const std::string& sep) { comma_sep_ = sep; }

void JsonStream::set_colon_sep(const std::string& sep) { colon_sep_ = sep; }

int32_t JsonStream::Parse(Json& json) {
    std::string str(ss_.str());
    tokenizer_ = new JsonTokenzier(str.c_str(), str.c_str() + str.size());
    if (NULL == tokenizer_) return ERR_NO_MEMORY;

    return ParseValue(json);
}

////////////////////////////////////////////////////////////////////////////////////
//protected methods

std::string JsonStream::str() const { return ss_.str(); }

void JsonStream::str(const std::string& s) { ss_.str(s); }

JsonStream& JsonStream::operator << (const char *s) {
    ss_ << s;
    return  *this;
}

JsonStream& JsonStream::operator << (const std::string& s) {
    ss_ << s;
    return *this;
}

std::string JsonStream::strerror(int32_t err) {
	std::ostringstream oss;
	oss << jslite_strerror(err) << std::endl;
	if (tokenizer_) oss << tokenizer_->str();
	return  oss.str();
}

int32_t JsonStream::PrintValue(const Json& json) {
    int32_t ret = 0;
    if (json.IsNull()) {
        ss_ << "null";
    } else if (json.IsString()) {
        ret = PrintString(json);
    } else if (json.IsInteger()) {
        ss_ << json.integer();
    } else if (json.IsUInteger()) {
        ss_ << json.uinteger();
    } else if (json.IsReal()) {
        ss_ << json.real();
    } else if (json.IsObject()) {
        ret = PrintObject(json);
    } else if (json.IsArray()) {        
        ret = PrintArray(json);
    } else if (json.IsBoolean()) {        
        ss_ << (json.boolean()?"true":"false");
    } else {
        return ERR_JSON_TYPE;
    }

    return ret;
}

int32_t JsonStream::PrintObject(const Json& json) {
    Json::Object::iterator begin(json.object().begin());
    Json::Object::iterator end(json.object().end());
    ss_ << "{";
    FormattingBegin(obj_sep_);
    int32_t ret = 0;
    for(Json::Object::iterator it(begin);it != end; ++it) {
        if (it != begin) {
            ss_ << ",";
            FormattingComma();
        }
        ss_ << "\"" << it->first << "\":" << colon_sep_;
        if (0 != (ret = PrintValue(it->second))) return ret;
    }
    FormattingEnd(obj_sep_);
    ss_ << "}";

    return 0;
}

int32_t JsonStream::PrintArray(const Json& json) {
    ss_ << "[";
    FormattingBegin(array_sep_);
    int32_t ret = 0;
    for(size_t i=0; i < json.size(); ++i) {
        if (i != 0) {
            ss_ << ",";
            FormattingComma();
        }
        if (0 != (ret = PrintValue(json[i]))) return ret;
    }
    FormattingEnd(array_sep_);
    ss_ << "]";

    return 0;
}

int32_t JsonStream::PrintString(const Json& json) {
    Json::String::const_iterator it(json.string().begin());
    Json::String::const_iterator end(json.string().end());

    ss_ << "\"";

    for(;it != end; ++it) {
        if (!(0x80 & *it)) { //0xxxxxxx
            switch(*it) {
            case '"': ss_ << "\\\""; break;
            case '\\': ss_ << "\\\\"; break;
            case '\b': ss_ << "\\b"; break;
            case '\f': ss_ << "\\f"; break;
            case '\n': ss_ << "\\n"; break;
            case '\r': ss_ << "\\r"; break;
            case '\t': ss_ << "\\t"; break;
            default:
                if (0x1F < *it) {
                    ss_ << *it;
                } else { //TODO error
                    return ERR_UTF8;
                }
                break;
            }
        } else {
            ss_ << *it;
        }
    }

    ss_ << "\"";

    return 0;
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
void name(JsonStream& jstm, const std::string& val) {\
    jstm.set_##name(val);\
}\
JOpt name(const std::string& val) {\
    return JOpt(name, val);\
}

DEF_OPT(obj_sep)
DEF_OPT(array_sep)
DEF_OPT(indent_sep)
DEF_OPT(comma_sep)

int32_t JsonStream::ParseValue(Json &json, size_t depth) {
    if (depth > MAX_STACK_DEPTH) return ERR_OVERFLOW;

    int32_t ret = 0;

    const JsonTokenzier::Token token = tokenizer_->SkipCommentAndNextToken();

    switch(token.type) {
    case JsonTokenzier::TK_ARR_BEGIN:
        ret = ParseArray(json, depth+1);
        break;
    case JsonTokenzier::TK_OBJ_BEGIN:
        ret = ParseObject(json, depth+1);
        break;
    case JsonTokenzier::TK_STRING:
        ret = jslite::ParseString(json, token);
        break;
    case JsonTokenzier::TK_INTEGER:
        ret = jslite::ParseInteger(json, token);
        break;
    case JsonTokenzier::TK_REAL:
        ret = jslite::ParseDouble(json, token);
        break;
    case JsonTokenzier::TK_TRUE:
        json = true;
        break;
    case JsonTokenzier::TK_FALSE:
        json = false;
        break;
    case JsonTokenzier::TK_NULL:
        break;
    default:
        return ERR_VALUE;
    }

    return ret;
}

int32_t JsonStream::ParseArray(Json &json, size_t depth) {
    if (depth > MAX_STACK_DEPTH) return ERR_OVERFLOW;

    json.array();

    tokenizer_->SkipSpace();

    if (']' == tokenizer_->Current()) {
        tokenizer_->SkipCommentAndNextToken();
        return 0;
    }

    JsonTokenzier::Token token;

    int32_t ret = 0;
    do {
        Json value;
        json.put(value);
        ret = ParseValue(json[json.size()-1], depth+1);
        if (0 != ret) return ret;
        token = tokenizer_->SkipCommentAndNextToken();
    } while(JsonTokenzier::TK_COMMA == token.type);

    if (JsonTokenzier::TK_ARR_END != tokenizer_->CurrentToken().type) return ERR_ARRAY_END;

    return 0;
}
    
int32_t JsonStream::ParseObject(Json &json, size_t depth) {
    if (depth > MAX_STACK_DEPTH) return ERR_OVERFLOW;

    json.object();

    JsonTokenzier::Token token;
    std::string key;
    int32_t ret = 0;

    while(JsonTokenzier::TK_OBJ_END != (token = tokenizer_->SkipCommentAndNextToken()).type) {
        if (JsonTokenzier::TK_STRING != token.type) return ERR_OBJECT_KEY;
        ret = jslite::ParseString(token, key);
        if (0 != ret) return ret;

        token = tokenizer_->SkipCommentAndNextToken();
        if (JsonTokenzier::TK_COLON != token.type) return ERR_OBJECT_SEP;

        ret = ParseValue(json[key], depth+1); 
        if (0 != ret) return ret;

        token = tokenizer_->SkipCommentAndNextToken();
        if (JsonTokenzier::TK_COMMA != token.type) break;
    }

    if (JsonTokenzier::TK_OBJ_END != token.type) return ERR_OBJECT_END;

    return 0;
}

JsonStream& operator << (JsonStream& jstm, const Json& json) {
    int32_t ret = jstm.Print(json);
    return jstm;
}


JsonStream& operator << (JsonStream& jstm, JsonStream& (__cdecl *func)(JsonStream&)) {
    return func(jstm);
}

JsonStream& operator << (JsonStream& jstm, const JOpt& opt) {
    (*opt.func_)(jstm, opt.val_);
    return jstm;
}

std::ostream& operator << (std::ostream& os, const Json& json) {
    JsonStream jstm;
    int32_t ret = jstm.Print(json);
    os << jstm.str();
    return os;
}

JsonStream& operator >> (JsonStream& jstm, Json& json) {
    int32_t ret = jstm.Parse(json);
    return jstm;
}

} //namespace jslite