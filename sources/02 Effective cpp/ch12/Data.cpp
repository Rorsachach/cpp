struct Day {
    explicit Day(int d) : val(d) {}
    int val;
};

// struct Month {
//     explicit Month(int m) : val(m) {}
//     int val;
// };

class Month {
public:
    static Month Jan() { return Month(1); }
    static Month Feb() { return Month(2); }
    static Month Mar() { return Month(3); }
    static Month Apr() { return Month(4); }
    static Month May() { return Month(5); }
    static Month Jun() { return Month(6); }
    static Month Jul() { return Month(7); }
    static Month Aug() { return Month(8); }
    static Month Sep() { return Month(9); }
    static Month Oct() { return Month(10); }
    static Month Nov() { return Month(11); }
    static Month Dec() { return Month(12); }
private:
    int val;
    explicit Month(int m) : val(m) {}
};

struct Year {
    explicit Year(int y) : val(y) {}
    int val;
};

class Date {
public:
    Date(const Month& month, const Day& day, const Year& year) : _month(month),
        _day(day), _year(year) {};

private:
    Month _month;
    Day _day;
    Year _year;
};

int main() {
    // Date d(Month(2), Day(15), Year(2023));
    Date d(Month::Feb(), Day(15), Year(2023));
}