#ifndef array_h
#define array_h

template<typename T, size_t N>
class array {
  private:
    T x[N];
  public:
    T& operator[](size_t i) { return x[i]; }
    const T& operator[](size_t i) const { return x[i]; }

    static size_t size() { return N; }
};

#endif
