#include "IoT.h"

#ifdef USE_IOT_MODULE
IoTListClass IoTList;
#endif

IoTListClass::IoTListClass()
{
}

IoTListClass::~IoTListClass()
{
}

void IoTListClass::RegisterGate(IoTGate* gate)
{
  if(!gate)
    return;
    
    gates.push_back(gate);
}

size_t IoTListClass::GetGatesCount()
{
  return gates.size();
}

IoTGate* IoTListClass::GetGate(size_t index)
{
  if(index < gates.size())
    return gates[index];

   return NULL;
}
