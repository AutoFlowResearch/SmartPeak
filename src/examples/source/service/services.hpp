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
// $Maintainer: Ahmed Khalil $
// $Authors: Ahmed Khalil $
// --------------------------------------------------------------------------

#include <SmartPeak/core/Server.h>
#include <SmartPeak/core/ConsoleHandler.h>

#include "workflow_grpc.h"
#include <grpcpp/grpcpp.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/health_check_service_interface.h>

// client-side
class WorkflowClient
{
public:
  WorkflowClient(std::shared_ptr<grpc::Channel> channel)
      : stub_(SmartPeakServer::Workflow::NewStub(channel)) {}

  std::string runWorkflow(const std::string& dataset_path)
  {
    SmartPeakServer::WorkflowParameters workflow_parameters;
    workflow_parameters.set_dataset_path(dataset_path);
    workflow_parameters.set_export_(SmartPeakServer::WorkflowParameters_ExportReport_ALL);
    SmartPeakServer::WorkflowResult workflow_status;
    grpc::ClientContext context;
    
    context.AddMetadata("smartpeak_version", SmartPeak::Utilities::getSmartPeakVersion());
    grpc::Status status = stub_->runWorkflow(&context, workflow_parameters, &workflow_status);
    
    if (status.ok()) {
      return workflow_status.status_code();
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

    inquire_logs.set_nr_lines(1000);
    std::unique_ptr<grpc::ClientReader<SmartPeakServer::LogStream> > reader(
        stub_->getLogStream(&context, inquire_logs)  );
    while (reader->Read(&log_stream)) {
      LOGI << "SERVER LOG : " << log_stream.log_line();
    }
    grpc::Status status = reader->Finish();
    if (status.ok()) {
      LOGI << "GUI >> getLogStream-rpc succeeded." << std::endl;
    } else {
      LOGW << "GUI >> getLogStream-rpc failed." << std::endl;
    }
  }

private:
  std::unique_ptr<SmartPeakServer::Workflow::Stub> stub_;
};

// server-side
class WorkflowService final : public SmartPeakServer::Workflow::Service
{
public:
  explicit WorkflowService() : is_logger_init_(false), session_id_(SmartPeak::Utilities::makeUniqueStringFromTime())
  {
    auto [logfilepath, logdir_created] = SmartPeak::Utilities::getLogFilepath("smartpeak_log");
    console_handler_ = &SmartPeak::ConsoleHandler::get_instance();
    console_handler_->set_log_directory(std::filesystem::path(logfilepath).parent_path().string());
    console_handler_->use_colors(false);
    console_handler_->set_severity(plog::debug);
    console_handler_->initialize("Starting SmartPeak Server version " + SmartPeak::Utilities::getSmartPeakVersion());
    is_logger_init_ = true;
  }
  
  virtual ::grpc::Status runWorkflow(::grpc::ServerContext* context,
                                     const ::SmartPeakServer::WorkflowParameters* request,
                                     ::SmartPeakServer::WorkflowResult* response) override
  {
    server_manager_.dataset_path = request->dataset_path();
    bool export_all;
    if (SmartPeakServer::WorkflowParameters_ExportReport_ALL == request->export_()) export_all = true;
    auto job_done = SmartPeak::serv::handleWorkflowRequest(&server_manager_, export_all);
    const auto app_hand = server_manager_.get_application_handler();
    const auto seq_hand = app_hand.sequenceHandler_;
    response->set_status_code(job_done ? "YES" : "NO");
    response->set_session_id(session_id_);
    response->set_path_to_results(app_hand.main_dir_.string());
    return grpc::Status::OK;
  }
  
  ::grpc::Status getLogStream(::grpc::ServerContext* context,
                              const ::SmartPeakServer::InquireLogs* request,
                              ::grpc::ServerWriter<::SmartPeakServer::LogStream>* writer) override
  {
    auto logstream = console_handler_->server_appender_.getAppenderRecordList(plog::debug);
    auto nr_lines = request->nr_lines();
    int message_list_start = (logstream.size() > nr_lines) ? logstream.size() - nr_lines : 0;
    for (int i = message_list_start; i < logstream.size(); ++i)
    {
      std::string logline_str(logstream.at(i).second.data(), logstream.at(i).second.data() + logstream.at(i).second.size());
      SmartPeakServer::LogStream logline;
      logline.set_log_line(logline_str);
      writer->Write(logline);
    }
    return grpc::Status::OK;
  }
  
private:
  std::unique_ptr<grpc::ServerCompletionQueue> cq_;
  SmartPeakServer::Workflow::AsyncService service_;
  std::unique_ptr<grpc::Server> server_;
  bool is_logger_init_;
  std::string session_id_;
  SmartPeak::ConsoleHandler* console_handler_;
  SmartPeak::serv::ServerManager server_manager_;
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
