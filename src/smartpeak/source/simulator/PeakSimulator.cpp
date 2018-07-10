/**TODO:  Add copyright*/

#include <SmartPeak/simulator/PeakSimulator.h>
#include <SmartPeak/simulator/EMGModel.h>

#include <vector>
#include <random>
#include <iostream>

namespace SmartPeak
{
  PeakSimulator::PeakSimulator()
  {        
  }

  PeakSimulator::PeakSimulator(const double& step_size_mu,
    const double& step_size_sigma,
    const double& window_start,
    const double& window_end,
    const double& noise_mu,
    const double& noise_sigma,
    const double& baseline_left,
    const double& baseline_right,
    const double& saturation_limit)
  {
    step_size_mu_ = step_size_mu;
    step_size_sigma_ = step_size_sigma;
    window_start_ = window_start;
    window_end_ = window_end;
    noise_mu_ = noise_mu;
    noise_sigma_ = noise_sigma;
    baseline_left_ = baseline_left;
    baseline_right_ = baseline_right;
    saturation_limit_ = saturation_limit;
  }

  PeakSimulator::~PeakSimulator()
  {
  }

  void PeakSimulator::setStepSizeMu(const double& step_size_mu)
  {
    step_size_mu_ = step_size_mu;
  }
  double PeakSimulator::getStepSizeMu() const
  {
    return step_size_mu_;
  }

  void PeakSimulator::setStepSizeSigma(const double& step_size_sigma)
  {
    step_size_sigma_ = step_size_sigma;
  }
  double PeakSimulator::getStepSizeSigma() const
  {
    return step_size_sigma_;
  }

  void PeakSimulator::setWindowStart(const double& window_start)
  {
    window_start_ = window_start;
  }
  double PeakSimulator::getWindowStart() const
  {
    return window_start_;
  }

  void PeakSimulator::setWindowEnd(const double& window_end)
  {
    window_end_ = window_end;
  }
  double PeakSimulator::getWindowEnd() const
  {
    return window_end_;
  }

  void PeakSimulator::setNoiseMu(const double& noise_mu)
  {
    noise_mu_ = noise_mu;
  }
  double PeakSimulator::getNoiseMu() const
  {
    return noise_mu_;
  }

  void PeakSimulator::setNoiseSimga(const double& noise_sigma)
  {
    noise_sigma_ = noise_sigma;
  }
  double PeakSimulator::getNoiseSigma() const
  {
    return noise_sigma_;
  }

  void PeakSimulator::setBaselineLeft(const double& baseline_left)
  {
    baseline_left_ = baseline_left;
  }
  double PeakSimulator::getBaselineLeft() const
  {
    return baseline_left_;
  }

  void PeakSimulator::setBaselineRight(const double& baseline_right)
  {
    baseline_right_ = baseline_right;
  }
  double PeakSimulator::getBaselineRight() const
  {
    return baseline_right_;
  }

  void PeakSimulator::setSaturationLimit(const double& saturation_limit)
  {
    saturation_limit_ = saturation_limit;
  }
  double PeakSimulator::getSaturationLimit() const
  {
    return saturation_limit_;
  }

  std::vector<double> PeakSimulator::generateRangeWithNoise(
    const double& start, const double& step_mu, const double& step_sigma, const double& end)
  {
    std::random_device rd{};
    std::mt19937 gen{rd()};

    double step_mu_used = step_mu;
    double step_sigma_used = step_sigma;
    // TODO: improve defaults
    if (step_mu <= 0.0)
    {
      std::cout << "Warning: mean of step size will generate negative values.  A default mean of 1.0 and std_dev of 0.0 will be used instead." << std::endl;
      step_mu_used = 1.0;
      step_sigma_used = 0.0;
    }
    else if (step_mu - 5*step_sigma <= 0.0)
    {
      std::cout << "Warning: mean and std_dev of range step size may generate negative values.  Reduce std_dev to at least 1/5 the mean of the step size.  A default std_dev of 0.0 will be used instead." << std::endl;
      step_sigma_used = 0.0;
    }

    std::normal_distribution<> d{step_mu_used, step_sigma_used};
    std::vector<double> array;
    double value = start;

    int cnt = 0;  // checks to ensure that an infinite loop is not run
    while(value <= end || cnt > 1e6)
    {
      array.push_back(value);
      value += d(gen); // could recode to better handle rounding errors
      cnt += 1;
    }
    return array;
  }

  void PeakSimulator::addNoise(
    std::vector<double>& array_IO,
      const double& mean, const double& std_dev)
  {
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<> d{mean, std_dev};
    // add noise to a new array
    for (double& value: array_IO)
    {
      value = value + d(gen);
    }
  }

  void PeakSimulator::addBaseline(
      const std::vector<double>& x_I,
      std::vector<double>& y_IO,
      const double& baseline_left, const double& baseline_right,
      const double& peak_apex)
  {
    for (int i = 0; i < x_I.size(); ++i)
    {
      if (x_I[i] <= peak_apex)
      {
        y_IO[i] = std::max(baseline_left, y_IO[i]);
      }
      else
      {
        y_IO[i] = std::max(baseline_right, y_IO[i]);
      }
    }
  }

  void PeakSimulator::flattenPeak(
      std::vector<double>& array_IO,
      const double& saturation_limit)
  {
    for (double& value: array_IO)
    {
      value = (value > saturation_limit) ? saturation_limit: value;      
    }
  }

  void PeakSimulator::simulatePeak(
    std::vector<double>& x_O, std::vector<double>& y_O, 
    const EMGModel& emg) const
  {
    x_O.clear();
    y_O.clear();

    // make the time array
    x_O = generateRangeWithNoise(window_start_, step_size_mu_, step_size_sigma_, window_end_);
    // make the intensity array
    for (double x: x_O)
    {
      y_O.push_back(emg.PDF(x));
    }
    // add a baseline to the intensity array
    addBaseline(x_O, y_O, baseline_left_, baseline_right_, emg.getMu());
    // add noise to the intensity array
    addNoise(y_O, noise_mu_, noise_sigma_);
    // add saturation limit
    flattenPeak(y_O, saturation_limit_);
  }
}