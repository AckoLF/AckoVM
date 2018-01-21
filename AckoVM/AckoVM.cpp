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

  System system(alignedVmSpace, VM_SPACE_SIZE, alignedPmtSpace, PMT_SPACE_SIZE,
                partition);

  return 0;
}
