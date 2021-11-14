#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <vector>

#include <string.h> //moved it from cpp file


#include <map>

#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)
#define MAX_CWD_LENGTH 256
#define COMMAND_MAX_LENGTH (80)

class Command {
// TODO: Add your data members
protected:
  const char* cmd;
  int argv;

 public:
  char** args; // move back to protected and add relevant get method --chprompt context
  Command(const char* cmd_line);
  virtual ~Command();
  virtual void execute() = 0;
  const char* getCmd();
  //virtual void prepare();
  //virtual void cleanup();
  // TODO: Add your extra methods if needed
};

class BuiltInCommand : public Command {
 public:
  BuiltInCommand(const char* cmd_line);
  virtual ~BuiltInCommand() {}
};

class ExternalCommand : public Command {
  JobsList* jobs;
 public:
  ExternalCommand(const char* cmd_line,JobsList* jobs);
  virtual ~ExternalCommand() {}
  void execute() override;
};

class PipeCommand : public Command {
  // TODO: Add your data members
 public:
  PipeCommand(const char* cmd_line);
  virtual ~PipeCommand() {}
  void execute() override;
};

class RedirectionCommand : public Command {
 // TODO: Add your data members
 public:
  explicit RedirectionCommand(const char* cmd_line);
  virtual ~RedirectionCommand() {}
  void execute() override;
  //void prepare() override;
  //void cleanup() override;
};

class ChangePromptCommand : public BuiltInCommand {
    std::string* prompt;
public:
    ChangePromptCommand(const char* cmd_line, std::string* prompt);
    virtual ~ChangePromptCommand() {}
    void execute() override;
};

class ChangeDirCommand : public BuiltInCommand {
// TODO: Add your data members public:
 public:
  bool cd_succeeded;
  char* classPlastPwd;
  ChangeDirCommand(const char* cmd_line, char** plastPwd);
  virtual ~ChangeDirCommand() {} // ============ implement this destructor??
  void execute() override;

};

class GetCurrDirCommand : public BuiltInCommand {
 public:
  GetCurrDirCommand(const char* cmd_line);
  virtual ~GetCurrDirCommand() {}
  void execute() override;
};

class ShowPidCommand : public BuiltInCommand {
 public:
  ShowPidCommand(const char* cmd_line);
  virtual ~ShowPidCommand() {}
  void execute() override;
};

class JobsList;
class QuitCommand : public BuiltInCommand {
// TODO: Add your data members public:
  QuitCommand(const char* cmd_line, JobsList* jobs);
  virtual ~QuitCommand() {}
  void execute() override;
};


enum JobStatus {Background, Stopped};

class JobsList {
 public:
  class JobEntry {
    public:
      int pid;
      int job_id;
      JobStatus status;
      time_t insert;
      const char* cmd;
      JobEntry()=default;
      JobEntry(int pid, int job_id,JobStatus status,time_t insert,const char* cmd);
   // TODO: Add your data members
  };
  int max_job_id;
  std::map<int, JobEntry> jobsDict;
 // TODO: Add your data members

 
 public:
  JobsList();
  ~JobsList() = default;
  void addJob(Command* cmd, bool isStopped = false);
  void printJobsList();
  void killAllJobs();
  void removeFinishedJobs();
  JobEntry * getJobById(int jobId);
  void removeJobById(int jobId);
  JobEntry * getLastJob(int* lastJobId);
  JobEntry *getLastStoppedJob(int *jobId);
  // TODO: Add extra methods or modify exisitng ones as needed
  void maxIdUpdate();
};

class JobsCommand : public BuiltInCommand {
 // TODO: Add your data members
  JobsList* jobs;
 public:
  JobsCommand(const char* cmd_line, JobsList* jobs);
  virtual ~JobsCommand() {}
  void execute() override;
};

class KillCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  KillCommand(const char* cmd_line, JobsList* jobs);
  virtual ~KillCommand() {}
  void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  ForegroundCommand(const char* cmd_line, JobsList* jobs);
  virtual ~ForegroundCommand() {}
  void execute() override;
};

class BackgroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  BackgroundCommand(const char* cmd_line, JobsList* jobs);
  virtual ~BackgroundCommand() {}
  void execute() override;
};

class CatCommand : public BuiltInCommand {
 public:
  CatCommand(const char* cmd_line);
  virtual ~CatCommand() {}
  void execute() override;
};


class SmallShell {
  // TODO: Add your data members
  SmallShell();
  char** plastPwd;
  bool first_legal_cd;
  std::string prompt;
  JobsList job_list;
  

 public:
  Command *CreateCommand(const char* cmd_line);
  SmallShell(SmallShell const&)      = delete; // disable copy ctor
  void operator=(SmallShell const&)  = delete; // disable = operator
  static SmallShell& getInstance() // make SmallShell singleton
  {
    static SmallShell instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }
  ~SmallShell();
  void executeCommand(const char* cmd_line);
  // TODO: add extra methods as needed
  void setPLastPwd(Command* cmd);
  std::string* getPPrompt()
  {
      return &prompt;
  }
};

#endif SMASH_COMMAND_H_