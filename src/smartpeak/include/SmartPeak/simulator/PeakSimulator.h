/**TODO:  Add copyright*/

#pragma once

#include <SmartPeak/simulator/EMGModel.h>

#include <vector>
#include <random>

namespace SmartPeak
{
  /**
    @brief Peak simulator.

    This class generates a chromatogram or spectrum peak.  The peak is modeled
      after an exponentially modified gaussian (EMG).

    References:
    Kalambet, Y.; Kozmin, Y.; Mikhailova, K.; Nagaev, I.; Tikhonov, P. (2011).
      "Reconstruction of chromatographic peaks using the exponentially modified Gaussian function".
      Journal of Chemometrics. 25 (7): 352. doi:10.1002/cem.1343
  */
  class PeakSimulator
  {
    /**
    Notes on potential optimizations:
    1. make a virtual class called DataSimulator
    2. make a virtual class called simulate
    3. make a virtual class called addNoise
    4. setters/getters would be unique to each derived class
    */
public:
    PeakSimulator() = default; ///< Default constructor
    PeakSimulator(const double& step_size_mu,
      const double& step_size_sigma,
      const double& window_start,
      const double& window_end,
      const double& noise_mu,
      const double& noise_sigma,
      const double& baseline_left,
      const double& baseline_right,
      const double& saturation_limit); ///< Explicit constructor

    ~PeakSimulator() = default; ///< Default destructor

    /**
      @brief simulates two vector of points that correspond to x and y values that
        represent a peak

      @param[out] x_IO A vector of x values representing time or m/z
      @param[out] y_IO A vector of y values representing the intensity at time t or m/z m
      @param[in] emg An emg model class
    */
    void simulatePeak(std::vector<double>& x_O, std::vector<double>& y_O,
      const EMGModel& emg) const;

    /**
      @brief Generates a range of values with noise sampled from a normal distribution

      @param[in] start Range start
      @param[in] step_mu Range mean step
      @param[in] step_sigma Range step standard deviation
      @param[in] end Range end

      @returns A vector of values from range start to end.
    */
    static std::vector<double> generateRangeWithNoise(
      const double& start, const double& step_mu,
      const double& step_sigma, const double& end);

    /**
      @brief Add random noise from a normal distribution to a vector of values
        to simulate detector noise.

      @param[in,out] array_IO Vector of values to add random noise
      @param[in] mean Mean of the normal distribution
      @param[in] std_dev Standard Deviation of the normal distribution

      @returns A vector of values with added random noise.
    */
    static void addNoise(
      std::vector<double>& array_IO,
      const double& mean, const double& std_dev);

    /**
      @brief Add a y offset (i.e., baseline) to a vector of values
        to simulate a rise in the baseline.

      @param[in] x_I Vector of time values
      @param[in,out] y_IO Vector of intensity values
      @param[in] baseline_left Left baseline offset
      @param[in] baseline_right Right baseline offse
      @param[in] peak_apex Time to divide left and right peak sides

      @returns A vector of values with added baselines.
    */
    static void addBaseline(
      const std::vector<double>& x_I,
      std::vector<double>& y_IO,
      const double& baseline_left, const double& baseline_right,
      const double& peak_apex);

    /**
      @brief Flatten the top of a peak to simulate a saturated peak.

      @param[in,out] array_IO Vector of values to add a saturation point to
      @param[in] saturation_limit Saturation limit of the simulated detector

      @returns A vector of values with a simulated saturation point.
    */
    static void flattenPeak(
      std::vector<double>& array_IO,
      const double& saturation_limit);

    void setStepSizeMu(const double& step_size_mu); ///< step_size_mu setter
    double getStepSizeMu() const; ///< step_size_mu getter

    void setStepSizeSigma(const double& step_size_mu); ///< step_size_sigma setter
    double getStepSizeSigma() const; ///< step_size_sigma getter

    void setWindowStart(const double& window_start); ///< window_start setter
    double getWindowStart() const; ///< window_start getter

    void setWindowEnd(const double& window_end); ///< window_end setter
    double getWindowEnd() const; ///< window_end getter

    void setNoiseMu(const double& noise_mu); ///< noise_mu setter
    double getNoiseMu() const; ///< noise_mu getter

    void setNoiseSimga(const double& noise_sigma); ///< noise_sigma setter
    double getNoiseSigma() const; ///< noise_sigma getter

    void setBaselineLeft(const double& baseline_left); ///< baseline_left setter
    double getBaselineLeft() const; ///< baseline_left getter

    void setBaselineRight(const double& baseline_right); ///< baseline_right setter
    double getBaselineRight() const; ///< baseline_right getter

    void setSaturationLimit(const double& saturation_limit); ///< saturation_limit setter
    double getSaturationLimit() const; ///< saturation_limit getter

private:
    double step_size_mu_ = 1.0; ///< The mean spacing between points
    double step_size_sigma_ = 0.001; ///< The standard deviation of spacing between points
    double window_start_ = 0.0; ///< Peak window start
    double window_end_ = 100.0; ///< Peak window end
    double noise_mu_ = 0.0; ///< Mean of random noise generated from a normal distribution
    double noise_sigma_ = 1.0; ///< Standard deviation of random noise generated from a normal distribution
    double baseline_left_ = 0.0; ///< Height of the left baseline
    double baseline_right_ = 0.0; ///< Height of the right baseline
    double saturation_limit_ = 1e6; ///< Maximum point height before peak saturation

  };
}
