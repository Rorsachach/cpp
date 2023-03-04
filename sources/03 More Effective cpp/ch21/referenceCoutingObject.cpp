#include <cstring>
#include <iostream>


class RCObject {
public:
  RCObject() : refCount(0), shareable(true) {}
  RCObject(const RCObject& rhs) : refCount(0), shareable(true) {}
  RCObject& operator=(const RCObject& rhs) { return *this; }
  virtual ~RCObject() {}

  void addReference() { ++refCount; }
  void removeReference() { if (--refCount == 0) delete this; }
  void markUnshareable() { shareable = false; }
  bool isShareable() const { return shareable; }
  bool isShared() const { return refCount > 1; }
private:
  int refCount;
  bool shareable;
};


class String {
public:
  String(const char* initValue = "") : value(new StringValue(initValue)) {}
  ~String();
  String(const String& rhs);
  String& operator=(const String& rhs);
  const char& operator[](int index) const { return value->data[index]; }
  char& operator[](int index);
  friend std::ostream& operator<<(std::ostream& os,const String& s);
private:
  struct StringValue : public RCObject {
    char* data;

    StringValue(const char* initValue);
    ~StringValue();
  };

  StringValue* value;
};

String::StringValue::StringValue(const char* initValue) {
  data = new char[std::strlen(initValue) + 1];
  std::strcpy(data, initValue);
}

String::StringValue::~StringValue() {
  delete[] data;
}

String::~String() {
  value->removeReference();
}

String::String(const String& rhs) {
  if (rhs.value->isShareable()) {
    value = rhs.value;
    value->addReference();
  } else {
    value = new StringValue(rhs.value->data);
  }
}

String& String::operator=(const String& rhs) {
  if (this == &rhs) return *this;
  value->removeReference();
  
  if (rhs.value->isShareable()) {
    value = rhs.value;
    value->addReference();
  } else {
    value = new StringValue(rhs.value->data);
  }

  return *this;
}

char& String::operator[](int index) {
  if (value->isShared()) {
    value->removeReference();
    value = new StringValue(value->data);
  }
  value->markUnshareable();
  return value->data[index];
}

std::ostream& operator<<(std::ostream& os,const String& s) {
  os << s.value->data;
  return os;
}

int main() {
  String s1("hello");
  char* p = &s1[1];
  String s2(s1);
  *p = 'x';
  std::cout << s2 <<std::endl;
}