#include <cstring>
#include <iostream>


class String {
public:
  String(const char* initValue = "") : value(new StringValue(initValue)) {}
  ~String();
  String(const String& rhs);
  String& operator=(const String& rhs);

  class CharProxy {
  public:
    CharProxy(String& str, int index) : theString(str), charIndex(index) {}
    CharProxy& operator=(const CharProxy& rhs);
    CharProxy& operator=(char c);
    operator char() const { return theString.value->data[charIndex]; }
  private:
    String& theString;
    int charIndex;
  };

  const CharProxy operator[](int index) const { 
    return CharProxy(const_cast<String&>(*this), index);
  }
  CharProxy operator[](int index) {
    return CharProxy(*this, index);
  }


  friend std::ostream& operator<<(std::ostream& os,const String& s);
  friend class CharProxy;
private:
  struct StringValue {
    int refCount;
    bool shareable;
    char* data;

    StringValue(const char* initValue);
    ~StringValue();
  };

  StringValue* value;
};

String::StringValue::StringValue(const char* initValue) : refCount(1), shareable(true) {
  data = new char[std::strlen(initValue) + 1];
  std::strcpy(data, initValue);
}

String::StringValue::~StringValue() {
  delete[] data;
}

String::~String() {
  if (--value->refCount == 0) delete value;
}

String::String(const String& rhs) {
  if (rhs.value->shareable) {
    value = rhs.value;
    ++value->refCount;
  } else {
    value = new StringValue(rhs.value->data);
  }
}

String& String::operator=(const String& rhs) {
  if (this == &rhs) return *this;
  if (--value->refCount == 0) delete value;
  
  if (rhs.value->shareable) {
    value = rhs.value;
    ++value->refCount;
  } else {
    value = new StringValue(rhs.value->data);
  }

  return *this;
}

String::CharProxy& String::CharProxy::operator=(const String::CharProxy& rhs) {
  if (theString.value->shareable) { // 写时拷贝
    theString.value = new StringValue(theString.value->data);
  }

  theString.value->data[charIndex] = rhs.theString.value->data[rhs.charIndex];

  return *this;
}

String::CharProxy& String::CharProxy::operator=(char c) {
  if (theString.value->shareable) { // 写时拷贝
    theString.value = new StringValue(theString.value->data);
  }

  theString.value->data[charIndex] = c;

  return *this;
}

std::ostream& operator<<(std::ostream& os,const String& s) {
  os << s.value->data;
  return os;
}

int main() {
  String s1("hello");
  char* p = s1[1];
  String s2(s1);
  *p = 'x';
  std::cout << s2 <<std::endl;
}