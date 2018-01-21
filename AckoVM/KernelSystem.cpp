#include "KernelSystem.h"

#include <cassert>
#include <iostream>

KernelSystem* KernelSystem::instance = nullptr;

KernelSystem::KernelSystem(PhysicalAddress processVMSpace,
                           PageNum processVMSpaceSize, PhysicalAddress pmtSpace,
                           PageNum pmtSpaceSize, Partition* partition) {
  this->lastGeneratedPID = 0;
  auto startAddress = reinterpret_cast<uint64_t>(processVMSpace);
  for (int i = 0; i < processVMSpaceSize; i++) {
    auto currentAddress = startAddress + i * PAGE_SIZE;
    this->freeSegments.emplace_back(
        reinterpret_cast<PhysicalAddress>(currentAddress));
  }
}

KernelSystem::~KernelSystem() {
  for (auto it : activeProcesses) {
    auto process = it.second;
    delete process;
  }
}

Process* KernelSystem::createProcess() {
  ProcessId currentPID = ++lastGeneratedPID;
  auto process = new Process(currentPID);
  activeProcesses[currentPID] = process;
  return process;
}

Time KernelSystem::periodicJob() { return PERIODIC_JOB_TIME; }

Status KernelSystem::access(ProcessId pid, VirtualAddress address,
                            AccessType type) {
  auto it = activeProcesses.find(pid);
  if (it == activeProcesses.end()) {
    std::cout << "KernelSystem::access() pid: " << pid << " is not active"
              << std::endl;
    return Status::TRAP;
  }
  auto process = it->second;
  auto physicalAddress = process->getPhysicalAddress(address);
  if (physicalAddress == 0) {
    std::cout << "KernelSystem::access() virtualAddress: " << address
              << " does not belong to pid: " << pid << std::endl;
    return Status::TRAP;
  }
  auto alignedAddress = address - address % PAGE_SIZE;
  auto segmentAccessPermissions =
      process->getSegmentAccessPermissions(alignedAddress);
  if (segmentAccessPermissions == AccessType::READ_WRITE) {
    if (type != AccessType::READ && type != AccessType::WRITE) {
      return Status::TRAP;
    }
    return Status::OK;
  }
  if (segmentAccessPermissions != type) {
    return Status::TRAP;
  }
  return Status::OK;
}

KernelSystem* KernelSystem::getInstance() {
  assert(instance != nullptr);
  return instance;
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

PhysicalAddress KernelSystem::getFreeSegment() {
  auto freeSegment = freeSegments.front();
  freeSegments.pop_front();
  return freeSegment;
}

void KernelSystem::releaseSegment(PhysicalAddress segment) {
  freeSegments.emplace_back(segment);
}
