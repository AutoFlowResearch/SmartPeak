/**TODO:  Add copyright*/

#pragma once

namespace SmartPeak
{
  /**
    @brief A class to generate points following an EMG distribution.

    References:
    Kalambet, Y.; Kozmin, Y.; Mikhailova, K.; Nagaev, I.; Tikhonov, P. (2011).
      "Reconstruction of chromatographic peaks using the exponentially modified Gaussian function".
      Journal of Chemometrics. 25 (7): 352. doi:10.1002/cem.1343
    Delley, R (1985).
      "Series for the Exponentially Modified Gaussian Peak Shape".
      Anal. Chem. 57: 388. doi:10.1021/ac00279a094.
    Dyson, N. A. (1998).
      Chromatographic Integration Methods.
      Royal Society of Chemistry, Information Services. p. 27. ISBN 9780854045105. Retrieved 2015-05-15.
  */
  class EMGModel
  {
    /**
    Notes on potential optimizations:
    1. make a virtual class called StatisticalModel
    2. make a virtual class called PDF
    3. make a virtual class called CDF
    4. setters/getters would be unique to each derived class
    */
public:
    EMGModel() = default; ///< Default constructor
    EMGModel(const double& h,
      const double& tau,
      const double& mu,
      const double& sigma); ///< Explicit constructor
    ~EMGModel() = default; ///< Default destructor

    void setH(const double& h); ///< EMG h setter
    double getH() const; ///< EMG h getter

    void setTau(const double& tau); ///< EMG tau setter
    double getTau() const; ///< EMG tau getter

    void setMu(const double& mu); ///< EMG mu setter
    double getMu() const; ///< EMG mu getter

    void setSigma(const double& sigma); ///< EMG sigma setter
    double getSigma() const; ///< EMG sigma getter

    /**
      @brief Calculates points from an EMG PDF

      @param[in] x_I X value of the EMG PDF

      @returns Y value of the EMG PDF.
    */
    double PDF(const double& x_I) const;

protected:
    /**
      @brief Calculates points from an EMG PDF using method 1

      @param[in] x_I X value of the EMG PDF

      @returns Y value of the EMG PDF.
    */
    double EMGPDF1_(const double& x_I) const;

    /**
      @brief Calculates points from an EMG PDF using method 2

      @param[in] x_I X value of the EMG PDF

      @returns Y value of the EMG PDF.
    */
    double EMGPDF2_(const double& x_I) const;

    /**
      @brief Calculates points from an EMG PDF using method 3

      @param[in] x_I X value of the EMG PDF

      @returns Y value of the EMG PDF.
    */
    double EMGPDF3_(const double& x_I) const;

    /**
      @brief Calculates the parameter z, which is used to decide
        which formulation of the EMG PDF to use for calculation.

      @param[in] x_I X value of the EMG PDF

      @returns z parameter.
    */
    double z_(const double& x_I) const;

private:
    double emg_h_ = 1.0; ///< Amplitude of the Gaussian peak
    double emg_tau_ = 0.1; ///< Exponential relaxation time
    double emg_mu_ = 0.0; ///< Mean of the EMG
    double emg_sigma_ = 1.0; ///< Standard deviation of the EGM

  };
}
