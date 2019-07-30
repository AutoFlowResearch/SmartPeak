// TODO: Add copyright

#pragma once


namespace SmartPeak
{
  /// docs
  class FIADataProcessor
  {
public:
    FIADataProcessor()                                      = delete;
    ~FIADataProcessor()                                     = delete;
    FIADataProcessor(const FIADataProcessor&)               = delete;
    FIADataProcessor& operator=(const FIADataProcessor&)    = delete;
    FIADataProcessor(FIADataProcessor&&)                    = delete;
    FIADataProcessor& operator=(FIADataProcessor&&)         = delete;

    // /**
    //   docs

    //   @note notes

    //   @param[in] param-in
    //   @param[out] param-out
    // */
    static void integrate_by_time(
    );
  };
}
