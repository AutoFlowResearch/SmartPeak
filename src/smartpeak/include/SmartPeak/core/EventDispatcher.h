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
// $Maintainer: Bertrand Boudaud $
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------

#pragma once

#include <SmartPeak/iface/IApplicationProcessorObserver.h>
#include <SmartPeak/core/ApplicationProcessorObservable.h>
#include <SmartPeak/iface/ISequenceProcessorObserver.h>
#include <SmartPeak/core/SequenceProcessorObservable.h>
#include <SmartPeak/iface/ISampleGroupProcessorObserver.h>
#include <SmartPeak/core/SampleGroupProcessorObservable.h>
#include <SmartPeak/iface/ISequenceSegmentProcessorObserver.h>
#include <SmartPeak/core/SequenceSegmentProcessorObservable.h>
#include <SmartPeak/iface/ISequenceObserver.h>
#include <SmartPeak/core/SequenceObservable.h>
#include <SmartPeak/iface/ITransitionsObserver.h>
#include <SmartPeak/core/TransitionsObservable.h>
#include <SmartPeak/iface/IFeaturesObserver.h>
#include <SmartPeak/core/FeaturesObservable.h>

#include <plog/Log.h>
#include <future>
#include <thread>
#include <string>
#include <vector>

namespace SmartPeak
{

  /**
    The EventDispatcher can be used to store and dispatch Observer events to another thread.
  */
  struct EventDispatcher : public
    IApplicationProcessorObserver,
    ISequenceProcessorObserver,
    ISequenceSegmentProcessorObserver,
    ISampleGroupProcessorObserver,
    ISequenceObserver,
    ITransitionsObserver,
    IFeaturesObserver,
    ApplicationProcessorObservable,
    SequenceProcessorObservable,
    SequenceSegmentProcessorObservable,
    SampleGroupProcessorObservable,
    SequenceObservable,
    TransitionsObservable,
    FeaturesObservable
  {
    EventDispatcher() = default;

    /**
      IApplicationProcessorObserver
    */
    virtual void onApplicationProcessorStart(const std::vector<std::string>& commands) override;
    virtual void onApplicationProcessorCommandStart(size_t command_index, const std::string& command_name) override;
    virtual void onApplicationProcessorCommandEnd(size_t command_index, const std::string& command_name) override;
    virtual void onApplicationProcessorEnd() override;
    virtual void onApplicationProcessorError(const std::string& error) override;
    
    /**
      IFeaturesObserver
    */
    virtual void onFeaturesUpdated() override;

    /**
      ISequenceSegmentProcessorObserver
    */
    virtual void onSequenceProcessorStart(const size_t nb_injections) override;
    virtual void onSequenceProcessorSampleStart(const std::string& sample_name) override;
    virtual void onSequenceProcessorSampleEnd(const std::string& sample_name) override;
    virtual void onSequenceProcessorEnd() override;
    virtual void onSequenceProcessorError(const std::string& sample_name, const std::string& processor_name, const std::string& error) override;
    virtual void onSequenceSegmentProcessorStart(const size_t nb_segments) override;
    virtual void onSequenceSegmentProcessorSampleStart(const std::string& segment_name) override;
    virtual void onSequenceSegmentProcessorSampleEnd(const std::string& segment_name) override;
    virtual void onSequenceSegmentProcessorEnd() override;
    virtual void onSequenceSegmentProcessorError(const std::string& segment_name, const std::string& processor_name, const std::string& error) override;

    /**
      ISampleGroupProcessorObserver
    */
    virtual void onSampleGroupProcessorStart(const size_t nb_groups) override;
    virtual void onSampleGroupProcessorSampleStart(const std::string& group_name) override;
    virtual void onSampleGroupProcessorSampleEnd(const std::string& group_name) override;
    virtual void onSampleGroupProcessorEnd() override;
    virtual void onSampleGroupProcessorError(const std::string& group_name, const std::string& processor_name, const std::string& error) override;

    /**
      ISequenceObserver
    */
    virtual void onSequenceUpdated() override;

    /**
      ITransitionsObserver
    */
    virtual void onTransitionsUpdated() override;

  public:
    /**
      @brief Send stored events to the dispatcher's observers.
    */
    void dispatchEvents();

  private:
    void queueEvent(std::shared_ptr<std::future<void>> event);

  private:
    std::mutex event_queue_mutex_;
    std::vector<std::shared_ptr<std::future<void>>> events_;
  };
}
