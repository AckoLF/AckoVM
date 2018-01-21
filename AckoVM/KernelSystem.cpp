#include "KernelSystem.h"

KernelSystem* KernelSystem::instance = nullptr;

KernelSystem::KernelSystem(PhysicalAddress processVMSpace,
                           PageNum processVMSpaceSize, PhysicalAddress pmtSpace,
                           PageNum pmtSpaceSize, Partition* partition) {
  this->lastGeneratedPID = 0;
}

KernelSystem::~KernelSystem() {
  for (auto it : activeProcesses) {
    auto process = it.second;
    delete process;
  }
}

Process* KernelSystem::createProcess() {
  ProcessId cuurrentPID = ++lastGeneratedPID;
  return new Process(cuurrentPID);
}

Time KernelSystem::periodicJob() { return PERIODIC_JOB_TIME; }

Status KernelSystem::access(ProcessId pid, VirtualAddress address,
                            AccessType type) {
  return Status();
}

KernelSystem* KernelSystem::getInstance(PhysicalAddress processVMSpace,
                                        PageNum processVMSpaceSize,
                                        PhysicalAddress pmtSpace,
                                        PageNum pmtSpaceSize,
                                        Partition* partition) {
  if (instance == nullptr) {
    instance = new KernelSystem(processVMSpace, processVMSpaceSize, pmtSpace,
                                pmtSpaceSize, partition);
  }
  return instance;
}
