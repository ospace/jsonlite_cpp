#include "json_parser.hpp"
#include "json_util.hpp"

namespace jslite {

const size_t MAX_STACK_DEPTH = 100000;

class JsonTokenzier {
public:
	typedef enum {
		TK_NONE, TK_WRONG, TK_STRING, TK_INTEGER, TK_REAL, TK_TRUE, TK_FALSE, TK_NULL, TK_EOF,
		TK_COLON, TK_COMMA, TK_OBJ_BEGIN, TK_OBJ_END, TK_ARR_BEGIN, TK_ARR_END
	} TokenType;
	
	struct Token {
		Token() { clear(); }
		void clear() { type = TK_NONE; begin = NULL; end = NULL; }
		TokenType type;
		const char *begin;
		const char *end;
	};

	JsonTokenzier(const char *begin, const char *end) : begin_(begin), end_(end), it_(begin) {}

	char Current() { return (IsDone() ? 0 : *it_); }

	char Next() { return (IsDone() ? 0 : *++it_); }

	bool IsDone() { return it_ == end_; }

	const Token& CurrentToken() { return token_; }

	const Token& NextToken() {
		SkipSpace();
		token_.clear();
		token_.begin = it_;

		char c = 0;
		switch(Current()) {
		case '{': token_.type = TK_OBJ_BEGIN; Next();break;
		case '}': token_.type = TK_OBJ_END; Next();break;
		case '[': token_.type = TK_ARR_BEGIN; Next();break;
		case ']': token_.type = TK_ARR_END; Next();break;
		case ',': token_.type = TK_COMMA; Next();break;
		case ':': token_.type = TK_COLON; Next();break;
		case 't':
			token_.type = (Expact("true", 4)?TK_TRUE:TK_WRONG);
			break;
		case 'f':
			token_.type = (Expact("false", 5)?TK_FALSE:TK_WRONG);
			break;
		case 'n':
			token_.type = (Expact("null", 4)?TK_NULL:TK_WRONG);
			break;
		case '"':
			for (c = Next(); !(IsDone()||'"'==c); c = Next()) {
				if ('\\' == c) Next();
			}
			token_.type = ('"' == Current()?TK_STRING:TK_WRONG);
			Next();
			break;
		case '0': case '1': case '2': case '3': case '4': case '5': 
		case '6': case '7': case '8': case '9': case '-': case '+':
			token_.type = TK_INTEGER;

			while (c = Next()) {
				if (isdigit(c)) continue;
				if ('+' == c || '-' == c) continue;
				if ('.' == c || 'e' == c || 'E' == c) {
					token_.type = TK_REAL;
					continue;
				}
				break;
			}
			break;
		case 0: token_.type = TK_EOF; break;
		default: token_.type = TK_WRONG; break;
		}

		token_.end = it_;

		return token_;
	}

	void SkipSpace() {
		for (char c = Current(); !IsDone() && isspace(c); c = Next());
	}

	std::string str() {
		std::stringstream ss;
		ss << "at " << (token_.begin ? token_.begin - begin_ : 0);
		ss << ", \"" << std::string(token_.begin, token_.end - token_.begin) << "\"";
		if (token_.type == TK_WRONG) {
			ss << " is wrong token";
		}
		return ss.str();
	}

protected:
	bool Expact(const char *str, ptrdiff_t len) {
		if (end_ - it_ < len) return false;
		if (0 != strncmp(str, it_, len)) return false;
		it_ += len;
		return true;
	}

private:
	const char *begin_;
	const char *end_;
	const char *it_;
	Token token_;
};

int32_t ParseString(const JsonTokenzier::Token& token, std::string& str) {
	str.clear();

	const char *it = token.begin;

	if ('"' != *it) return -1;

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
				if (std::distance(it, token.end) < 4) return -2;
				if (!(isxdigit(*it) && isxdigit(*(it+1)) && isxdigit(*(it+2)) && isxdigit(*(it+3)))) return -3;
				str += UnicodeToUTF8(asc2hex(*it, *(it+1)), asc2hex(*(it+2), *(it+3)));
				it += 3;
				break;
			default: return -4;
			}
		} else {
			str += *it;
		}
	}

	if ('"' != *it) return -5;

	return 0;
}

int32_t ParseString(Json &json, const JsonTokenzier::Token& token) {
	std::string str;

	int32_t ret = ParseString(token, str);
	if (0 > ret) return ret;
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
		if (pt != (buf + len)) return -1;		
	} else {
		char *buf = (char*)malloc(len+1);
		memcpy(buf, token.begin, len);
		buf[len] = '\0';
		num = strtod(buf, &pt);
		free(buf);
		if (pt != (buf + len)) return -1;		
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
		if (str+len != pt) return 1;
		json = val;
		return 0;
	}

	if ('-' == *token.begin) {
		--len;
		if (19 > len) {
			memcpy(str, token.begin, len);
			jslite::Json::Integer val = strtoll(str, &pt, 10);
			if (str+len != pt) return 1;
			json  = val;
			return 0;
		}
	} else {
		if (20 > len) {
			memcpy(str, token.begin, len);
			jslite::Json::UInteger val = strtoull(str, &pt, 10);
			if (str+len != pt) return 1;
			json = val;
			return 0;
		}
	}

	return ParseDouble(json, token);
}


Json* JsonParser::Parse(const std::string& str) {
	Json *json = new Json();
	if (0 > Parse(*json, str)) {
		delete json;
		return NULL;
	}
	return json;
}

int32_t JsonParser::Parse(Json& json, const std::string& str) {
	tokenizer_ = new JsonTokenzier(str.c_str(), str.c_str() + str.size());
	if (NULL == tokenizer_) return -1;

	return ParseValue(json);
}

int32_t JsonParser::ParseValue(Json &json, size_t depth) {
	if (depth > MAX_STACK_DEPTH) return -99;

	int32_t ret = 0;

	const JsonTokenzier::Token token = tokenizer_->NextToken();

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
		return -2;
	}

	return ret;
}

int32_t JsonParser::ParseArray(Json &json, size_t depth) {
	if (depth > MAX_STACK_DEPTH) return -99;

	json.array();

	tokenizer_->SkipSpace();

	if (']' == tokenizer_->Current()) {
		tokenizer_->NextToken();
		return 0;
	}

	JsonTokenzier::Token token;

	int32_t ret = 0;
	do {
		Json value;
		json.put(value);
		ret = ParseValue(json[json.size()-1], depth+1);
		if (0 > ret) return ret;
		token = tokenizer_->NextToken();
	} while(JsonTokenzier::TK_COMMA == token.type);

	if (JsonTokenzier::TK_ARR_END != tokenizer_->CurrentToken().type) return -5;

	return 0;
}
	
int32_t JsonParser::ParseObject(Json &json, size_t depth) {
	if (depth > MAX_STACK_DEPTH) return -99;

	int32_t ret = 0;
	std::string key;

	json.object();

	JsonTokenzier::Token token;

	while(JsonTokenzier::TK_OBJ_END != (token = tokenizer_->NextToken()).type) {
		if (JsonTokenzier::TK_STRING != token.type) return -2;
		ret = jslite::ParseString(token, key);
		if (0 > ret) return -3;

		token = tokenizer_->NextToken();
		if (JsonTokenzier::TK_COLON != token.type) return -4;

		ret = ParseValue(json[key], depth+1); 
		if (0 > ret) return ret;

		token = tokenizer_->NextToken();
		if (JsonTokenzier::TK_COMMA != token.type) break;
	}

	if (JsonTokenzier::TK_OBJ_END != token.type) return -5;

	return 0;
}

std::string JsonParser::error_str(int32_t errno) const {
	//const JsonTokenzier::Token token = tokenizer_->CurrentToken();

	std::stringstream ss;
	//size_t pos = std::distance(begin_, it_);
	ss << "parsing failed " << tokenizer_->str();
	//ss << " >>> \"" << std::string(begin_ + pos - 1, std::min((end_-it_), 20)) << "\"";
	return ss.str();
}

} //namespace jsonlite