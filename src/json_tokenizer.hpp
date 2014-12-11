#ifndef __JS_JSON_TOKENIZER_HPP_20130410__
#define __JS_JSON_TOKENIZER_HPP_20130410__

namespace jslite {

class JsonTokenzier {
public:
    typedef enum {
        TK_NONE, TK_WRONG, TK_STRING, TK_INTEGER, TK_REAL, TK_TRUE, TK_FALSE, TK_NULL,
        TK_COLON, TK_COMMA, TK_OBJ_BEGIN, TK_OBJ_END, TK_ARR_BEGIN, TK_ARR_END, 
        TK_COMMENT_C, TK_COMMENT_CPP, TK_EOF
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

    char Bump() { return (IsDone() ? 0 : *it_++); }

    bool IsDone() { return it_ == end_; }

    const Token& CurrentToken() { return token_; }

    const Token& SkipCommentAndNextToken() {
        do {
            NextToken();
        } while(JsonTokenzier::TK_COMMENT_CPP == token_.type || JsonTokenzier::TK_COMMENT_C == token_.type);
        return token_;
    }

    const Token& NextToken() {
        SkipSpace();
        token_.clear();
        token_.begin = it_;

        char c = 0;
        switch(c = Bump()) {
        case '{': token_.type = TK_OBJ_BEGIN; break;
        case '}': token_.type = TK_OBJ_END; break;
        case '[': token_.type = TK_ARR_BEGIN; break;
        case ']': token_.type = TK_ARR_END; break;
        case ',': token_.type = TK_COMMA; break;
        case ':': token_.type = TK_COLON; break;
        case 't':
            token_.type = (Expact("rue", 3)?TK_TRUE:TK_WRONG);
            break;
        case 'f':
            token_.type = (Expact("alse", 4)?TK_FALSE:TK_WRONG);
            break;
        case 'n':
            token_.type = (Expact("ull", 3)?TK_NULL:TK_WRONG);
            break;
        case '"':
            for (c = Bump(); c && '"' != c; c = Bump()) {
                if ('\\' == c) Bump();
            }
            token_.type = ('"' == c ? TK_STRING : TK_WRONG);
            break;
        case '0': case '1': case '2': case '3': case '4': case '5': 
        case '6': case '7': case '8': case '9': case '-': case '+':
            token_.type = TK_INTEGER;

            do {
                if (isdigit(c)) continue;
                if ('+' == c || '-' == c) continue;
                if ('.' == c || 'e' == c || 'E' == c) {
                    token_.type = TK_REAL;
                    continue;
                }
                break;
            } while (c = Bump());
            break;
        case '/':
            c = Bump();
            if ('/' == c) {
                token_.type = TK_COMMENT_CPP;
                while(c = Bump()) {
                    if ('\r' == c || '\n' == c) break;
                }
            } else if('*' == c) {
                token_.type = TK_COMMENT_C;
                while(c = Bump()) {
                    if ('*' == c && '/' == Current()) {
                        Bump();
                        break;
                    }
                }
            } else {
                token_.type = TK_WRONG;
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
        size_t line_num = 0, col_num = 0;
        {
            const char *line = begin_;
            for (const char *it = begin_; it != token_.begin; ++it) {
                if ('\n' == *it) {
                    ++line_num;
                    line = it+1;
                }
            }
            col_num = token_.begin - line;
        }

        std::ostringstream oss;
        if (token_.type == TK_WRONG) {
            oss << "Wrong keyword: " << std::string(token_.begin, token_.end - token_.begin) << " is undefined";
        } else {
            oss << "Keyword: " << std::string(token_.begin, token_.end - token_.begin);
        }

        oss << " at Line: " << line_num << ", Column: " << col_num;

        return oss.str();
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

} // namespace jslite

#endif //__JS_JSON_TOKENIZER_HPP_20130410__