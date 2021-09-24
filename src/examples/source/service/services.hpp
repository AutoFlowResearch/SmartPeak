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

#include "workflow_grpc.h"
#include <grpcpp/grpcpp.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/health_check_service_interface.h>

// client-side
class WorkflowClientS {
 public:
  WorkflowClientS(std::shared_ptr<grpc::Channel> channel)
      : stub_(SmartPeakServer::Workflow::NewStub(channel)) {}

  std::string runWorkflow(const std::string& sequence_file_path) {

  SmartPeakServer::WorkflowParameters workflow_parameters;
  workflow_parameters.set_sequence_file(sequence_file_path);
  SmartPeakServer::WorkflowStatus workflow_status;

  grpc::ClientContext context;
    context.AddMetadata("smartpeak_version", SmartPeak::Utilities::getSmartPeakVersion());
  grpc::Status status = stub_->runWorkflow(&context, workflow_parameters, &workflow_status);

  if (status.ok()) {
    return workflow_status.status_code();
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return "RPC failed";
  }
}

 private:
  std::unique_ptr<SmartPeakServer::Workflow::Stub> stub_;
};

class WorkflowClientA {
 public:
  WorkflowClientA(std::shared_ptr<grpc::Channel> channel)
      : stub_(SmartPeakServer::Workflow::NewStub(channel)) {}

  std::string runWorkflow(const std::string& sequence_file_path) {
    SmartPeakServer::WorkflowParameters workflow_parameters; // request
    workflow_parameters.set_sequence_file(sequence_file_path);
    SmartPeakServer::WorkflowStatus workflow_status; // reply

    grpc::ClientContext context;
    grpc::CompletionQueue cq;
    grpc::Status status;

    context.AddMetadata("smartpeak_version", SmartPeak::Utilities::getSmartPeakVersion());

    std::unique_ptr<grpc::ClientAsyncResponseReader<SmartPeakServer::WorkflowStatus> > rpc(
            stub_->PrepareAsyncrunWorkflow(&context, workflow_parameters, &cq));

    rpc->StartCall();

    rpc->Finish(&workflow_status, &status, (void*)1);
    void* got_tag;
    bool ok = false;

    GPR_ASSERT(cq.Next(&got_tag, &ok));
    GPR_ASSERT(got_tag == (void*)1);
    GPR_ASSERT(ok);

  if (status.ok()) {
    return workflow_status.status_code();
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return "RPC failed";
  }
}

 private:
  std::unique_ptr<SmartPeakServer::Workflow::Stub> stub_;
};

class LogStreamClientS {
 public:
  LogStreamClientS(std::shared_ptr<grpc::Channel> channel)
      : stub_(SmartPeakServer::Workflow::NewStub(channel)) {}

  void getLogstream() {
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
      std::cout << "GUI >> getLogStream-rpc succeeded." << std::endl;
    } else {
      std::cout << "GUI >> getLogStream-rpc failed." << std::endl;
    }
  }

 private:
  std::unique_ptr<SmartPeakServer::Workflow::Stub> stub_;
};


class LogStreamClientA {
 public:
  LogStreamClientA(std::shared_ptr<grpc::Channel> channel)
      : stub_(SmartPeakServer::Workflow::NewStub(channel)) {}

  void getLogstream() {
    SmartPeakServer::InquireLogs inquire_logs;
    SmartPeakServer::LogStream log_stream;
    grpc::ClientContext context;
    grpc::CompletionQueue cq;
    grpc::Status status;

    inquire_logs.set_nr_lines(500);

    AsyncClientCall* call = new AsyncClientCall;
    
    call->response_reader = stub_->AsyncgetLogStream(&call->context, inquire_logs, &cq_, (void *)call);
  }

  void AsyncCompleteRpc() {
    void* got_tag;
    bool ok = false;

    while (cq_.Next(&got_tag, &ok)) {
      ResponseHandler* responseHandler = static_cast<ResponseHandler*>(got_tag);
      std::cout << "Tag received: " << responseHandler << std::endl;

      std::cout << "Next returned: " << ok << std::endl;
      responseHandler->HandleResponse(ok);
    }
  }

 private:

  class ResponseHandler {
    public:
      virtual bool HandleResponse(bool eventStatus) = 0;
    };

  class AsyncClientCall: public ResponseHandler {
//    enum CallStatus {CREATE, PROCESS, PROCESSED, FINISH};
    enum CallStatus {CREATE, PROCESS, FINISH};
    CallStatus callStatus_;
  public:

    AsyncClientCall(): callStatus_(CREATE) {}

    SmartPeakServer::LogStream reply;
    grpc::ClientContext context;
    grpc::Status status;
    
    std::unique_ptr<grpc::ClientAsyncReaderInterface<SmartPeakServer::LogStream>> response_reader;

    bool HandleResponse(bool responseStatus) override {
      switch (callStatus_) {
      case CREATE:
        if (responseStatus) {
          response_reader->Read(&reply, (void*)this);
          callStatus_ = PROCESS;
        } else {
          response_reader->Finish(&status, (void*)this);
          callStatus_ = FINISH;
        }
        break;
      case PROCESS:
        if (responseStatus) {
          std::cout << ">>> GUI received : " << this << " : " << reply.log_line() << std::endl;
          response_reader->Read(&reply, (void*)this);
        } else {
          response_reader->Finish(&status, (void*)this);
          callStatus_ = FINISH;
        }
        break;
      case FINISH:
        if (status.ok()) {
          std::cout << ">>> GUI : Server Response Completed: " << this << " CallData: " << this << std::endl;
        } else {
          std::cout << ">>> GUI : RPC failed" << std::endl;
        }
        delete this;
      }
    }
  };

  std::unique_ptr<SmartPeakServer::Workflow::Stub> stub_;
  grpc::CompletionQueue cq_;
};

// server-side
class WorkflowService final : public SmartPeakServer::Workflow::Service {
public:
  
  explicit WorkflowService() : is_logger_init_(false) {
    auto [logfilepath, logdir_created] = SmartPeak::Utilities::getLogFilepath("smartpeak_log");
    console_handler_ = &SmartPeak::ConsoleHandler::get_instance();
    console_handler_->set_log_directory(std::filesystem::path(logfilepath).parent_path().string());
    console_handler_->use_colors(false);
    console_handler_->set_severity(plog::debug);
    console_handler_->initialize("Start SmartPeak version " + SmartPeak::Utilities::getSmartPeakVersion());
    is_logger_init_ = true;
  }
  
  virtual ::grpc::Status runWorkflow(::grpc::ServerContext* context,
                                     const ::SmartPeakServer::WorkflowParameters* request,
                                     ::SmartPeakServer::WorkflowStatus* response) override {

    std::string sequence_file = "";
    sequence_file = request->sequence_file();
    auto [job_done, log_records] = SmartPeak::serv::handleWorkflowRequest(sequence_file, is_logger_init_);
    response->set_status_code( job_done ? "YES" : "NO" );
    return grpc::Status::OK;
  }
  
  ::grpc::Status getLogStream(::grpc::ServerContext* context,
                              const ::SmartPeakServer::InquireLogs* request,
                              ::grpc::ServerWriter< ::SmartPeakServer::LogStream>* writer) override {
    
    auto logstream = console_handler_->gui_appender_.getAppenderRecordList(plog::debug);
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
  SmartPeak::ConsoleHandler* console_handler_;
};

void runSyncServer(std::string server_address) {
  WorkflowService workflow_service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  grpc::ServerBuilder builder;
  
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&workflow_service);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << ">> SERVER listening on : " << server_address << std::endl;

  server->Wait();
}

struct CallData {
  SmartPeakServer::Workflow::AsyncService* service;
  grpc::ServerCompletionQueue* cq;
  SmartPeak::ConsoleHandler* console_handler_;
  std::vector<SmartPeak::GuiAppender::GuiAppenderRecord> log_records;
  
  CallData(SmartPeakServer::Workflow::AsyncService* service, grpc::ServerCompletionQueue* cq) : service(service), cq(cq) {
    auto [logfilepath, logdir_created] = SmartPeak::Utilities::getLogFilepath("smartpeak_log");
    console_handler_ = &SmartPeak::ConsoleHandler::get_instance();
    console_handler_->set_log_directory(std::filesystem::path(logfilepath).parent_path().string());
    console_handler_->use_colors(false);
    console_handler_->set_severity(plog::debug);
    console_handler_->initialize("Start SmartPeak version " + SmartPeak::Utilities::getSmartPeakVersion());
  }
};

class Call {
 public:
  virtual void Proceed(bool ok) = 0;
};

class RunWorkflowCall final : public Call {
 public:
  explicit RunWorkflowCall(CallData* data) :
    data_(data), responder_(&ctx_), status_(REQUEST), is_logger_init(false)
  {
    data->service->RequestrunWorkflow(&ctx_, &request_, &responder_, data_->cq, data_->cq, this);
  }

  void Proceed(bool ok) {
    std::string value;
    int shard;

    switch (status_) {
      case REQUEST:
      {
        new RunWorkflowCall(data_);
        if (!ok) {
          std::cerr << "RunWorkflowCall has failed. Finishing." << std::endl;
          responder_.FinishWithError(grpc::Status::CANCELLED, this);
          status_ = FINISH;
          break;
        }
          
        std::string sequence_file = "";
        sequence_file = request_.sequence_file();
        auto [job_done, log_records] = SmartPeak::serv::handleWorkflowRequest(sequence_file, is_logger_init);
        data_->log_records = log_records;
        response_.set_status_code(job_done?"OK":"FAILED");
        responder_.Finish(response_, grpc::Status::OK, this);
        status_ = FINISH;
        break;
      }

      case FINISH:
      {
        if (!ok) {
          std::cerr << "RunWorkflowCall finished unexpectedly" << std::endl;
          delete this;
          break;
        }
      }
    }
  }

 private:
  CallData* data_;
  grpc::ServerContext ctx_;
  grpc::ServerAsyncResponseWriter<::SmartPeakServer::WorkflowStatus> responder_;
  ::SmartPeakServer::WorkflowParameters request_;
  ::SmartPeakServer::WorkflowStatus response_;
  enum CallStatus { REQUEST, FINISH };
  CallStatus status_;
  bool is_logger_init;
};

class GetLogStreamCall final : public Call {
 public:
  explicit GetLogStreamCall(CallData* data) :
    data_(data), responder_(&ctx_), status_(REQUEST)
  {
    data_->service->RequestgetLogStream(&ctx_, &request_, &responder_, data_->cq, data_->cq, (void*)this);
  }

  void Proceed(bool ok) {
    switch (status_) {
      case REQUEST:
      {
        new GetLogStreamCall(data_);
        if (!ok) {
          std::cerr << "GetLogStreamCall has failed. Finishing." << std::endl;
          responder_.Finish(grpc::Status::CANCELLED, this);
          status_ = FINISH;
          break;
        }

        //auto logstream = data_->log_records;
        auto logstream = data_->console_handler_->gui_appender_.getAppenderRecordList(plog::debug);
        auto nr_lines = request_.nr_lines();
        int message_list_start = (logstream.size() > nr_lines) ? logstream.size() - nr_lines : 0;
        for (int i = message_list_start; i < logstream.size(); ++i)
        {
          std::string logline_str(logstream.at(i).second.data(), logstream.at(i).second.data() + logstream.at(i).second.size());
          response_.Clear();
          response_.set_log_line(logline_str);
          responder_.Write(response_, this);
        }

        responder_.Finish(grpc::Status::OK, this);
        status_ = FINISH;
        break;
      }
        
      case FINISH:
      {
        if (!ok)
          std::cerr << "GetLogStreamCall finished unexpectedly" << std::endl;
        delete this;
        break;
      }
    }
  }

 private:
  CallData* data_;
  grpc::ServerContext ctx_;
  grpc::ServerAsyncWriter<::SmartPeakServer::LogStream> responder_;
  ::SmartPeakServer::InquireLogs request_;
  ::SmartPeakServer::LogStream response_;
  enum CallStatus { REQUEST, FINISH };
  CallStatus status_;
};

class AsyncServer final {
 public:
  AsyncServer(const std::string& listening_address) {}
  ~AsyncServer()  {
    server_->Shutdown();
    cq_->Shutdown();
  }

  void init(const std::string& listening_address,
                         const std::string& hostname) {
    grpc::ServerBuilder builder;
    int selected_port;
    builder.AddListeningPort(listening_address, grpc::InsecureServerCredentials(),
                             &selected_port);
    builder.RegisterService(&service_);
    cq_ = builder.AddCompletionQueue();
    server_ = builder.BuildAndStart();
    if (selected_port == 0) {
      std::cerr << "Could not bind to given port" << std::endl;
      exit(EXIT_FAILURE);
    }
    std::string port = std::to_string(selected_port);
    std::string node_address = hostname + ":" + port;
    std::cerr << "Async SmartPeak Server listening on port " << port << std::endl;
  }
  
  void run() {
    CallData data{&service_, cq_.get()};
    new RunWorkflowCall(&data);
    new GetLogStreamCall(&data);
    void* tag;
    bool ok;
    for (;;) {
      if (!cq_->Next(&tag, &ok)) {
        std::cerr << "Shutting down..." << std::endl;
        break;
      }
      static_cast<Call*>(tag)->Proceed(ok);
    }
  }

 private:
  SmartPeakServer::Workflow::AsyncService service_;
  std::unique_ptr<grpc::Server> server_;
  std::unique_ptr<grpc::ServerCompletionQueue> cq_;
  void* call_ = nullptr;
  std::thread watcher_;
};
