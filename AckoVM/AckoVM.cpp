#include <iostream>

#include "part.h"
#include "vm_declarations.h"

#include "System.h"

#define VM_SPACE_SIZE (10000)
#define PMT_SPACE_SIZE (3000)

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

  cout << "Testing whether KernelSystem is singleton or not" << endl;

  System system1(alignedVmSpace, VM_SPACE_SIZE, alignedPmtSpace, PMT_SPACE_SIZE,
                 partition);
  System system2(alignedVmSpace, VM_SPACE_SIZE, alignedPmtSpace, PMT_SPACE_SIZE,
                 partition);
  System system3(alignedVmSpace, VM_SPACE_SIZE, alignedPmtSpace, PMT_SPACE_SIZE,
                 partition);

  auto process1 = system1.createProcess();
  auto process2 = system2.createProcess();
  auto process3 = system3.createProcess();

  cout << "proccess1 pid: " << process1->getProcessId() << endl;
  cout << "proccess2 pid: " << process2->getProcessId() << endl;
  cout << "proccess3 pid: " << process3->getProcessId() << endl;

  int debug;
  cin >> debug;
  cout << debug << endl;

  return 0;
}
