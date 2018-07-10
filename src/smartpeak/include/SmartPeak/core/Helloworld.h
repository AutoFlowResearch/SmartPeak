#ifndef SMARTPEAK_HELLOWORLD_H
#define SMARTPEAK_HELLOWORLD_H

namespace SmartPeak
{

  class Helloworld
  {
public:
    /// Default constructor
    Helloworld();    
    /// Destructor
    ~Helloworld();

    double addNumbers(const double& x, const double& y) const;

  };
}

#endif //SMARTPEAK_HELLOWORLD_H