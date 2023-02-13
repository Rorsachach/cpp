int main() {
    int a = 10;
    int b = 11;
    const int* p = &a;
    int* const p = &a;
    *p = 12;
    p = &b;
    *p = 12;
    p = &b;
}