#include <string>

using namespace std;

typedef long long ObjectID;

class LargeObject {
public:
  LargeObject(ObjectID id);
  const string& field1() const;
  int field2() const;
  double field3() const;
  const string& field4() const;

private:
  ObjectID oid;
  mutable string *field1Value;
  mutable int *field2Value;
  mutable double *field3Value;
  mutable string *field4Value;
};

LargeObject::LargeObject(ObjectID id) : oid(id), field1Value(0),
  field2Value(0), field3Value(0), field4Value(0) {} // 在调用构造函数时，并不读取内容

int LargeObject::field2() const {
  if (field2Value == 0) {
    *field2Value = 1; // 这里是从数据库单独读取 field2 内容并赋值 
  }

  return *field2Value;
}