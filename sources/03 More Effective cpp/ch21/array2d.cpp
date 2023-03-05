#include <iostream>

template <class T>
class Array2D {
public:
  Array2D(int dim1, int dim2) {
    array = (Array1D**) operator new(sizeof(Array1D*) * dim1);
    for (int i = 0; i < dim1; ++i) {
      array[i] = new Array1D(dim2);
    }
  }
  ~Array2D() { operator delete(array); }
  class Array1D {
  public:
    Array1D(int dim) : array(new T[dim]) {}
    ~Array1D() { delete[] array; }

    T& operator[](int index) { return *(array + index); }
    const T& operator[] (int index) const {return *(array + index); }
  private:
    T* array;
  };

  Array1D& operator[](int index) { return **(array + index); }
  const Array1D& operator[](int index) const { return **(array + index); }

private:
  Array1D** array;
};

int main() {
  Array2D<int> data(10, 20);
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 20; ++j) {
      data[i][j] = i * 20 + j;
    }
  }

  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 20; ++j) {
      std::cout << data[i][j] << " ";
    }
    std::cout << std::endl;
  }
}