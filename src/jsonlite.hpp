//////////////////////////////////////////////////////////////////
// JSON generator using STL
// Copyright (c) 2012 Park Jae Seong(ospace114@naver.com)
// Version 0.4
// RFC 4627 JSON


// JSON String
// prefix backslash-u unicode escape
/*
\u000A - line feed
\u000D - Carriage Return
\u2028 - Line separator U+2028
\u2029 - Paragraph separator U+2029

*/

/* TODO
   ( ) sort
   ( ) seperate type of number: Integer, Double
   ( ) detailed parsing error
*/

#ifndef __JSON_LITE_HPP_20121127__
#define __JSON_LITE_HPP_20121127__

#include <stdint.h>
#include <sstream>
#include <typeinfo>
#include <algorithm>
#include <stdexcept>
#include <functional>
#include <map>
#include <deque>
#ifdef WIN32
#include <memory>
#else
#include <tr1/memory>
#endif

namespace jslite {


class Json {
public:
	typedef Json                        Value;
	typedef std::string                 String;
	typedef std::wstring                WString;  // UCS2
	typedef int64_t                     Integer;
	typedef uint64_t                    UInteger;
	typedef double                      Real;
	typedef bool                        Boolean;
	typedef std::map<std::string, Json> Object;
	typedef std::deque<Json>            Array;

    Json();
	Json(const Json& val);
    Json(const char* val);
    Json(const String& val);
    Json(Boolean val);
	Json(UInteger val);
	Json(Real val);
	Json(Integer val);

    ~Json();

	Json& Swap(Json& other);

    bool IsNull() const;
    bool IsString() const;
    bool IsObject() const;
    bool IsArray() const;
    bool IsBoolean() const;
	bool IsInteger() const;
	bool IsUInteger() const;
	bool IsReal() const;
	bool IsNumber() const;

    void remove();
    void clear();
    void remove_at(size_t idx);
    void remove_by(const String& key);

    const String& string() const;
	WString wstring() const;

#ifdef WIN32
	String multibyte() const;
	//Json& from_multibyte(const String& str);
#endif

    Boolean boolean() const; 
	Array& array() const;
	Array& array();
	Object& object() const;
	Object& object();
	Integer integer() const;
	UInteger uinteger() const;
	Real real() const;

	Json& operator = (const Json& val);
    Json& operator = (const String& val);
	Json& operator = (const WString& val);
    Json& operator = (const char* val);
    Json& operator = (Boolean val);
	Json& operator = (Integer val);
	Json& operator = (UInteger val);
	Json& operator = (Real val);
	Json& operator = (int val);

	bool operator == (const Json& other) const;
    Json& operator [] (const std::string& key);
    Json& operator [] (const std::string& key) const;
    Json& operator [] (unsigned short idx);
    Json& operator [] (unsigned short idx) const;
    Json& put(const Json& val);
    size_t size() const;
    size_t length() const; //similar to javascript
    
    template <class Function>
    Function for_keys(Function f) const {
        const Object &obj = object();
		for(Object::const_iterator it=obj.begin(); it!=obj.end(); ++it) f(it->first, it->second);
        return f;
    }

    template <class Function>
    Function for_each(Function f) const {
        Array &arr = array();
        return std::for_each(arr.begin(), arr.end(), f);
    }

    //for debuging
    std::string ToString() const;

protected:
	struct Dummy {
        virtual ~Dummy() {}
        virtual const std::type_info& type() const = 0;
		virtual Dummy* clone() const = 0;
    };

    template <typename V>
    struct Any : Dummy {
        Any(const V& v) : v_(v) {}
        Any() {}
        const std::type_info& type() const { return typeid(V); }
		Dummy* clone() const { return new Any<V>(v_); }
        V    v_;
    };

    template <typename V>
    Any<V>* any_cast() const {
		if (NULL == value_) throw std::logic_error("null object");
        if (typeid(V) == value_->type()) return static_cast<Any<V>*>(value_);
		std::stringstream ss;
		ss << "type mismatch: expected " << typeid(V).name() << ", but " << value_->type().name();
        throw std::logic_error(ss.str());
    }

private:
    Dummy *value_;
};

} // namespace jslite

#endif //__JSON_LITE_HPP_20121127__