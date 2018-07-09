/**TODO:  Add copyright*/

#include <SmartPeak/simulator/ChromatogramSimulator.h>
#include <SmartPeak/simulator/PeakSimulator.h>
#include <SmartPeak/simulator/EMGModel.h>

#include <vector>
#include <algorithm>
#include <iostream>

namespace SmartPeak
{
  ChromatogramSimulator::ChromatogramSimulator()
  {        
  }

  ChromatogramSimulator::~ChromatogramSimulator()
  {
  }

  double ChromatogramSimulator::findPeakOverlap(
    const PeakSimulator& peak_left, const EMGModel& emg_left,
    const PeakSimulator& peak_right, const EMGModel& emg_right) const
  {
    std::vector<double> x_left, y_left, x_right, y_right;
    PeakSimulator peak_l = peak_left;
    PeakSimulator peak_r = peak_right;

    // move windows just to the overlapping region
    peak_l.setWindowStart(peak_r.getWindowStart());
    peak_r.setWindowEnd(peak_l.getWindowEnd());

    // simulate the peaks for the overlapping regions
    peak_l.simulatePeak(x_left, y_left, emg_left);
    peak_r.simulatePeak(x_right, y_right, emg_right);

    // find the highest point where the peaks cross
    double x_overlap = peak_left.getWindowEnd();
    double y_overlap = 0.0;
    for (int i=x_right.size()-1; i>=0; --i)
    {  // iterate in reverse order to extend the left peak
      for (int j=x_left.size()-1; j>=0; --j)
      {
        if (x_right[i] <= x_left[j] && y_right[i] <= y_left[j])
        {
          if (y_overlap < y_right[i])
          {
            y_overlap =  y_right[i];
            x_overlap =  x_right[i];
          }
        }
      }
    }
    return x_overlap;
  }
  
  void ChromatogramSimulator::joinPeakWindows(
    PeakSimulator& peak_left, EMGModel& emg_left,
    PeakSimulator& peak_right, EMGModel& emg_right) const
  {
    // Check order of left and right peaks
    if (peak_left.getWindowStart() > peak_right.getWindowStart() &&
      peak_left.getWindowEnd() > peak_right.getWindowEnd())
    {  // peaks are swapped
      std::cout << "Left and right peaks are swapped!" << std::endl;
      std::swap(peak_left, peak_right);
      std::swap(emg_left, emg_right);
    }
    
    const double x_delta = peak_right.getWindowStart() - peak_left.getWindowEnd();
    const double y_delta = peak_right.getBaselineLeft() - peak_left.getBaselineRight();
    if (x_delta >= 0.0 && y_delta <= 0.0)
    {  
      // Non overlapping windows; Left baseline is higher
      // increase the right peak baseline to match the left peak baseline
      peak_right.setBaselineLeft(peak_left.getBaselineRight()); 
      // extend left baseline to right baseline using the left peak sample rate
      peak_left.setWindowEnd(peak_right.getWindowStart());
    }
    else if (x_delta >= 0.0 && y_delta > 0.0)
    {
      // Non overlapping windows; Left baseline is lower
      // increase the left peak baseline to match the right peak baseline
      peak_left.setBaselineRight(peak_right.getBaselineLeft());
      // extend the left baseline using the left peak sample rate
      peak_left.setWindowEnd(peak_right.getWindowStart());
    }
    else if (x_delta < 0.0 && y_delta <= 0.0)
    {
      // Overlapping windows; Left baseline is higher
      // increase the right peak baseline to match the left peak baseline
      peak_right.setBaselineLeft(peak_left.getBaselineRight()); 
      // find the overlap
      const double overlap = findPeakOverlap(
        peak_left, emg_left,
        peak_right, emg_right
      );
      peak_right.setWindowStart(overlap);
      peak_left.setWindowEnd(overlap);
    }
    else if (x_delta < 0.0 && y_delta > 0.0)
    {
      // Overlapping windows; Right baseline is higher
      // increase the left peak baseline to match the right peak baseline
      peak_left.setBaselineRight(peak_right.getBaselineLeft());
      // find the overlap
      const double overlap = findPeakOverlap(
        peak_left, emg_left,
        peak_right, emg_right
      );
      peak_right.setWindowStart(overlap);
      peak_left.setWindowEnd(overlap);
    }    
  }

  void ChromatogramSimulator::simulateChromatogram(std::vector<double>& x_O, std::vector<double>& y_O,
    const std::vector<PeakSimulator>& peaks, const std::vector<EMGModel>& emgs) const
  {
    // check vector lengths
    if (peaks.size() != emgs.size())
    {
      std::cout << "Length of peaks vectors is not equal to length of EMGs vector!" << std::endl;
      std::cout << "There are " << peaks.size() << " peaks and " << emgs.size() << " EMGs." << std::endl;
      return;
    }

    // clear any potential input in x and y vectors
    x_O.clear();
    y_O.clear();

    // Order the list of peaks from lowest to highest emg_mu
    std::vector<std::pair<PeakSimulator, EMGModel>> peak_emg_pairs;
    for (int i=0; i<emgs.size(); ++i)
    {
      const std::pair<PeakSimulator, EMGModel> peak_emg(peaks[i], emgs[i]);
      peak_emg_pairs.push_back(peak_emg);
    }
    std::sort(peak_emg_pairs.begin(), peak_emg_pairs.end(),
      [](std::pair<PeakSimulator, EMGModel> lhs, std::pair<PeakSimulator, EMGModel> rhs)
      {
        return lhs.second.getMu() < rhs.second.getMu(); //ascending order
      }
    );

    // Join the peaks in order
    if (peak_emg_pairs.size() > 1)
    {
      for (int i=1; i<peak_emg_pairs.size(); ++i)
      {      
        joinPeakWindows(peak_emg_pairs[i-1].first, peak_emg_pairs[i-1].second,
          peak_emg_pairs[i].first, peak_emg_pairs[i].second);
      }
    }

    // Add the peaks in order
    for (int i=0; i<peak_emg_pairs.size(); ++i)
    {  
      // make the first peak
      std::vector<double> x, y;
      peak_emg_pairs[i].first.simulatePeak(x, y, peak_emg_pairs[i].second);

      // extend the chromatogram
      x_O.reserve(x_O.size() + distance(x.begin(), x.end()));
      x_O.insert(x_O.end(), x.begin(), x.end());
      y_O.reserve(y_O.size() + distance(y.begin(), y.end()));
      y_O.insert(y_O.end(), y.begin(), y.end());
    }    
  }
}