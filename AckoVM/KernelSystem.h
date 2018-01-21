#pragma once

#include <unordered_map>

#include "part.h"
#include "vm_declarations.h"

#include "Process.h"

#define PERIODIC_JOB_TIME (150)

class KernelSystem {
 public:
  KernelSystem(PhysicalAddress processVMSpace, PageNum processVMSpaceSize,
               PhysicalAddress pmtSpace, PageNum pmtSpaceSize,
               Partition* partition);
  ~KernelSystem();
  Process* createProcess();

  Time periodicJob();
  // Hardware job
  Status access(ProcessId pid, VirtualAddress address, AccessType type);

 private:
  ProcessId lastGeneratedPID;
  std::unordered_map<ProcessId, Process*> activeProcesses;
};
