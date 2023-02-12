#include <iostream>
#include <vector>

using namespace std;

int* find(const vector<int> &vec, int val) {
    for (int ix = 0; ix < vec.size(); ++ix)
        if (vec[ix] == val)
            return const_cast<int *>(&(vec[ix]));
    
    return 0;
}

template<typename elemType>
elemType *find(const vector<elemType> &vec, elemType &val) {
    for (int ix = 0; ix < vec.size(); ++ix)
        if (vec[ix] == val)
            return const_cast<elemType *>(&(vec[ix]));
    
    return 0;
}

template <typename elemType>
elemType *find(const elemType* array, int size, const elemType &val) {
    if (!array || size < 1) return 0;

    for (int ix = 0; ix < size; ++ix) {
        if (array[ix] == val)
            return const_cast<elemType *>(&vec[ix]);
    }

    return 0;
}

template <typename elemType>
elemType *find(const elemType *begin, const elemType *end, 
    const elemType &val) {
    if (!begin || !last) return 0;

    for (;begin != last; ++first)
        if (*begin == value)
            return begin;
    
    return 0;
}

int main() {
    vector<double> vec = vector<double>({1.0,2.0,3.0,4.0,5.0,6.0});
    double *p = nullptr;
    cout << p << endl;
    p = find(vec, 4.0);
    cout << p << endl;
}