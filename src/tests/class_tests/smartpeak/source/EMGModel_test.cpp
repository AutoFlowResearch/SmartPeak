/**TODO:  Add copyright*/

#define BOOST_TEST_MODULE EMGModel test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/simulator/EMGModel.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(emgmodel)

class EMGModel_test: public EMGModel
{
public:
  double z_(const double& x_I) const
  {
    return EMGModel::z_(x_I);
  }
  double EMGPDF1_(const double& x_I) const
  {
    return EMGModel::EMGPDF1_(x_I);
  }
  double EMGPDF2_(const double& x_I) const
  {
    return EMGModel::EMGPDF2_(x_I);
  }
  double EMGPDF3_(const double& x_I) const
  {
    return EMGModel::EMGPDF3_(x_I);
  }
};

BOOST_AUTO_TEST_CASE(constructor)
{
  EMGModel* ptr = nullptr;
  EMGModel* nullPointer = nullptr;
  ptr = new EMGModel();
  BOOST_CHECK_NE(ptr, nullPointer);
}

BOOST_AUTO_TEST_CASE(destructor)
{
  EMGModel* ptr = nullptr;
  ptr = new EMGModel();
  delete ptr;
}

BOOST_AUTO_TEST_CASE(constructor2)
{
  EMGModel emg(1.0, 2.0, 3.0, 4.0);

  BOOST_CHECK_EQUAL(emg.getH(), 1.0);
  BOOST_CHECK_EQUAL(emg.getTau(), 2.0);
  BOOST_CHECK_EQUAL(emg.getMu(), 3.0);
  BOOST_CHECK_EQUAL(emg.getSigma(), 4.0);
}

BOOST_AUTO_TEST_CASE(gettersAndSetters)
{
  EMGModel emg;
  emg.setH(1.0);
  emg.setTau(2.0);
  emg.setMu(3.0);
  emg.setSigma(4.0);

  BOOST_CHECK_EQUAL(emg.getH(), 1.0);
  BOOST_CHECK_EQUAL(emg.getTau(), 2.0);
  BOOST_CHECK_EQUAL(emg.getMu(), 3.0);
  BOOST_CHECK_EQUAL(emg.getSigma(), 4.0);
}

BOOST_AUTO_TEST_CASE(z_)
{
  EMGModel_test emg;
  emg.setH(1.0);
  emg.setTau(0.1);
  emg.setMu(0.0);
  emg.setSigma(1.0);

  BOOST_CHECK_CLOSE(emg.z_(-100), 77.781745930520216, 1e-6);
  BOOST_CHECK_CLOSE(emg.z_(-10), 14.142135623730949, 1e-6);
  BOOST_CHECK_CLOSE(emg.z_(0), 7.0710678118654746, 1e-6);
  BOOST_CHECK_CLOSE(emg.z_(10), 0.0, 1e-6);
  BOOST_CHECK_CLOSE(emg.z_(100), -63.639610306789272, 1e-6);

  emg.setTau(1e-12);
  BOOST_CHECK_CLOSE(emg.z_(0), 707106781186.54749, 1e-6);
}

BOOST_AUTO_TEST_CASE(emgpdf1_)
{
  EMGModel_test emg;
  emg.setH(1.0);
  emg.setTau(0.1);
  emg.setMu(0.0);
  emg.setSigma(1.0);

  BOOST_CHECK_CLOSE(emg.EMGPDF1_(100), 0.0, 1e-6);
}

BOOST_AUTO_TEST_CASE(emgpdf2_)
{
  EMGModel_test emg;
  emg.setH(1.0);
  emg.setTau(0.1);
  emg.setMu(0.0);
  emg.setSigma(1.0);

  BOOST_CHECK_CLOSE(emg.EMGPDF2_(0), 0.99028596471732067, 1e-6);
}

BOOST_AUTO_TEST_CASE(emgpdf3_)
{
  EMGModel_test emg;
  emg.setH(1.0);
  emg.setTau(1e-12);
  emg.setMu(0.0);
  emg.setSigma(1.0);

  BOOST_CHECK_CLOSE(emg.EMGPDF3_(0), 1.0, 1e-6);
}

BOOST_AUTO_TEST_CASE(emgpdf)
{
  EMGModel emg;
  emg.setH(1.0);
  emg.setTau(0.1);
  emg.setMu(0.0);
  emg.setSigma(1.0);

  BOOST_CHECK_CLOSE(emg.PDF(100), 0.0, 1e-6);
  BOOST_CHECK_CLOSE(emg.PDF(0), 0.99028596471732067, 1e-6);
  emg.setTau(1e-12);
  BOOST_CHECK_CLOSE(emg.PDF(0), 1.0, 1e-6);
}

BOOST_AUTO_TEST_SUITE_END()
