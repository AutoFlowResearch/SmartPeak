/**TODO:  Add copyright*/

#define BOOST_TEST_MODULE ChromatogramSimulator test suite 
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/simulator/ChromatogramSimulator.h>
#include <SmartPeak/simulator/PeakSimulator.h>
#include <SmartPeak/simulator/EMGModel.h>

#include <iostream>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(chromatogramsimulator)

// class ChromatogramSimulator_test: public ChromatogramSimulator
// {
// public:
// };

BOOST_AUTO_TEST_CASE(constructor) 
{
  ChromatogramSimulator* ptr = nullptr;
  ChromatogramSimulator* nullPointer = nullptr;
	ptr = new ChromatogramSimulator();
  BOOST_CHECK_NE(ptr, nullPointer);
}

BOOST_AUTO_TEST_CASE(destructor) 
{
  ChromatogramSimulator* ptr = nullptr;
	ptr = new ChromatogramSimulator();
  delete ptr;
}

BOOST_AUTO_TEST_CASE(findPeakOverlap)
{
  ChromatogramSimulator chromsimulator;
  PeakSimulator peak_left, peak_right;
  EMGModel emg_left, emg_right;  

 // Overlapping windows; left and right baseline are equal;
  peak_left = PeakSimulator(1.0, 0.0, 
    0.0, 12.0, 
    0.0, 0.0,
    1.0, 5.0, //bl, br
    15);
  peak_right = PeakSimulator(1.0, 0.0, 
    8.0, 20.0, 
    0.0, 0.0,
    5.0, 1.0, //bl, br
    15);
  emg_left = EMGModel(10.0, 0.0, 5.0, 1.0);
  emg_right = EMGModel(10.0, 0.0, 15.0, 1.0);
  chromsimulator.findPeakOverlap(peak_left, emg_left, peak_right, emg_right);
  BOOST_CHECK_EQUAL(chromsimulator.findPeakOverlap(peak_left, emg_left, peak_right, emg_right), 12.0);

 // Merged peaks: both baselines overlap
  peak_left = PeakSimulator(1.0, 0.0, 
    0.0, 15.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    15);
  peak_right = PeakSimulator(1.0, 0.0, 
    5.0, 20.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    15);
  emg_left = EMGModel(10.0, 0.0, 9.0, 1.0);
  emg_right = EMGModel(10.0, 0.0, 11.0, 1.0);
  chromsimulator.findPeakOverlap(peak_left, emg_left, peak_right, emg_right);
  BOOST_CHECK_EQUAL(chromsimulator.findPeakOverlap(peak_left, emg_left, peak_right, emg_right), 10.0);

 // Merged peaks: both baselines do not overlap
  peak_left = PeakSimulator(1.0, 0.0, 
    0.0, 13.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    15);
  peak_right = PeakSimulator(1.0, 0.0, 
    5.0, 20.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    15);
  emg_left = EMGModel(10.0, 0.0, 9.0, 1.0);
  emg_right = EMGModel(10.0, 0.0, 11.0, 1.0);
  chromsimulator.findPeakOverlap(peak_left, emg_left, peak_right, emg_right);
  BOOST_CHECK_EQUAL(chromsimulator.findPeakOverlap(peak_left, emg_left, peak_right, emg_right), 10.0);
}

BOOST_AUTO_TEST_CASE(joinPeakWindows) 
{
  ChromatogramSimulator chromsimulator;
  PeakSimulator peak_left, peak_right;
  EMGModel emg_left, emg_right;

  // Perfect overlap; no differences in baseline
  peak_left = PeakSimulator(1.0, 0.0, 
    0.0, 10.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    15);
  peak_right = PeakSimulator(1.0, 0.0, 
    10.0, 20.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    15);
  emg_left = EMGModel(10.0, 0.0, 5.0, 1.0);
  emg_right = EMGModel(10.0, 0.0, 15.0, 1.0);
  chromsimulator.joinPeakWindows(peak_left, emg_left, peak_right, emg_right);
  BOOST_CHECK_EQUAL(peak_left.getBaselineRight(), 1.0);
  BOOST_CHECK_EQUAL(peak_right.getBaselineLeft(), 1.0);
  BOOST_CHECK_EQUAL(peak_left.getWindowEnd(), 10.0);
  BOOST_CHECK_EQUAL(peak_right.getWindowStart(), 10.0);
  BOOST_CHECK_EQUAL(emg_left.getMu(), 5.0);
  BOOST_CHECK_EQUAL(emg_right.getMu(), 15.0);

  // Perfect overlap; no differences in baseline
  // swapped peaks
  peak_right = PeakSimulator(1.0, 0.0, 
    0.0, 10.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    15);
  peak_left = PeakSimulator(1.0, 0.0, 
    10.0, 20.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    15);
  emg_right = EMGModel(10.0, 0.0, 5.0, 1.0);
  emg_left = EMGModel(10.0, 0.0, 15.0, 1.0);
  chromsimulator.joinPeakWindows(peak_left, emg_left, peak_right, emg_right);
  BOOST_CHECK_EQUAL(peak_left.getBaselineRight(), 1.0);
  BOOST_CHECK_EQUAL(peak_right.getBaselineLeft(), 1.0);
  BOOST_CHECK_EQUAL(peak_left.getWindowStart(), 0.0);
  BOOST_CHECK_EQUAL(peak_left.getWindowEnd(), 10.0);
  BOOST_CHECK_EQUAL(peak_right.getWindowStart(), 10.0);
  BOOST_CHECK_EQUAL(peak_right.getWindowEnd(), 20.0);
  BOOST_CHECK_EQUAL(emg_left.getMu(), 5.0);
  BOOST_CHECK_EQUAL(emg_right.getMu(), 15.0);

 // Non overlapping windows; Left baseline is higher
  peak_left = PeakSimulator(1.0, 0.0, 
    0.0, 8.0, 
    0.0, 0.0,
    1.0, 5.0, //bl, br
    15);
  peak_right = PeakSimulator(1.0, 0.0, 
    12.0, 20.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    15);
  emg_left = EMGModel(10.0, 0.0, 5.0, 1.0);
  emg_right = EMGModel(10.0, 0.0, 15.0, 1.0);
  chromsimulator.joinPeakWindows(peak_left, emg_left, peak_right, emg_right);
  BOOST_CHECK_EQUAL(peak_left.getBaselineRight(), 5.0);
  BOOST_CHECK_EQUAL(peak_right.getBaselineLeft(), 5.0);
  BOOST_CHECK_EQUAL(peak_left.getWindowEnd(), 12.0);
  BOOST_CHECK_EQUAL(peak_right.getWindowStart(), 12.0);
  BOOST_CHECK_EQUAL(emg_left.getMu(), 5.0);
  BOOST_CHECK_EQUAL(emg_right.getMu(), 15.0);

 // Non overlapping windows; Right baseline is higher
  peak_left = PeakSimulator(1.0, 0.0, 
    0.0, 8.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    15);
  peak_right = PeakSimulator(1.0, 0.0, 
    12.0, 20.0, 
    0.0, 0.0,
    5.0, 1.0, //bl, br
    15);
  emg_left = EMGModel(10.0, 0.0, 5.0, 1.0);
  emg_right = EMGModel(10.0, 0.0, 15.0, 1.0);
  chromsimulator.joinPeakWindows(peak_left, emg_left, peak_right, emg_right);
  BOOST_CHECK_EQUAL(peak_left.getBaselineRight(), 5.0);
  BOOST_CHECK_EQUAL(peak_right.getBaselineLeft(), 5.0);
  BOOST_CHECK_EQUAL(peak_left.getWindowEnd(), 12.0);
  BOOST_CHECK_EQUAL(peak_right.getWindowStart(), 12.0);
  BOOST_CHECK_EQUAL(emg_left.getMu(), 5.0);
  BOOST_CHECK_EQUAL(emg_right.getMu(), 15.0);

 // Overlapping windows; Left baseline is higher
  peak_left = PeakSimulator(1.0, 0.0, 
    0.0, 12.0, 
    0.0, 0.0,
    1.0, 5.0, //bl, br
    15);
  peak_right = PeakSimulator(1.0, 0.0, 
    8.0, 20.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    15);
  emg_left = EMGModel(10.0, 0.0, 5.0, 1.0);
  emg_right = EMGModel(10.0, 0.0, 15.0, 1.0);
  chromsimulator.joinPeakWindows(peak_left, emg_left, peak_right, emg_right);
  BOOST_CHECK_EQUAL(peak_left.getBaselineRight(), 5.0);
  BOOST_CHECK_EQUAL(peak_right.getBaselineLeft(), 5.0);
  BOOST_CHECK_EQUAL(peak_left.getWindowEnd(), 12.0);
  BOOST_CHECK_EQUAL(peak_right.getWindowStart(), 12.0);
  BOOST_CHECK_EQUAL(emg_left.getMu(), 5.0);
  BOOST_CHECK_EQUAL(emg_right.getMu(), 15.0);

 // Overlapping windows; Right baseline is higher
  peak_left = PeakSimulator(1.0, 0.0, 
    0.0, 12.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    15);
  peak_right = PeakSimulator(1.0, 0.0, 
    8.0, 20.0, 
    0.0, 0.0,
    5.0, 1.0, //bl, br
    15);
  emg_left = EMGModel(10.0, 0.0, 5.0, 1.0);
  emg_right = EMGModel(10.0, 0.0, 15.0, 1.0);
  chromsimulator.joinPeakWindows(peak_left, emg_left, peak_right, emg_right);
  BOOST_CHECK_EQUAL(peak_left.getBaselineRight(), 5.0);
  BOOST_CHECK_EQUAL(peak_right.getBaselineLeft(), 5.0);
  BOOST_CHECK_EQUAL(peak_left.getWindowEnd(), 12.0);
  BOOST_CHECK_EQUAL(peak_right.getWindowStart(), 12.0);
  BOOST_CHECK_EQUAL(emg_left.getMu(), 5.0);
  BOOST_CHECK_EQUAL(emg_right.getMu(), 15.0);
}

BOOST_AUTO_TEST_CASE(simulateChromatogram) 
{
  ChromatogramSimulator chromsimulator;
  PeakSimulator peak1, peak2, peak3;
  EMGModel emg1, emg2, emg3;
  std::vector<double> chrom_time, chrom_intensity, x_test, y_test;

  std::vector<PeakSimulator> peaks;
  std::vector<EMGModel> emgs;

  // Perfect gaussian peak
  peak1 = PeakSimulator(1.0, 0.0, 
    0.0, 10.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    100);
  emg1 = EMGModel(10.0, 0.0, 5.0, 1.0);
  peaks = {peak1};
  emgs = {emg1};

  chromsimulator.simulateChromatogram(chrom_time, chrom_intensity,
    peaks, emgs);
  x_test = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  y_test = {1, 1, 1, 1.35335, 6.06531, 10, 6.06531, 1.35335, 1, 1, 1};
  for (int i=0; i<chrom_time.size(); ++i)
  {
    BOOST_CHECK_CLOSE(chrom_time[i], x_test[i], 1e-3);
    BOOST_CHECK_CLOSE(chrom_intensity[i], y_test[i], 1e-3);
  }

  // Perfect gaussian peaks
  peak1 = PeakSimulator(1.0, 0.0, 
    0.0, 10.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    100);
  emg1 = EMGModel(10.0, 0.0, 5.0, 1.0);
  peak2 = PeakSimulator(1.0, 0.0, 
    10.0, 20.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    100);
  emg2 = EMGModel(10.0, 0.0, 15.0, 1.0);
  peak3 = PeakSimulator(1.0, 0.0, 
    20.0, 30.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    100);
  emg3 = EMGModel(10.0, 0.0, 25.0, 1.0);
  peaks = {peak1, peak2, peak3};
  emgs = {emg1, emg2, emg3};

  chromsimulator.simulateChromatogram(chrom_time, chrom_intensity,
    peaks, emgs);
  x_test = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
  y_test = {1, 1, 1, 1.35335, 6.06531, 10, 6.06531, 1.35335, 1, 1, 1, 1, 1, 1, 1.35335, 6.06531, 10, 6.06531, 1.35335, 1, 1, 1, 1, 1, 1, 1.35335, 6.06531, 10, 6.06531, 1.35335, 1, 1, 1};
  for (int i=0; i<chrom_time.size(); ++i)
  {
    BOOST_CHECK_CLOSE(chrom_time[i], x_test[i], 1e-3);
    BOOST_CHECK_CLOSE(chrom_intensity[i], y_test[i], 1e-3);
  }

  // Increase tailing
  peak1 = PeakSimulator(1.0, 0.0, 
    0.0, 10.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    100);
  emg1 = EMGModel(10.0, 0.0, 5.0, 1.0);
  peak2 = PeakSimulator(1.0, 0.0, 
    10.0, 20.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    100);
  emg2 = EMGModel(10.0, 0.2, 15.0, 1.0);
  peak3 = PeakSimulator(1.0, 0.0, 
    20.0, 30.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    100);
  emg3 = EMGModel(10.0, 1.0, 25.0, 1.0);
  peaks = {peak1, peak2, peak3};
  emgs = {emg1, emg2, emg3};

  chromsimulator.simulateChromatogram(chrom_time, chrom_intensity,
    peaks, emgs);
  x_test = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
  y_test = {1, 1, 1, 1.35335, 6.06531, 10, 6.06531, 1.35335, 1, 1, 1, 1, 1, 1, 1, 4.92435, 9.64041, 7.17685, 2.06109, 1, 1, 1, 1, 1, 1, 1, 2.55573, 6.5568, 7.60173, 4.70568, 2.01076, 1, 1};
  for (int i=0; i<chrom_time.size(); ++i)
  {
    BOOST_CHECK_CLOSE(chrom_time[i], x_test[i], 1e-3);
    BOOST_CHECK_CLOSE(chrom_intensity[i], y_test[i], 1e-3);
  }

  // Overlap and cutoff peak
  peak1 = PeakSimulator(1.0, 0.0, 
    0.0, 10.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    100);
  emg1 = EMGModel(10.0, 0.0, 5.0, 1.0);
  peak2 = PeakSimulator(1.0, 0.0, 
    10.0, 20.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    100);
  emg2 = EMGModel(10.0, 0.0, 7.0, 1.0);
  peak3 = PeakSimulator(1.0, 0.0, 
    20.0, 30.0, 
    0.0, 0.0,
    1.0, 1.0, //bl, br
    100);
  emg3 = EMGModel(10.0, 0.0, 29.0, 1.0);
  peaks = {peak1, peak2, peak3};
  emgs = {emg1, emg2, emg3};

  chromsimulator.simulateChromatogram(chrom_time, chrom_intensity,
    peaks, emgs);
  x_test = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
  y_test = {1, 1, 1, 1.35335, 6.06531, 10, 6.06531, 1.35335, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1.35335, 6.06531, 10, 6.06531};
  for (int i=0; i<chrom_time.size(); ++i)
  {
    BOOST_CHECK_CLOSE(chrom_time[i], x_test[i], 1e-3);
    BOOST_CHECK_CLOSE(chrom_intensity[i], y_test[i], 1e-3);
  }

  // // UNCOMMENT to print out new test values
  // for (int i=0; i<chrom_time.size(); ++i)
  // {
  //   std::cout<< chrom_time[i] << "  " << chrom_intensity[i] <<std::endl;
  // }
  // std::cout<< ";" <<std::endl;
  // for (int i=0; i<chrom_time.size(); ++i)
  // {
  //   std::cout<< chrom_time[i] << ", ";
  // }
  // std::cout<< ";" <<std::endl;
  // for (int i=0; i<chrom_intensity.size(); ++i)
  // {
  //   std::cout<< chrom_intensity[i] << ", ";
  // }
  // std::cout<< ";" <<std::endl;

}

BOOST_AUTO_TEST_SUITE_END()