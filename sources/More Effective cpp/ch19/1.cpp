#include <iostream>
#include <memory>

using namespace std;

class ALA {
public:
  virtual void processAdoption() = 0;
};

class Puppy : public ALA {
public:
  virtual void processAdoption();
};

class Kitten : public ALA {
public:
  virtual void processAdoption();
};

ALA *readALA(istream &s) {
  return nullptr;
}

void processAdoptions(istream &dataSource) {
  while (dataSource) {
    // ALA *pa = readALA(dataSource);
    // pa->processAdoption();
    // delete pa;
    auto_ptr<ALA> pa(readALA(dataSource));
    pa->processAdoption();
  }
}