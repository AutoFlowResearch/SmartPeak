/**TODO:  Add copyright*/

#include <SmartPeak/simulator/EMGModel.h>

#include <cmath>

namespace SmartPeak
{
  EMGModel::EMGModel(const double& h,
    const double& tau,
    const double& mu,
    const double& sigma)
  {
    emg_h_ = h;
    emg_tau_ = tau;
    emg_mu_ = mu;
    emg_sigma_ = sigma;
  }

  void EMGModel::setH(const double& h)
  {
    emg_h_ = h;
  }
  double EMGModel::getH() const
  {
    return emg_h_;
  }

  void EMGModel::setTau(const double& tau)
  {
    emg_tau_ = tau;
  }
  double EMGModel::getTau() const
  {
    return emg_tau_;
  }

  void EMGModel::setMu(const double& mu)
  {
    emg_mu_ = mu;
  }
  double EMGModel::getMu() const
  {
    return emg_mu_;
  }

  void EMGModel::setSigma(const double& sigma)
  {
    emg_sigma_ = sigma;
  }
  double EMGModel::getSigma() const
  {
    return emg_sigma_;
  }

  double EMGModel::z_(const double& x_I) const
  {
    double z = 1/std::sqrt(2)*(emg_sigma_/emg_tau_ - (x_I-emg_mu_)/emg_sigma_);
    return z;
  }

  double EMGModel::EMGPDF1_(const double& x_I) const
  {
    const double PI = 3.141592653589793;
    const double term1a = emg_h_*emg_sigma_/emg_tau_*std::sqrt(PI/2);
    const double term2a = 0.5*std::pow(emg_sigma_/emg_tau_, 2) - (x_I-emg_mu_)/emg_tau_;
    const double term3a = 1/sqrt(2)*(emg_sigma_/emg_tau_ - (x_I-emg_mu_)/emg_sigma_);
    const double y = term1a*std::exp(term2a)*std::erfc(term3a);
    return y;
  }

  double EMGModel::EMGPDF2_(const double& x_I) const
  {
    const double PI = 3.141592653589793;
    const double term1a = emg_h_*emg_sigma_/emg_tau_*std::sqrt(PI/2);
    const double term2b = -0.5*std::pow((x_I-emg_mu_)/emg_sigma_, 2);
    const double term3a = 1/sqrt(2)*(emg_sigma_/emg_tau_ - (x_I-emg_mu_)/emg_sigma_);
    const double y = term1a*std::exp(term2b)*std::exp(std::pow(term3a, 2))*std::erfc(term3a);
    return y;
  }

  double EMGModel::EMGPDF3_(const double& x_I) const
  {
    const double term1b = emg_h_;
    const double term2b = -0.5*std::pow((x_I-emg_mu_)/emg_sigma_, 2);
    const double term3b = 1 - (x_I-emg_mu_)*emg_tau_/std::pow(emg_sigma_, 2);
    const double y = term1b*std::exp(term2b)/term3b;
    return y;
  }

  double EMGModel::PDF(const double& x_I) const
  {
    const double z =  z_(x_I);
    double y = 0.0;
    if (z < 0)
    {
      y = EMGPDF1_(x_I);
    }
    else if (z >= 0 && z <= 6.71e7)
    {
      y = EMGPDF2_(x_I);
    }
    else if (z > 6.71e7)
    {
      y = EMGPDF3_(x_I);
    }
    return y;
  }
}