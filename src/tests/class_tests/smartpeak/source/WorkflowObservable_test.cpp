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
// $Maintainer: Douglas McCloskey $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE WorkflowObservable test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/WorkflowObservable.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(workflowobservable)

struct WorkflowObserverTest : public IWorkflowObserver
{
  virtual void workflowUpdated() override
  {
    nb_notifications_++;
  }
  int nb_notifications_ = 0;
};

BOOST_AUTO_TEST_CASE(add_remove_observers)
{
  WorkflowObserverTest obs1;
  WorkflowObserverTest obs2;
  BOOST_CHECK_EQUAL(obs1.nb_notifications_, 0);
  BOOST_CHECK_EQUAL(obs2.nb_notifications_, 0);

  WorkflowObservable observable;

  observable.notifyWorkflowChanged();
  BOOST_CHECK_EQUAL(obs1.nb_notifications_, 0);
  BOOST_CHECK_EQUAL(obs2.nb_notifications_, 0);

  observable.addWorkflowObserver(&obs1);
  observable.notifyWorkflowChanged();
  BOOST_CHECK_EQUAL(obs1.nb_notifications_, 1);
  BOOST_CHECK_EQUAL(obs2.nb_notifications_, 0);

  observable.addWorkflowObserver(&obs2);
  observable.notifyWorkflowChanged();
  BOOST_CHECK_EQUAL(obs1.nb_notifications_, 2);
  BOOST_CHECK_EQUAL(obs2.nb_notifications_, 1);

  observable.removeWorkflowObserver(&obs1);
  observable.notifyWorkflowChanged();
  BOOST_CHECK_EQUAL(obs1.nb_notifications_, 2);
  BOOST_CHECK_EQUAL(obs2.nb_notifications_, 2);

  observable.removeWorkflowObserver(&obs2);
  observable.notifyWorkflowChanged();
  BOOST_CHECK_EQUAL(obs1.nb_notifications_, 2);
  BOOST_CHECK_EQUAL(obs2.nb_notifications_, 2);

  // remove non registered observer
  observable.removeWorkflowObserver(&obs1);
  observable.notifyWorkflowChanged();
  BOOST_CHECK_EQUAL(obs1.nb_notifications_, 2);
  BOOST_CHECK_EQUAL(obs2.nb_notifications_, 2);
}

BOOST_AUTO_TEST_SUITE_END()