// taken from http://www.johndcook.com/blog/standard_deviation/

#ifndef running_stat_h
#define running_stat_h

class running_stat {
  int n;
  double oldM, newM, oldS, newS;

public:
  running_stat();

  void clear();
  void push(double x);
  void push(double x, double w); // do not use this on the first iteration
  int  num() const;

  double mean() const;
  double var() const;
  double stdev() const;
};

#endif
