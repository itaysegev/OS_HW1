#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <vector>
#include <string.h>
#include <map>
#include <list>
#include <unistd.h>
#include <fstream>


#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)
#define MAX_CWD_LENGTH 256
#define COMMAND_MAX_LENGTH (80)
#define NO_CURR_PID (-1)
#define NO_CURR_JOB_ID (-1)
#define NOT_SET (-1)
#define ERROR (-1)
#define EXITED (-1)
#define LEN_OF_BG_FG (2)


class TimedCommandEntry {
public:
    time_t alrm_time; 
    std::string timeout_cmd;
    int pid_cmd;
    bool operator< (TimedCommandEntry const& entry2);
    bool operator== (TimedCommandEntry const& entry2);

    TimedCommandEntry() = default;
    TimedCommandEntry(time_t alrm_time, std::string timeout_cmd, int pid_command);
    ~TimedCommandEntry() = default;

    void setTimeoutCmd(const char* cmd_line);
    
};

class Command {
protected:
  const char* cmd;
  int argv;
  int cmd_job_id; //relevant for timeout commands only! used to update the right entry in jobs list

 public:
  char** args; // move back to protected and add relevant get method --chprompt context
  TimedCommandEntry* timed_entry;

  Command(const char* cmd_line);
  virtual ~Command();
  virtual void execute() = 0;
  const char* getCmd();
  int getCmdJobId();
  void cleanCmdJobId(); // safety measure 
  
  //virtual void prepare();
  //virtual void cleanup();
};

class BuiltInCommand : public Command {
 public:
  BuiltInCommand(const char* cmd_line);
  virtual ~BuiltInCommand() {}
};

class JobsList;

class ExternalCommand : public Command {
  JobsList* jobs;
 public:
  ExternalCommand(const char* cmd_line,JobsList* jobs);
  virtual ~ExternalCommand() {}
  void execute() override;
};

class PipeCommand : public Command {
  std::string first_command;
  std::string second_command;
  bool is_stderr;
 public:
  PipeCommand(const char* cmd_line);
  virtual ~PipeCommand() {}
  void execute() override;
};

class RedirectionCommand : public Command {
  std::string command_cmd;
  std::string file_name;
  bool append;
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
 public:
  bool cd_succeeded;
  char* classPlastPwd;
  ChangeDirCommand(const char* cmd_line, char** plastPwd);
  virtual ~ChangeDirCommand();
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
  JobsList* jobs;
 public:
  QuitCommand(const char* cmd_line, JobsList* jobs);
  virtual ~QuitCommand() { jobs = NULL; }
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
      std::string cmd;
      JobEntry()=default;
      JobEntry(int pid, int job_id,JobStatus status,time_t insert,std::string cmd);
  };
  bool jobs_list_empty;
  int max_id;
  std::map<int, JobEntry> jobsDict;
 
 public:
  JobsList();
  ~JobsList() = default;
  int addJob(int pid, std::string cmd,bool isStopped=false);
  void printJobsList();
  void killAllJobs();
  void removeFinishedJobs();
  JobEntry * getJobById(int jobId);
  void removeJobById(int jobId);
  JobEntry * getLastJob(int* lastJobId);
  JobEntry *getLastStoppedJob(int *jobId);
  void maxIdUpdate();
  void printKilledJobList();
};

class JobsCommand : public BuiltInCommand {
  JobsList* jobs;
 public:
  JobsCommand(const char* cmd_line, JobsList* jobs);
  virtual ~JobsCommand() {}
  void execute() override;
};

class KillCommand : public BuiltInCommand {
  JobsList* jobs;
 public:
  KillCommand(const char* cmd_line, JobsList* jobs);
  virtual ~KillCommand() {}
  void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
    JobsList* jobs;
 public:
  ForegroundCommand(const char* cmd_line, JobsList* jobs);
  virtual ~ForegroundCommand() {}
  void execute() override;
  JobsList::JobEntry* setCurrJobToForeground();
};

class BackgroundCommand : public BuiltInCommand {
    JobsList* jobs;
 public:
  BackgroundCommand(const char* cmd_line, JobsList* jobs);
  virtual ~BackgroundCommand() {}
  void execute() override;
  JobsList::JobEntry* setCurrJobToBackground();
};

class CatCommand : public BuiltInCommand {
 public:
  CatCommand(const char* cmd_line);
  virtual ~CatCommand() {}
  void execute() override;
};

class HeadCommand : public BuiltInCommand {
public:
    HeadCommand(const char* cmd_line);
    virtual ~HeadCommand() {}
    void execute() override;
    int setLinesNum();
};

class SmallShell {
  SmallShell();
  char** plastPwd;
  bool legal_cd_made_before;
  std::string prompt;
  JobsList job_list;

  int curr_pid;
  std::string curr_cmd;
  bool curr_fg_from_jobs;
  int curr_fg_from_jobs_id;

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
  void setPLastPwd(Command* cmd);
  void setCurrPid(int curr_pid);
  int getCurrPid();
  void setCurrCmd(std::string curr_cmd);
  void setCurrFgFromJobs(int curr_fg_from_jobs_id);
  int getCurrFgFromJobsListId();
  bool CurrFgIsFromJobsList();
  std::string getCurrCmd();
  void resetCurrFgInfo();
  JobsList* getJobs();
  std::string* getPPrompt()
  {
      return &prompt;
  }
  std::list<TimedCommandEntry> timed_list;

};

#endif //SMASH_COMMAND_H_