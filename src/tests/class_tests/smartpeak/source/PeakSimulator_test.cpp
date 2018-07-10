/**TODO:  Add copyright*/

#define BOOST_TEST_MODULE PeakSimulator test suite 
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/simulator/PeakSimulator.h>

#include <iostream>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(peaksimulator)

// class PeakSimulator_test: public PeakSimulator
// {
// public:
// };

BOOST_AUTO_TEST_CASE(constructor) 
{
  PeakSimulator* ptr = nullptr;
  PeakSimulator* nullPointer = nullptr;
	ptr = new PeakSimulator();
  BOOST_CHECK_NE(ptr, nullPointer);
}

BOOST_AUTO_TEST_CASE(constructor2) 
{  
  PeakSimulator psim(500.0, 1.0, 0.0, 10.0, 2.0, 1.0, 5.0, 10.0, 1e6);

  BOOST_CHECK_EQUAL(psim.getStepSizeMu(), 500.0);
  BOOST_CHECK_EQUAL(psim.getStepSizeSigma(), 1.0);
  BOOST_CHECK_EQUAL(psim.getWindowStart(), 0.0);
  BOOST_CHECK_EQUAL(psim.getWindowEnd(), 10.0);
  BOOST_CHECK_EQUAL(psim.getNoiseMu(), 2.0);
  BOOST_CHECK_EQUAL(psim.getNoiseSigma(), 1.0);
  BOOST_CHECK_EQUAL(psim.getBaselineLeft(), 5.0);
  BOOST_CHECK_EQUAL(psim.getBaselineRight(), 10.0);
  BOOST_CHECK_EQUAL(psim.getSaturationLimit(), 1e6);
}

BOOST_AUTO_TEST_CASE(destructor) 
{
  PeakSimulator* ptr = nullptr;
	ptr = new PeakSimulator();
  delete ptr;
}

BOOST_AUTO_TEST_CASE(gettersAndSetters) 
{
  PeakSimulator psim;
  psim.setStepSizeMu(500.0);
  psim.setStepSizeSigma(1.0);
  psim.setWindowStart(0.0);
  psim.setWindowEnd(10.0);
  psim.setNoiseMu(2.0);
  psim.setNoiseSimga(1.0);
  psim.setBaselineLeft(5.0);
  psim.setBaselineRight(10.0);
  psim.setSaturationLimit(1e6);

  BOOST_CHECK_EQUAL(psim.getStepSizeMu(), 500.0);
  BOOST_CHECK_EQUAL(psim.getStepSizeSigma(), 1.0);
  BOOST_CHECK_EQUAL(psim.getWindowStart(), 0.0);
  BOOST_CHECK_EQUAL(psim.getWindowEnd(), 10.0);
  BOOST_CHECK_EQUAL(psim.getNoiseMu(), 2.0);
  BOOST_CHECK_EQUAL(psim.getNoiseSigma(), 1.0);
  BOOST_CHECK_EQUAL(psim.getBaselineLeft(), 5.0);
  BOOST_CHECK_EQUAL(psim.getBaselineRight(), 10.0);
  BOOST_CHECK_EQUAL(psim.getSaturationLimit(), 1e6);
}

BOOST_AUTO_TEST_CASE(generateRangeWithNoise) 
{ 
  PeakSimulator psim;

  // no noise
  std::vector<double> range = psim.generateRangeWithNoise(0.0, 1.0, 0.0, 10.0);
  BOOST_CHECK_EQUAL(range.size(), 11);
  BOOST_CHECK_EQUAL(range[0], 0.0);
  BOOST_CHECK_EQUAL(range[10], 10.0);

  // with noise
  range = psim.generateRangeWithNoise(0.0, 1.0, 0.1, 10.0);
  BOOST_CHECK_EQUAL(range[0], 0.0);
  BOOST_CHECK_NE(range[10], 10.0);

  // negative step size
  range = psim.generateRangeWithNoise(0.0, -1.0, 0.0, 10.0);
  BOOST_CHECK_EQUAL(range.size(), 11);
  BOOST_CHECK_EQUAL(range[0], 0.0);
  BOOST_CHECK_EQUAL(range[10], 10.0);

  // high standard deviation
  range = psim.generateRangeWithNoise(0.0, 1.0, 10.0, 10.0);
  BOOST_CHECK_EQUAL(range.size(), 11);
  BOOST_CHECK_EQUAL(range[0], 0.0);
  BOOST_CHECK_EQUAL(range[10], 10.0);
}

BOOST_AUTO_TEST_CASE(addNoise) 
{ 
  PeakSimulator psim;

  // no noise
  const std::vector<double> range = {0, 1, 2, 3, 4, 5};
  std::vector<double> noise_range = range;
  psim.addNoise(noise_range, 0.0, 0.0);
  for (int i=0; i<range.size(); ++i)
  {
    BOOST_CHECK_EQUAL(range[i], noise_range[i]);
  }

  // with noise
  noise_range = range;
  psim.addNoise(noise_range, 0.0, 1.0);
  for (int i=0; i<range.size(); ++i)
  {
    BOOST_CHECK_NE(range[i], noise_range[i]);
  }

  // with noise
  noise_range = range;
  psim.addNoise(noise_range, 1.0, 0.0);
  for (int i=0; i<range.size(); ++i)
  {
    BOOST_CHECK_EQUAL(range[i] + 1.0, noise_range[i]);
  }
}

BOOST_AUTO_TEST_CASE(addBaseline) 
{ 
  PeakSimulator psim;

  // toy peak
  const std::vector<double> x = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  const std::vector<double> y = {0, 0, 1, 3, 7, 10, 7, 3, 1, 0, 0};
  
  // no baseline
  std::vector<double> y_baseline = y;
  psim.addBaseline(x, y_baseline, 0.0, 0.0, 5);
  std::vector<double> y_test = {0, 0, 1, 3, 7, 10, 7, 3, 1, 0, 0};
  for (int i=0; i<y_test.size(); ++i)
  {
    BOOST_CHECK_EQUAL(y_baseline[i], y_test[i]);
  }

  // no noise
  y_baseline = y;
  psim.addBaseline(x, y_baseline, 3, 1, 5);
  y_test = {3, 3, 3, 3, 7, 10, 7, 3, 1, 1, 1};
  for (int i=0; i<y_test.size(); ++i)
  {
    BOOST_CHECK_EQUAL(y_baseline[i], y_test[i]);
  }
}

BOOST_AUTO_TEST_CASE(flattenPeak) 
{ 
  PeakSimulator psim;

  // toy peak
  const std::vector<double> y = {0, 0, 1, 3, 7, 10, 7, 3, 1, 0, 0};
  
  // no saturation
  std::vector<double> y_saturation = y;
  psim.flattenPeak(y_saturation, 10);
  for (int i=0; i<y.size(); ++i)
  {
    BOOST_CHECK_EQUAL(y_saturation[i], y[i]);
  }

  // saturation
  y_saturation = y;
  psim.flattenPeak(y_saturation, 5);
  std::vector<double> y_test = {0, 0, 1, 3, 5, 5, 5, 3, 1, 0, 0};
  for (int i=0; i<y_test.size(); ++i)
  {
    BOOST_CHECK_EQUAL(y_saturation[i], y_test[i]);
  }
}

BOOST_AUTO_TEST_CASE(simulatePeak) 
{ 

  // time and intensity arrays
  std::vector<double> x, y;

  // Gaussian peak, evenly spaced points, no detector noise or saturation
  PeakSimulator psim(1.0, 0.0, 
    0.0, 10.0, 
    0.0, 0.0,
    0.0, 0.0, 
    15);
  EMGModel emg(10.0, 0.0, 5.0, 1.0);

  psim.simulatePeak(x, y, emg);
  std::vector<double> x_test = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  std::vector<double> y_test = {3.7266531720786709e-05,
    0.0033546262790251184, 0.11108996538242306, 
    1.353352832366127, 6.0653065971263338, 10,
    6.0653065971263338, 1.353352832366127, 0.11108996538242306,
    0.0033546262790251184, 3.7266531720786709e-05};
  for (int i=0; i<x.size(); ++i)
  {
    BOOST_CHECK_CLOSE(x[i], x_test[i], 1e-6);
    BOOST_CHECK_CLOSE(y[i], y_test[i], 1e-6);
  }

  // time and intensity arrays
  x.clear();
  y.clear();

  // Tailing peak, evenly spaced points, no detector noise or saturation
  psim = PeakSimulator(1.0, 0.0, 
    0.0, 10.0, 
    0.0, 0.0,
    0.0, 0.0, 
    15);
  emg = EMGModel(10.0, 0.5, 5.0, 1.0);

  psim.simulatePeak(x, y, emg);
  x_test = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  y_test = {1.04424e-05, 0.0010894327367425214,
  0.0428381, 0.640548, 3.69487, 8.42738,
  7.95379, 3.39235, 0.772531, 0.1214391, 0.0167949};
  for (int i=0; i<x.size(); ++i)
  {
    BOOST_CHECK_CLOSE(x[i], x_test[i], 1e-4);
    BOOST_CHECK_CLOSE(y[i], y_test[i], 1e-4);
  }

  // time and intensity arrays
  x.clear();
  y.clear();

  // Tailing peak, non-evenly spaced points, detector noise and saturation
  psim = PeakSimulator(1.0, 0.1, 
    0.0, 10.0, 
    0.0, 0.5,
    1.0, 3.0, 
    8);
  emg = EMGModel(10.0, 0.5, 5.0, 1.0);

  psim.simulatePeak(x, y, emg);
  x_test = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  y_test = {1.04424e-05, 0.0010894327367425214,
  0.0428381, 0.640548, 3.69487, 8.42738,
  7.95379, 3.39235, 0.772531, 0.1214391, 0.0167949};
  BOOST_CHECK_EQUAL(x.size(), y.size());
  BOOST_CHECK_EQUAL(x[0], 0);
  for (int i=0; i<y.size(); ++i)
  {
    BOOST_CHECK(y[i] <= 8.0);
  }

  // time and intensity arrays
  x.clear();
  y.clear();

  // Negative step size
  psim = PeakSimulator(-0.2, 0.0, 
    0.0, 10.0, 
    0.0, 0.0,
    0.0, 0.0, 
    15);
  emg = EMGModel(10.0, 0.0, 5.0, 1.0);

  psim.simulatePeak(x, y, emg);
  x_test = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  y_test = {3.7266531720786709e-05,
    0.0033546262790251184, 0.11108996538242306, 
    1.353352832366127, 6.0653065971263338, 10,
    6.0653065971263338, 1.353352832366127, 0.11108996538242306,
    0.0033546262790251184, 3.7266531720786709e-05};
  for (int i=0; i<x.size(); ++i)
  {
    BOOST_CHECK_CLOSE(x[i], x_test[i], 1e-6);
    BOOST_CHECK_CLOSE(y[i], y_test[i], 1e-6);
  }

  // // UNCOMMENT to print out new test values
  // for (int i=0; i<x.size(); ++i)
  // {
  //   std::cout<< x[i] << ", ";
  // }
  // std::cout<< ";" <<std::endl;
  // for (int i=0; i<y.size(); ++i)
  // {
  //   std::cout<< y[i] << ", ";
  // }
  // std::cout<< ";" <<std::endl;
}

BOOST_AUTO_TEST_SUITE_END()