#pragma once

#include <list>
#include <unordered_map>

#include "part.h"
#include "vm_declarations.h"

#include "Process.h"

#define PERIODIC_JOB_TIME (150)

class KernelSystem {
 public:
  // KernelSystem is singleton so we delete the copy/move constructors and
  // assignment operators and the default construtor
  KernelSystem() = delete;
  KernelSystem(KernelSystem const&) = delete;
  KernelSystem(KernelSystem&&) = delete;
  void operator=(KernelSystem const&) = delete;
  void operator=(KernelSystem&&) = delete;

  ~KernelSystem();
  Process* createProcess();
  Time periodicJob();
  Status access(ProcessId pid, VirtualAddress address, AccessType type);

  static KernelSystem* getInstance();
  static KernelSystem* getInstance(PhysicalAddress processVMSpace,
                                   PageNum processVMSpaceSize,
                                   PhysicalAddress pmtSpace,
                                   PageNum pmtSpaceSize, Partition* partition);

  PhysicalAddress getFreeSegment(ProcessId pid);
  void releaseSegment(PhysicalAddress segment);
  Partition* getPartition();
  std::list<PhysicalAddress>* getFreeSegments();

 private:
  ProcessId lastGeneratedPID;
  Partition* partition;
  std::list<PhysicalAddress> freeSegments;
  std::list<ClusterNo> freeClusters;
  std::unordered_map<ProcessId, Process*> activeProcesses;
  KernelSystem(PhysicalAddress processVMSpace, PageNum processVMSpaceSize,
               PhysicalAddress pmtSpace, PageNum pmtSpaceSize,
               Partition* partition);
  static KernelSystem* instance;
};
