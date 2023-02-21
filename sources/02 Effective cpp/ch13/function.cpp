#include <functional>

class GameCharacter;
int defaultHealthCalc(const GameCharacter& gc);
class GameCharacter {
public:
  typedef std::function<int (const GameCharacter&)> HealthCalcFunc;
  explicit GameCharacter(HealthCalcFunc hcf = defaultHealthCalc) : healthFunc(hcf) {}
  int healthValue() const {
    return healthFunc(*this);
  }
private:
  HealthCalcFunc healthFunc;
};

// 普通函数，但是返回值为 short，std::function 允许隐式转型
short calcHealth(const GameCharacter& gc); 

// 函数对象
struct HealthCalculator {
  int operator()(const GameCharacter& gc) const;
};

// 成员函数
class GameLevel {
public:
  float health(const GameCharacter& gc) const;
};

class EvilBadGuy : public GameCharacter {
public:
  EvilBadGuy(HealthCalcFunc hcf) : GameCharacter(hcf) {}
};

EvilBadGuy ebg1(calcHealth); // 封装普通函数
EvilBadGuy ebg2(HealthCalculator()); // 封装函数对象

GameLevel currentLevel;
EvilBadGuy ebg3(std::bind(
  &GameLevel::health, 
  currentLevel, 
  std::placeholders::_1
));