#include <cstring>
#include <iostream>


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

char& String::operator[](int index) {
  if (value->refCount > 1) {
    --value->refCount;
    value = new StringValue(value->data);
  }
  value->shareable = false;
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