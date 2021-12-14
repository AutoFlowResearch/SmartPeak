// --------------------------------------------------------------------------
//   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
// --------------------------------------------------------------------------
// Copyright The SmartPeak Team -- Novo Nordisk Foundation
// Center for Biosustainability, Technical University of Denmark 2018-2022.
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
// $Maintainer: Ahmed Khalil $
// $Authors: Ahmed Khalil $
// --------------------------------------------------------------------------

#include <SmartPeak/core/Server.h>
#include <SmartPeak/core/ConsoleHandler.h>
#include <SmartPeak/core/Utilities.h>

#include "workflow_grpc.h"
#include <grpcpp/grpcpp.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/health_check_service_interface.h>

// client-side
class WorkflowClient
{
public:
  WorkflowClient() {}
  
  void setChannel(std::shared_ptr<grpc::Channel> channel)
  {
    stub_ = SmartPeakServer::Workflow::NewStub(channel);
    channel_set = true;
  }
  
  bool isChannelSet()
  {
    return channel_set;
  }

  std::string runWorkflow(const std::string& dataset_path, const std::string& username, const std::string& password)
  {
    SmartPeakServer::WorkflowParameters workflow_parameters;
    workflow_parameters.set_dataset_path(dataset_path);
    workflow_parameters.set_export_(SmartPeakServer::WorkflowParameters_ExportReport_ALL);
    SmartPeakServer::WorkflowResult workflow_status;
    grpc::ClientContext context;
    
    context.AddMetadata("smartpeak_version", SmartPeak::Utilities::getSmartPeakVersion());
    context.AddMetadata("id", username);
    context.AddMetadata("password", password);
    
    grpc::Status status = stub_->runWorkflow(&context, workflow_parameters, &workflow_status);
    
    if (status.ok()) {
      return workflow_status.status_code();
    } else if (status.error_code() == grpc::StatusCode::UNAUTHENTICATED) {
      LOGE << "Authentication Failed : " << status.error_code() << ":" << status.error_message() << std::endl;
      return std::string("UNAUTHORIZED");
    } else {
      LOGE << status.error_code() << ": " << status.error_message() << std::endl;
      return "RPC failed";
    }
  }
  
  void getLogstream()
  {
    SmartPeakServer::InquireLogs inquire_logs;
    SmartPeakServer::LogStream log_stream;
    grpc::ClientContext context;

    std::unique_ptr<grpc::ClientReader<SmartPeakServer::LogStream> > reader(
        stub_->getLogStream(&context, inquire_logs)  );
    while (reader->Read(&log_stream)) {
      if (log_stream.log_severity() == ::SmartPeakServer::LogStream_LogSeverity_NONE) {
        LOGN << "SERVER LOG : " << log_stream.log_line();
      }
      if (log_stream.log_severity() == ::SmartPeakServer::LogStream_LogSeverity_FATAL) {
        LOGF << "SERVER LOG : " << log_stream.log_line();
      }
      if (log_stream.log_severity() == ::SmartPeakServer::LogStream_LogSeverity_ERROR) {
        LOGE << "SERVER LOG : " << log_stream.log_line();
      }
      if (log_stream.log_severity() == ::SmartPeakServer::LogStream_LogSeverity_WARNING) {
        LOGW << "SERVER LOG : " << log_stream.log_line();
      }
      if (log_stream.log_severity() == ::SmartPeakServer::LogStream_LogSeverity_INFO) {
        LOGI << "SERVER LOG : " << log_stream.log_line();
      }
      if (log_stream.log_severity() == ::SmartPeakServer::LogStream_LogSeverity_DEBUG) {
        LOGD << "SERVER LOG : " << log_stream.log_line();
      }
      if (log_stream.log_severity() == ::SmartPeakServer::LogStream_LogSeverity_VERBOSE) {
        LOGV << "SERVER LOG : " << log_stream.log_line();
      }
    }
    grpc::Status status = reader->Finish();
  }
  
  std::string getProgressInfo()
  {
    SmartPeakServer::WorkflowParameters workflow_parameters;
    SmartPeakServer::ProgressInfo progress_info;
    grpc::ClientContext context;

    grpc::Status status = stub_->getProgressInfo(&context, workflow_parameters, &progress_info);
    if (status.ok()) {
      return progress_info.status_code();
    } else {
      LOGE << status.error_code() << ": " << status.error_message() << std::endl;
      return "RPC failed";
    }
  }

  void getEvent(SmartPeak::IApplicationProcessorObserver* application_observer,
                SmartPeak::ISampleGroupProcessorObserver* sample_group_observer,
                SmartPeak::ISequenceProcessorObserver* sequence_processor_observer,
                SmartPeak::ISequenceSegmentProcessorObserver* sequence_segment_processor_observer,
                SmartPeak::ISequenceObserver* sequence_observer,
                SmartPeak::ITransitionsObserver* transition_observer)
  {
    SmartPeakServer::WorkflowParameters workflow_parameters;
    SmartPeakServer::WorkflowEvent workflow_event;

    grpc::ClientContext context;

    grpc::Status status = stub_->getWorkflowEvent(&context, workflow_parameters, &workflow_event);
    if (status.ok()) {
      /**
      IApplicationProcessorObserver
      */
      if (workflow_event.event_name() == "onApplicationProcessorStart")
      {
        std::vector<std::string> command_list;
        for (const auto command : workflow_event.command_list())
        {
          command_list.push_back(command);
        }
        application_observer->onApplicationProcessorStart(command_list);
      }
       if (workflow_event.event_name() == "onApplicationProcessorCommandStart")
      {
        application_observer->onApplicationProcessorCommandStart(workflow_event.event_index(), workflow_event.item_name());
      }
      if (workflow_event.event_name() == "onApplicationProcessorCommandEnd")
      {
        application_observer->onApplicationProcessorCommandStart(workflow_event.event_index(), workflow_event.item_name());
      }
      if (workflow_event.event_name() == "onApplicationProcessorEnd")
      {
        application_observer->onApplicationProcessorEnd();
      }
      /**
        ISampleGroupProcessorObserver
      */
      if (workflow_event.event_name() == "onSampleGroupProcessorStart")
      {
        sample_group_observer->onSampleGroupProcessorStart(workflow_event.event_index());
      }
      if (workflow_event.event_name() == "onSampleGroupProcessorSampleStart")
      {
        sample_group_observer->onSampleGroupProcessorSampleStart(workflow_event.item_name());
      }
      if (workflow_event.event_name() == "onSampleGroupProcessorSampleEnd")
      {
        sample_group_observer->onSampleGroupProcessorSampleEnd(workflow_event.item_name());
      }
      if (workflow_event.event_name() == "onSampleGroupProcessorEnd")
      {
        sample_group_observer->onSampleGroupProcessorEnd();
      }
      /**
       ISequenceProcessorObserver
      */
      if (workflow_event.event_name() == "onSequenceProcessorStart")
      {
        sequence_processor_observer->onSequenceProcessorStart(workflow_event.event_index());
      }
      if (workflow_event.event_name() == "onSequenceProcessorSampleStart")
      {
        sequence_processor_observer->onSequenceProcessorSampleStart(workflow_event.item_name());
      }
      if (workflow_event.event_name() == "onSequenceProcessorEnd")
      {
        sequence_processor_observer->onSequenceProcessorEnd();
      }
      if (workflow_event.event_name() == "onSequenceProcessorSampleEnd")
      {
        sequence_processor_observer->onSequenceProcessorSampleEnd(workflow_event.item_name());
      }
      /**
        ISequenceSegmentProcessorObserver
      */
      if (workflow_event.event_name() == "onSequenceSegmentProcessorStart")
      {
        sequence_segment_processor_observer->onSequenceSegmentProcessorStart(workflow_event.event_index());
      }
      if (workflow_event.event_name() == "onSequenceSegmentProcessorSampleStart")
      {
        sequence_segment_processor_observer->onSequenceSegmentProcessorSampleStart(workflow_event.item_name());
      }
      if (workflow_event.event_name() == "onSequenceSegmentProcessorEnd")
      {
        sequence_segment_processor_observer->onSequenceSegmentProcessorEnd();
      }
      if (workflow_event.event_name() == "onSequenceSegmentProcessorSampleEnd")
      {
        sequence_segment_processor_observer->onSequenceSegmentProcessorSampleEnd(workflow_event.item_name());
      }
      /**
        ISequenceObserver
      */
      if (workflow_event.event_name() == "onSequenceUpdated")
      {
        sequence_observer->onSequenceUpdated();
      }
      /**
        ITransitionsObserver
      */
      if (workflow_event.event_name() == "onTransitionsUpdated")
      {
        transition_observer->onTransitionsUpdated();
      }
    }
  }
  
  void stopRunningWorkflow()
  {
    SmartPeakServer::Interrupter intr_request;
    SmartPeakServer::Interrupter intr_response;
    grpc::ClientContext context;
    stub_->stopRunningWorkflow(&context, intr_request, &intr_response);
  }

private:
  std::unique_ptr<SmartPeakServer::Workflow::Stub> stub_;
  bool channel_set = false;
};


// server-side
class WorkflowService final : public SmartPeakServer::Workflow::Service
{
  enum class WorkflowStatus {
    IDLE,
    RUNNING
  };
  
public:
  explicit WorkflowService() : is_logger_init_(false)
  {
    auto [logfilepath, logdir_created] = SmartPeak::Utilities::getLogFilepath("smartpeak_log");
    console_handler_ = &SmartPeak::ConsoleHandler::get_instance();
    console_handler_->set_log_directory(std::filesystem::path(logfilepath).parent_path().string());
    console_handler_->use_colors(false);
    console_handler_->set_severity(plog::debug);
    console_handler_->initialize("Starting SmartPeak Server version " + SmartPeak::Utilities::getSmartPeakVersion());
    is_logger_init_ = true;
  }
  
  virtual bool validateCredentials(const std::string& username, const std::string& password)
  {
    sqlite3* users_db = nullptr;
    sqlite3_stmt* query_results = nullptr;
    std::string pwd_sha256;
    bool credentials_matched = false;
    
    std::string db_path;
    SmartPeak::Utilities::getEnvVariable("SMARTPEAK_USERS_DB", &db_path);
    
    if(sqlite3_open(db_path.c_str(), &users_db) != SQLITE_OK) {
      LOGE << "Can't open users database: " << sqlite3_errmsg(users_db);
      sqlite3_close(users_db);
      return credentials_matched;
    }
        
    std::string sql = "SELECT * FROM active_users WHERE username = '" + username + "';";

    if (sqlite3_prepare(users_db, sql.c_str(), sql.size(), &query_results, NULL) != SQLITE_OK)
    {
      LOGE << " Erroneous sql statement:" << sqlite3_errmsg(users_db);
      sqlite3_close(users_db);
      sqlite3_finalize(query_results);
      return credentials_matched;
    }
    else
    {
      switch (sqlite3_step(query_results))
      {
      case SQLITE_ROW:
        {
          pwd_sha256 = (const char*)sqlite3_column_text(query_results, 1);
          if (pwd_sha256 == password) { credentials_matched = true; }
          sqlite3_close(users_db);
          break;
        }
      case SQLITE_DONE:
        break;
      default:
        LOGE << "Error reading database: " << sqlite3_errmsg(users_db);
        break;
      }
    }
    sqlite3_finalize(query_results);
    return credentials_matched;
  }
  
  virtual ::grpc::Status stopRunningWorkflow(::grpc::ServerContext* context,
                                             const ::SmartPeakServer::Interrupter* request,
                                             ::SmartPeakServer::Interrupter* response) override
  {
    workflow_process_interrupted = true;
  }
  
  virtual ::grpc::Status runWorkflow(::grpc::ServerContext* context,
                                     const ::SmartPeakServer::WorkflowParameters* request,
                                     ::SmartPeakServer::WorkflowResult* response) override
  {
    std::string started_at = SmartPeak::Utilities::getCurrentTime();
    session_id_ = SmartPeak::Utilities::makeUniqueStringFromTime();
    server_manager_.dataset_path = request->dataset_path();
    bool export_all;
    if (SmartPeakServer::WorkflowParameters_ExportReport_ALL == request->export_()) export_all = true;
    
    auto client_metadata = context->client_metadata();
    const auto username = client_metadata.find("id");
    const auto password = client_metadata.find("password");

    if (username == client_metadata.end()) {
      LOGW << "Username not provided! Aborting ..";
      return grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "UNAUTHORIZED");
    }
    if (username == client_metadata.end()) {
      LOGW << "Password not provided! Aborting ..";
      return grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "UNAUTHORIZED");
    }
    
    if (validateCredentials(username->second.data(), password->second.data())) {
      LOGI << "Successfully logged in as: " << username->second.data() << " ...";
      if (workflow_status_ == WorkflowStatus::IDLE || workflow_process_interrupted) {
        
        if (workflow_process_interrupted) {
          server_manager_.reset();
        }
        
        workflow_status_ = WorkflowStatus::RUNNING;
        handle_workflow_future_ = std::async(
          std::launch::async,
          &SmartPeak::serv::handleWorkflowRequest,
          &server_manager_,
          export_all
        );
        
        if (workflow_process_interrupted) {
          workflow_process_interrupted = false;
          return grpc::Status(grpc::StatusCode::CANCELLED, "CANCELLED");
        }
        
        if (handle_workflow_future_.valid()) {
          response->set_status_code(handle_workflow_future_.get() ? "YES" : "NO");
          progress_info_.set_status_code(response->status_code());
        }
        
        const auto app_hand = server_manager_.get_application_handler();
        const auto seq_hand = app_hand.sequenceHandler_;
        
        SmartPeak::Utilities::writeToServerSessionFile(
          app_hand.main_dir_.string(),
          username->second.data(), app_hand.main_dir_.string(),
          response->status_code(),
          started_at, SmartPeak::Utilities::getCurrentTime(),
          app_hand.main_dir_.string()+"/"+"exports",
          console_handler_->get_log_filepath());
        
        if (response->status_code() == "NO") {
          return grpc::Status(grpc::StatusCode::ABORTED, "ABORTED");
        }
        
        response->set_session_id(session_id_);
        response->set_path_to_results(app_hand.main_dir_.string()+"/"+"exports");
        server_manager_.reset();
        workflow_status_ = WorkflowStatus::IDLE;
        return grpc::Status::OK;
      } else {
        LOGW << "SERVER BUSY -- NOT PROCESSING : " << server_manager_.dataset_path;
        return grpc::Status(grpc::StatusCode::UNAVAILABLE, "BUSY");
      }
    } else {
      return grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "UNAUTHORIZED");
    }
  }
  
  virtual ::grpc::Status getProgressInfo(::grpc::ServerContext* context,
                                         const ::SmartPeakServer::WorkflowParameters* request,
                                         ::SmartPeakServer::ProgressInfo* response) override
  {
    response->set_status_code(progress_info_.status_code());
    return grpc::Status::OK;
  }

  virtual ::grpc::Status getWorkflowEvent(::grpc::ServerContext* context,
    const ::SmartPeakServer::WorkflowParameters* request,
    ::SmartPeakServer::WorkflowEvent* response) override
  {
    auto &event_dispatcher = server_manager_.get_event_dispatcher();
    event_dispatcher.dispatchEvents();
    auto& server_event_dispatcher_observer = server_manager_.get_server_event_dispatcher_observer();
    if (server_event_dispatcher_observer.events_.empty())
    {
      return grpc::Status::CANCELLED;
    }
    else
    {
      auto [event_name, event_index, item_name, commands_list] = server_event_dispatcher_observer.events_[0];
      server_event_dispatcher_observer.events_.erase(server_event_dispatcher_observer.events_.begin());
      response->set_event_name(event_name);
      response->set_event_index(event_index);
      response->set_item_name(item_name);
      google::protobuf::RepeatedField<std::string> data(commands_list.begin(), commands_list.end());
      response->mutable_command_list()->Add(commands_list.begin(), commands_list.end());
    }
    return grpc::Status::OK;
  }
  
  ::grpc::Status getLogStream(::grpc::ServerContext* context,
                              const ::SmartPeakServer::InquireLogs* request,
                              ::grpc::ServerWriter<::SmartPeakServer::LogStream>* writer) override
  {
    auto logstream = console_handler_->server_appender_.getAppenderRecordList(plog::verbose);
    for (int i = 0; i < logstream.size(); ++i)
    {
      std::string logline_str(logstream.at(i).second.data(), logstream.at(i).second.data() + logstream.at(i).second.size());
      SmartPeakServer::LogStream logline;
      logline.set_log_line(logline_str);
      
      if (logstream.at(i).first == plog::none)
        logline.set_log_severity(::SmartPeakServer::LogStream_LogSeverity_NONE);
      if (logstream.at(i).first == plog::fatal)
        logline.set_log_severity(::SmartPeakServer::LogStream_LogSeverity_FATAL);
      if (logstream.at(i).first == plog::error)
        logline.set_log_severity(::SmartPeakServer::LogStream_LogSeverity_ERROR);
      if (logstream.at(i).first == plog::warning)
        logline.set_log_severity(::SmartPeakServer::LogStream_LogSeverity_WARNING);
      if (logstream.at(i).first == plog::info)
        logline.set_log_severity(::SmartPeakServer::LogStream_LogSeverity_INFO);
      if (logstream.at(i).first == plog::debug)
        logline.set_log_severity(::SmartPeakServer::LogStream_LogSeverity_DEBUG);
      if (logstream.at(i).first == plog::verbose)
        logline.set_log_severity(::SmartPeakServer::LogStream_LogSeverity_VERBOSE);
      
      writer->Write(logline);
    }
    return grpc::Status::OK;
  }
  
private:
  std::unique_ptr<grpc::ServerCompletionQueue> cq_;
  SmartPeakServer::Workflow::AsyncService service_;
  std::unique_ptr<grpc::Server> server_;
  bool is_logger_init_;
  std::string session_id_ = "";
  SmartPeak::ConsoleHandler* console_handler_;
  SmartPeak::serv::ServerManager server_manager_;
  std::future<bool> handle_workflow_future_;
  ::SmartPeakServer::ProgressInfo progress_info_;
  WorkflowStatus workflow_status_ = WorkflowStatus::IDLE;
  bool workflow_process_interrupted = false;
};

void runSmartPeakServer(std::string server_address) {
  WorkflowService workflow_service;
  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&workflow_service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  LOGI << ">> SmartPeak Server listening on : " << server_address << std::endl;
  server->Wait();
}
