#include "jsonlite.hpp"
#include "json_util.hpp"
#include <sstream>

namespace jslite {

struct TypeInfoComp {
	bool operator() (std::type_info const *l, std::type_info const *r) {
		return l->before(*r);
	}
};

static Json::Type_T TypeOf(std::type_info const* type) {
	static std::map<std::type_info const*, Json::Type_T, TypeInfoComp> types;
	if (types.empty()) {
		types[&typeid(Json::String)] = Json::TYPE_STR;
		types[&typeid(Json::Integer)] = Json::TYPE_INT;
		types[&typeid(Json::UInteger)] = Json::TYPE_UINT;
		types[&typeid(Json::Real)] = Json::TYPE_REAL;
		types[&typeid(Json::Boolean)] = Json::TYPE_BOOL;
		types[&typeid(Json::Object)] = Json::TYPE_OBJ;
		types[&typeid(Json::Array)] = Json::TYPE_ARR;
	}

	std::map<std::type_info const*, Json::Type_T, TypeInfoComp>::iterator it(types.find(type));
	return ((types.end() == it)? Json::TYPE_NONE : it->second);
}

Json::Json() : value_(NULL) {}

Json::Json(const char* val) : value_(new Any<String>(val)) {}

Json::Json(const String& val) : value_(new Any<String>(val)) {}

Json::Json(Boolean val) : value_(new Any<Boolean>(val)) {}

Json::Json(UInteger val) : value_(new Any<UInteger>(val)) {}

Json::Json(Real val) : value_(new Any<Real>(val)) {}

Json::Json(Integer val) : value_(new Any<Integer>(val)) {}

Json::Json(const Json& val) : value_(val.value_?val.value_->clone():NULL) { }

Json::~Json() { clear(); }

Json& Json::Swap(Json& other) { std::swap(value_, other.value_); return *this; }

Json::Type_T Json::Type() const {
	if (NULL == value_) return Json::TYPE_NULL;
	return TypeOf(&value_->type());
}

bool Json::IsNull() const { return NULL == value_; }

bool Json::IsString() const { return value_ && (typeid(String) == value_->type()); }

bool Json::IsObject() const { return value_ && (typeid(Object) == value_->type()); }

bool Json::IsArray() const  { return value_ && (typeid(Array) == value_->type()); }

bool Json::IsBoolean() const { return value_ && (typeid(Boolean) == value_->type()); }

bool Json::IsInteger() const { return value_ && (typeid(Integer) == value_->type()); }

bool Json::IsUInteger() const { return value_ && (typeid(UInteger) == value_->type()); }

bool Json::IsReal() const { return value_ && (typeid(Real) == value_->type()); }

bool Json::IsNumber() const {
    if (IsInteger() || IsUInteger() || IsReal()) return true;
    return false;
}

void Json::remove() { clear(); }

void Json::clear() {
    if (value_) {
        Dummy *dummy = value_;
        value_ = NULL;
        delete dummy;
    }
}

void Json::remove_at(size_t idx) {
    if (!IsArray()) return;
    Array &arr = any_cast<Array>()->v_;
    if (idx > arr.size()) return;
    arr.erase(arr.begin()+idx);
}

void Json::remove_by(const String& key) {
    if (!IsObject()) return;
    Object &obj = any_cast<Object>()->v_;
    obj.erase(key);
}

const Json::String& Json::string() const { return any_cast<Json::String>()->v_; }

Json::WString Json::wstring() const { return ToUCS2(string()); }

#ifdef WIN32
Json::String Json::multibyte() const { return UnicodeToMultibyte(wstring()); }

//Json& Json::from_multibyte(const Json::String& str) { return operator = (MSConvertTo<WString>(str)); }
#endif

Json::Boolean Json::boolean() const { return any_cast<Boolean>()->v_; } 

Json::Array& Json::array() const { return any_cast<Array>()->v_; }

Json::Array& Json::array() {
    if (IsNull()) value_ = new Any<Array>();
    return any_cast<Array>()->v_;
}

Json::Object& Json::object() const { return any_cast<Object>()->v_; }

Json::Object& Json::object() {
    if (IsNull()) value_ = new Any<Object>();
    return any_cast<Object>()->v_;
}

Json::Integer Json::integer() const { return any_cast<Integer>()->v_; }

Json::UInteger Json::uinteger() const { return any_cast<UInteger>()->v_; }

Json::Real Json::real() const { return any_cast<Real>()->v_; }

Json& Json::operator = (const Json& val) {
    Json(val).Swap(*this);
    return *this;
}

Json& Json::operator = (const String& val) {
    //if (!unicodelite::ValidUTF8(val)) return *this; //TODO exception

    if (IsNull()) {
        Json(val).Swap(*this);
    } else {
        any_cast<String>()->v_ = val;
    }
    return *this;
}

Json& Json::operator = (const WString& val) {
    std::string target;

    for(size_t i = 0; i < val.size(); ++i) {
        target += UnicodeToUTF8(val[i]);
    }

    return operator = (target);
}

Json& Json::operator = (const char* val) { return operator = (String(val)); }

Json& Json::operator = (Json::Boolean val) {
    if (IsNull()) {
        Json(val).Swap(*this);
    } else {
        any_cast<Boolean>()->v_ = val;
    }
    return *this;
}

Json& Json::operator = (Integer val) {
    if (IsNull()) {
        Json(val).Swap(*this);
    } else {
        any_cast<Integer>()->v_ = val;
    }
    return *this;
}

Json& Json::operator = (UInteger val) {
    if (IsNull()) {
        Json(val).Swap(*this);
    } else {
        any_cast<UInteger>()->v_ = val;
    }
    return *this;
}

Json& Json::operator = (Real val) {
    if (IsNull()) {
        Json(val).Swap(*this);
    } else {
        any_cast<Real>()->v_ = val;
    }
    return *this;
}

Json& Json::operator = (int val) {
    return operator = ((Integer) val);
}

bool Json::operator == (const Json& other) const {
    if (this == &other) return true;
    if (IsNull()) return other.IsNull();
    if (other.IsNull()) return false;
    if (value_->type() != other.value_->type()) return false;
    if (IsInteger()) return integer() == other.integer();
    if (IsUInteger()) return uinteger() == other.uinteger();
    if (IsReal()) return real() == other.real();
    if (IsString()) return string() == other.string();
    if (IsArray())  return array() == other.array();
    if (IsObject()) return object() == other.object();
    return false;
}

Json& Json::operator [] (const std::string& key) {
    if (IsNull()) value_ = new Any<Object>();
    return object()[key];
}

Json& Json::operator [] (const std::string& key) const {
    return object()[key];
}

Json& Json::operator [] (unsigned short idx) {
    Array &arr = array();
    if (idx >= arr.size()) throw std::range_error("out of range array");
    return arr[idx];
}

Json& Json::operator [] (unsigned short idx) const {
    Array &arr = array();
    if (idx >= arr.size()) throw std::range_error("out of range array");
    return arr[idx];
}

Json& Json::put(const Json& val) {
    if (IsNull()) value_ = new Any<Array>();
    array().push_back(val);
    return *this;
}

size_t Json::size() const {
    if (IsNull()) return 0;
    if (IsObject()) return object().size();
    if (IsArray()) return array().size();
    if (IsString()) return string().size();
    return 1;
}

size_t Json::length() const { return size(); } //similar to javascript
    
//for debuging
std::string Json::str() const {
    std::ostringstream oss;
    oss << "{ \"value\":\"" << value_ << "\"";
    oss << ", \"typeid\":\"";
    if (value_) {
        oss << value_->type().name();
    }  else {
        oss << typeid(value_).name();
    }
    oss << "\" }";
    return oss.str();
}

} // namespace jslite