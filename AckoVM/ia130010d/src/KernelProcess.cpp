#include "KernelProcess.h"
#include "KernelSystem.h"

#include <iostream>

KernelProcess::KernelProcess(ProcessId pid) { this->pid = pid; }

KernelProcess::~KernelProcess() {}

ProcessId KernelProcess::getProcessId() const { return pid; }

Status KernelProcess::createSegment(VirtualAddress startAddress,
                                    PageNum segmentSize, AccessType flags) {
  std::cout << "KernelProcess::createSegment() pid: " << pid
            << " startAddress : " << startAddress
            << " segmentSize: " << segmentSize << std::endl;

  if (startAddress % PAGE_SIZE) {
    std::cout << "KernelProcess::createSegment() startAddress: " << startAddress
              << " is not aligned to PAGE_SIZE: " << PAGE_SIZE << std::endl;
    return Status::TRAP;
  }

  bool overlapFound = false;
  for (int i = 0; i < segmentSize && !overlapFound; i++) {
    VirtualAddress currentAddress = startAddress + i * PAGE_SIZE;
    auto it = pmt.find(currentAddress);
    if (it != pmt.end()) {
      overlapFound = true;
    }
  }

  if (overlapFound) {
    std::cout
        << "KernelProcess::createSegment() overlapFound for startAddress: "
        << startAddress << " segmentSize: " << segmentSize
        << " refusing to createSegment" << std::endl;
    return Status::TRAP;
  }

  auto kernelSystem = KernelSystem::getInstance();
  for (int i = 0; i < segmentSize; i++) {
    VirtualAddress currentAddress = startAddress + i * PAGE_SIZE;
    auto segmentPhysicalAddress = kernelSystem->getFreeSegment(pid);
    pmt[currentAddress] = segmentPhysicalAddress;
    segmentAccessPermissions[currentAddress] = flags;
  }
  return Status::OK;
}

Status KernelProcess::loadSegment(VirtualAddress startAddress,
                                  PageNum segmentSize, AccessType flags,
                                  void* content) {
  std::cout << "KernelProcess::loadSegment() pid: " << pid
            << " startAddress : " << startAddress
            << " segmentSize: " << segmentSize << std::endl;

  // First create the segment
  auto createSegmentReturnCode =
      this->createSegment(startAddress, segmentSize, flags);
  if (createSegmentReturnCode != Status::OK) {
    std::cout << "KernelProcess::loadSegment() failed to create segment!"
              << std::endl;
    return Status::TRAP;
  }

  // Afterwards load the segment with content
  for (int i = 0; i < segmentSize; i++) {
    auto offset = i * PAGE_SIZE;
    VirtualAddress currentAddress = startAddress + offset;
    auto currentContent = reinterpret_cast<PhysicalAddress>(
        reinterpret_cast<uint64_t>(content) + offset);
    auto physicalAddress = pmt[currentAddress];
    memcpy(physicalAddress, currentContent, PAGE_SIZE);
  }

  return Status::OK;
}

Status KernelProcess::deleteSegment(VirtualAddress startAddress) {
  if (isAddressInColdStorage(startAddress)) {
    coldStorage.erase(startAddress);
    return Status::OK;
  }
  auto it = pmt.find(startAddress);
  if (it == pmt.end()) {
    std::cout << "KernelProcess::deleteSegment() startAddress: " << startAddress
              << " does not belong to any segment, refusing to deleteSegment"
              << std::endl;
    return Status::TRAP;
  }
  auto physicalAddress = it->second;
  auto kernelSystem = KernelSystem::getInstance();
  kernelSystem->releaseSegment(physicalAddress);
  pmt.erase(startAddress);
  segmentAccessPermissions.erase(startAddress);
  return Status::OK;
}

Status KernelProcess::pageFault(VirtualAddress address) {
  if (isAddressInColdStorage(address) == false) {
    std::cout << "KernelProcess::pageFault() address: " << address
              << " is not in coldStorage, refusing to handle pageFault"
              << std::endl;
    return Status::TRAP;
  }
  // Need to load the address from coldStorage to VM, if there is no free space
  // we also need to put something in coldStorage

  auto clusterNo = coldStorage[address];
  auto partition = KernelSystem::getInstance()->getPartition();
  auto freeSegments = KernelSystem::getInstance()->getFreeSegments();
  char* buffer = new char[PAGE_SIZE];
  partition->readCluster(clusterNo, buffer);
  coldStorage.erase(address);

  // If there is free space we just load into VM
  if (!(*freeSegments).empty()) {
    auto physicalAddress = (*freeSegments).front();
    (*freeSegments).pop_front();
    memcpy(physicalAddress, buffer, PAGE_SIZE);
    pmt[address] = physicalAddress;
    return Status::OK;
  }

  // Otherwise we swap
  auto it = pmt.begin();
  auto virtualAddress = it->first;
  auto physicalAddress = it->second;
  pmt.erase(it);

  char* content = new char[PAGE_SIZE];
  memcpy(content, physicalAddress, PAGE_SIZE);
  memcpy(physicalAddress, buffer, PAGE_SIZE);
  pmt[address] = physicalAddress;

  auto coldStorageContent = reinterpret_cast<const char*>(content);
  partition->writeCluster(clusterNo, coldStorageContent);
  coldStorage[virtualAddress] = clusterNo;

  return Status::OK;
}

PhysicalAddress KernelProcess::getPhysicalAddress(VirtualAddress address) {
  VirtualAddress offset = address % PAGE_SIZE;
  VirtualAddress alignedVirtualAddress = address - offset;
  auto& it = pmt.find(alignedVirtualAddress);
  if (it != pmt.end()) {
    auto physicalAddress = reinterpret_cast<uint64_t>(it->second);
    return reinterpret_cast<PhysicalAddress>(physicalAddress + offset);
  }
  return 0;
}

AccessType KernelProcess::getSegmentAccessPermissions(VirtualAddress address) {
  return segmentAccessPermissions[address];
}

bool KernelProcess::isAddressInColdStorage(VirtualAddress address) {
  auto it = coldStorage.find(address);
  if (it != coldStorage.end()) {
    return true;
  }
  return false;
}

std::unordered_map<VirtualAddress, PhysicalAddress>* KernelProcess::getPmt() {
  return &pmt;
}

std::unordered_map<VirtualAddress, ClusterNo>* KernelProcess::getColdStorage() {
  return &coldStorage;
}
