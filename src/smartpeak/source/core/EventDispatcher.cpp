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
// $Maintainer: Bertrand Boudaud, Ahmed Khalil, Douglas McCloskey $
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------

#pragma once

#include <SmartPeak/core/EventDispatcher.h>

#include <plog/Log.h>
#include <future>
#include <thread>
#include <string>
#include <vector>

namespace SmartPeak
{

    /**
      IApplicationProcessorObserver
    */
    void EventDispatcher::onApplicationProcessorStart(const std::vector<std::string>& commands)
    {
      queueEvent(std::make_shared<std::future<void>>(std::async(std::launch::deferred,
        [this, commands] { this->notifyApplicationProcessorStart(commands); }
      )));
    }
    void EventDispatcher::onApplicationProcessorCommandStart(size_t command_index, const std::string& command_name)
    {
      queueEvent(std::make_shared<std::future<void>>(std::async(std::launch::deferred,
        [this, command_index, command_name] { this->notifyApplicationProcessorCommandStart(command_index, command_name); }
      )));
    }
    void EventDispatcher::onApplicationProcessorCommandEnd(size_t command_index, const std::string& command_name)
    {
      queueEvent(std::make_shared<std::future<void>>(std::async(std::launch::deferred,
        [this, command_index, command_name] { this->notifyApplicationProcessorCommandEnd(command_index, command_name); }
      )));
    }
    void EventDispatcher::onApplicationProcessorEnd()
    {
      queueEvent(std::make_shared<std::future<void>>(std::async(std::launch::deferred,
        [this] { this->notifyApplicationProcessorEnd(); }
      )));
    }
    void EventDispatcher::onApplicationProcessorError(const std::string& error)
    {
      queueEvent(std::make_shared<std::future<void>>(std::async(std::launch::deferred,
        [this, error] { this->notifyApplicationProcessorError(error); }
      )));
    }

    /**
      IFeaturesObserver
    */
    void EventDispatcher::onFeaturesUpdated()
    {
      queueEvent(std::make_shared<std::future<void>>(std::async(std::launch::deferred,
        [this] { this->notifyFeaturesUpdated(); }
      )));
    }

    /**
      ISequenceSegmentProcessorObserver
    */
    void EventDispatcher::onSequenceProcessorStart(const size_t nb_injections)
    {
      queueEvent(std::make_shared<std::future<void>>(std::async(std::launch::deferred,
        [this, nb_injections] { this->notifySequenceProcessorStart(nb_injections); }
      )));
    }
    void EventDispatcher::onSequenceProcessorSampleStart(const std::string& sample_name)
    {
      queueEvent(std::make_shared<std::future<void>>(std::async(std::launch::deferred,
        [this, sample_name] { this->notifySequenceProcessorSampleStart(sample_name); }
      )));
    }
    void EventDispatcher::onSequenceProcessorSampleEnd(const std::string& sample_name)
    {
      queueEvent(std::make_shared<std::future<void>>(std::async(std::launch::deferred,
        [this, sample_name] { this->notifySequenceProcessorSampleEnd(sample_name); }
      )));
    }
    void EventDispatcher::onSequenceProcessorEnd()
    {
      queueEvent(std::make_shared<std::future<void>>(std::async(std::launch::deferred,
        [this] { this->notifySequenceProcessorEnd(); }
      )));
    }
    void EventDispatcher::onSequenceProcessorError(const std::string& sample_name, const std::string& processor_name, const std::string& error)
    {
      queueEvent(std::make_shared<std::future<void>>(std::async(std::launch::deferred,
        [this, sample_name, processor_name, error] { this->notifySequenceProcessorError(sample_name, processor_name, error); }
      )));
    }
    void EventDispatcher::onSequenceSegmentProcessorStart(const size_t nb_segments)
    {
      queueEvent(std::make_shared<std::future<void>>(std::async(std::launch::deferred,
        [this, nb_segments] { this->notifySequenceSegmentProcessorStart(nb_segments); }
      )));
    }
    void EventDispatcher::onSequenceSegmentProcessorSampleStart(const std::string& segment_name)
    {
      queueEvent(std::make_shared<std::future<void>>(std::async(std::launch::deferred,
        [this, segment_name] { this->notifySequenceSegmentProcessorSampleStart(segment_name); }
      )));
    }
    void EventDispatcher::onSequenceSegmentProcessorSampleEnd(const std::string& segment_name)
    {
      queueEvent(std::make_shared<std::future<void>>(std::async(std::launch::deferred,
        [this, segment_name] { this->notifySequenceSegmentProcessorSampleEnd(segment_name); }
      )));
    }
    void EventDispatcher::onSequenceSegmentProcessorEnd()
    {
      queueEvent(std::make_shared<std::future<void>>(std::async(std::launch::deferred,
        [this] { this->notifySequenceSegmentProcessorEnd(); }
      )));
    }
    void EventDispatcher::onSequenceSegmentProcessorError(const std::string& segment_name, const std::string& processor_name, const std::string& error)
    {
      queueEvent(std::make_shared<std::future<void>>(std::async(std::launch::deferred,
        [this, segment_name, processor_name, error] { this->notifySequenceSegmentProcessorError(segment_name, processor_name, error); }
      )));
    }

    /**
      ISampleGroupProcessorObserver
    */
    void EventDispatcher::onSampleGroupProcessorStart(const size_t nb_groups)
    {
      queueEvent(std::make_shared<std::future<void>>(std::async(std::launch::deferred,
        [this, nb_groups] { this->notifySampleGroupProcessorStart(nb_groups); }
      )));
    }
    void EventDispatcher::onSampleGroupProcessorSampleStart(const std::string& group_name)
    {
      queueEvent(std::make_shared<std::future<void>>(std::async(std::launch::deferred,
        [this, group_name] { this->notifySampleGroupProcessorSampleStart(group_name); }
      )));
    }
    void EventDispatcher::onSampleGroupProcessorSampleEnd(const std::string& group_name)
    {
      queueEvent(std::make_shared<std::future<void>>(std::async(std::launch::deferred,
        [this, group_name] { this->notifySampleGroupProcessorSampleEnd(group_name); }
      )));
    }
    void EventDispatcher::onSampleGroupProcessorEnd()
    {
      queueEvent(std::make_shared<std::future<void>>(std::async(std::launch::deferred,
        [this] { this->notifySampleGroupProcessorEnd(); }
      )));
    }
    void EventDispatcher::onSampleGroupProcessorError(const std::string& group_name, const std::string& processor_name, const std::string& error)
    {
      queueEvent(std::make_shared<std::future<void>>(std::async(std::launch::deferred,
        [this, group_name, processor_name, error] { this->notifySampleGroupProcessorError(group_name, processor_name, error); }
      )));
    }

    /**
      ISequenceObserver
    */
    void EventDispatcher::onSequenceUpdated()
    {
      queueEvent(std::make_shared<std::future<void>>(std::async(std::launch::deferred,
        [this] { this->notifySequenceUpdated(); }
      )));
    }

    /**
      ITransitionsObserver
    */
    void EventDispatcher::onTransitionsUpdated()
    {
      queueEvent(std::make_shared<std::future<void>>(std::async(std::launch::deferred,
        [this] { this->notifyTransitionsUpdated(); }
      )));
    }

    void EventDispatcher::dispatchEvents()
    {
      decltype(events_) events_to_dispatch;
      {
        // we will dispatch a copy of the events to avoid
        // feeding events while we are dispatching them.
        std::scoped_lock lock(event_queue_mutex_);
        events_to_dispatch.swap(events_);
      }
      for (auto& event : events_to_dispatch)
      {
        event->get();
      }
    }

    void EventDispatcher::queueEvent(std::shared_ptr<std::future<void>> event)
    {
      std::scoped_lock lock(event_queue_mutex_);
      events_.push_back(event);
    }

}
