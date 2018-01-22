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
  this->partition = partition;
  for (int i = 0; i < partition->getNumOfClusters(); i++) {
    freeClusters.emplace_back(i);
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
  if (process->isAddressInColdStorage(address)) {
    std::cout << "KernelSystem::access() virtualAddress: " << address
              << " for pid: " << pid
              << " is in coldStorage, generating PAGE_FAULT" << std::endl;
    return Status::PAGE_FAULT;
  }

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
    if (type == AccessType::EXECUTE) {
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

PhysicalAddress KernelSystem::getFreeSegment(ProcessId pid) {
  if (freeSegments.empty()) {
    // Need to put something in coldStorage
    std::cout
        << "KernelSystem::getFreeSegment() need to put something in coldStorage"
        << std::endl;

    auto clusterNo = freeClusters.front();
    freeClusters.pop_front();

    auto process = activeProcesses[pid];
    auto pmt = process->getPmt();
    auto coldStorage = process->getColdStorage();

    auto it = pmt->begin();
    auto virtualAddress = it->first;
    auto physicalAddress = it->second;
    auto buffer = reinterpret_cast<const char*>(physicalAddress);
    partition->writeCluster(clusterNo, buffer);
    pmt->erase(it);

    (*coldStorage)[virtualAddress] = clusterNo;

    return physicalAddress;
  }
  auto freeSegment = freeSegments.front();
  freeSegments.pop_front();
  return freeSegment;
}

void KernelSystem::releaseSegment(PhysicalAddress segment) {
  freeSegments.emplace_back(segment);
}

Partition* KernelSystem::getPartition() { return this->partition; }

std::list<PhysicalAddress>* KernelSystem::getFreeSegments() {
  return &freeSegments;
}
