#include <cassert>
#include <iostream>

#include "part.h"
#include "vm_declarations.h"

#include "ProcessTest.h"
#include "System.h"
#include "SystemTest.h"

#define VM_SPACE_SIZE (30)
#define PMT_SPACE_SIZE (3000)
#define N_PROCESS (2)
#define PERIODIC_JOB_COST (1)

using namespace std;

PhysicalAddress alignPointer(PhysicalAddress address) {
  uint64_t addr = reinterpret_cast<uint64_t>(address);

  addr += PAGE_SIZE;
  addr = addr / PAGE_SIZE * PAGE_SIZE;

  return reinterpret_cast<PhysicalAddress>(addr);
}

int main(void) {
  auto partition = new Partition("p1.ini");
  cout << partition->getNumOfClusters() << endl;

  uint64_t size = (VM_SPACE_SIZE + 2) * PAGE_SIZE;
  PhysicalAddress vmSpace = (PhysicalAddress) new char[size];

  cout << "vmSpace: " << vmSpace << endl;
  PhysicalAddress alignedVmSpace = alignPointer(vmSpace);
  cout << "alignedVmSpace: " << alignedVmSpace << endl;

  size = (PMT_SPACE_SIZE + 2) * PAGE_SIZE;
  PhysicalAddress pmtSpace = (PhysicalAddress) new char[size];

  cout << "pmtSpace: " << pmtSpace << endl;
  PhysicalAddress alignedPmtSpace = alignPointer(pmtSpace);
  cout << "alignedPmtSpace: " << alignedPmtSpace << endl;

  System system(alignedVmSpace, VM_SPACE_SIZE, alignedPmtSpace, PMT_SPACE_SIZE,
                partition);
  /*SystemTest systemTest(system, alignedVmSpace, VM_SPACE_SIZE);
  ProcessTest* process[N_PROCESS];
  std::thread* threads[N_PROCESS];

  std::mutex globalMutex;

  for (int i = 0; i < N_PROCESS; i++) {
    process[i] = new ProcessTest(system, systemTest);
  }

  for (int i = 0; i < N_PROCESS; i++) {
    std::cout << "Create process " << i << std::endl;
    threads[i] = new std::thread(&ProcessTest::run, process[i]);
  }

  for (int i = 0; i < N_PROCESS; i++) {
    threads[i]->join();
    delete threads[i];
    delete process[i];
  }

  delete[] vmSpace;
  delete[] pmtSpace;

  std::cout << "Test finished\n";

  cout << "Test creating overlapping segments and deleting segments" << endl;
  auto p = system.createProcess();
  auto createSegmentReturnCode =
      p->createSegment(0, 10, AccessType::READ_WRITE);
  assert(createSegmentReturnCode == Status::OK);
  createSegmentReturnCode = p->createSegment(1024, 10, AccessType::READ_WRITE);
  assert(createSegmentReturnCode == Status::TRAP);
  auto deleteSegmentReturnCode = p->deleteSegment(1024 * 10);
  assert(deleteSegmentReturnCode == Status::TRAP);
  deleteSegmentReturnCode = p->deleteSegment(1024);
  assert(deleteSegmentReturnCode == Status::OK);
  createSegmentReturnCode = p->createSegment(1024, 1, AccessType::READ_WRITE);
  assert(createSegmentReturnCode == Status::OK);

  cout << "Test finished" << endl;*/

  auto process = system.createProcess();
  auto pid = process->getProcessId();
  auto startAddress = 0;
  for (int i = 0; i < 2 * VM_SPACE_SIZE; i++) {
    auto offset = i * PAGE_SIZE;
    auto currentAddress = startAddress + offset;
    auto createSegmentReturnCode =
        process->createSegment(currentAddress, 1, AccessType::READ_WRITE);
    assert(createSegmentReturnCode == Status::OK);
  }

  auto cntOk = 0, cntPageFault = 0, cntTrap = 0;
  for (int i = 0; i < 2 * VM_SPACE_SIZE; i++) {
    auto offset = i * PAGE_SIZE;
    auto currentAddress = startAddress + offset;
    auto accessReturnCode =
        system.access(pid, currentAddress, AccessType::EXECUTE);
    if (accessReturnCode == Status::OK) {
      cntOk++;
    } else if (accessReturnCode == Status::PAGE_FAULT) {
      cntPageFault++;
    } else if (accessReturnCode == Status::TRAP) {
      cntTrap++;
    }
  }
  assert(cntPageFault == VM_SPACE_SIZE);
  assert(cntTrap == VM_SPACE_SIZE);

  for (int i = 0; i < 2 * VM_SPACE_SIZE; i++) {
    auto offset = i * PAGE_SIZE;
    auto currentAddress = startAddress + offset;
    auto accessReturnCode =
        system.access(pid, currentAddress, AccessType::READ_WRITE);
    if (accessReturnCode == Status::PAGE_FAULT) {
      auto pageFault = process->pageFault(currentAddress);
      assert(pageFault == Status::OK);
      auto returnCode =
          system.access(pid, currentAddress, AccessType::READ_WRITE);
      assert(returnCode == Status::OK);
    } else if (accessReturnCode == Status::TRAP) {
      assert(false);
    }
  }

  /*
  auto access = system.access(process->getProcessId(), 0 + index * PAGE_SIZE,
                              AccessType::EXECUTE);
  assert(access == Status::PAGE_FAULT);
  auto pageFault = process->pageFault(index);
  assert(pageFault == Status::OK);
  access = system.access(process->getProcessId(), 0 + index * PAGE_SIZE,
                         AccessType::EXECUTE);
  assert(access == Status::TRAP);
  access = system.access(process->getProcessId(), 0 + index * PAGE_SIZE,
                         AccessType::READ);
  assert(access == Status::OK);
  */
  cout << "Test finished" << endl;

  int debug;
  cin >> debug;
  cout << debug << endl;

  return 0;
}
