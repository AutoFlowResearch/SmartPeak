// --------------------------------------------------------------------------
//   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
// --------------------------------------------------------------------------
// Copyright The SmartPeak Team -- Novo Nordisk Foundation 
// Center for Biosustainability, Technical University of Denmark 2018-2021.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Douglas McCloskey, Ahmed Khalil $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <SmartPeak/test_config.h>

#include <gtest/gtest.h>
#include <SmartPeak/core/ParametersObservable.h>

using namespace SmartPeak;
using namespace std;

struct ParametersObserverTest : public IParametersObserver
{
  virtual void onParametersUpdated() override
  {
    nb_notifications_++;
  }
  int nb_notifications_ = 0;
};

TEST(ParametersObserver, add_remove_observers)
{
  ParametersObserverTest obs1;
  ParametersObserverTest obs2;
  EXPECT_EQ(obs1.nb_notifications_, 0);
  EXPECT_EQ(obs2.nb_notifications_, 0);

  ParametersObservable observable;

  observable.notifyParametersUpdated();
  EXPECT_EQ(obs1.nb_notifications_, 0);
  EXPECT_EQ(obs2.nb_notifications_, 0);

  observable.addParametersObserver(&obs1);
  observable.notifyParametersUpdated();
  EXPECT_EQ(obs1.nb_notifications_, 1);
  EXPECT_EQ(obs2.nb_notifications_, 0);

  observable.addParametersObserver(&obs2);
  observable.notifyParametersUpdated();
  EXPECT_EQ(obs1.nb_notifications_, 2);
  EXPECT_EQ(obs2.nb_notifications_, 1);

  observable.removeParametersObserver(&obs1);
  observable.notifyParametersUpdated();
  EXPECT_EQ(obs1.nb_notifications_, 2);
  EXPECT_EQ(obs2.nb_notifications_, 2);

  observable.removeParametersObserver(&obs2);
  observable.notifyParametersUpdated();
  EXPECT_EQ(obs1.nb_notifications_, 2);
  EXPECT_EQ(obs2.nb_notifications_, 2);

  // remove non registered observer
  observable.removeParametersObserver(&obs1);
  observable.notifyParametersUpdated();
  EXPECT_EQ(obs1.nb_notifications_, 2);
  EXPECT_EQ(obs2.nb_notifications_, 2);
}
