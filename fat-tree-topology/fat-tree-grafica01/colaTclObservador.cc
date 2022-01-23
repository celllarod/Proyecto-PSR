#include "ns3/ptr.h"
#include "ns3/queue-disc.h"
#include "ns3/queue.h"
#include "ns3/random-variable-stream.h"
#include "ns3/average.h"
#include "colaTclObservador.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ColaTcl");

// Cliente: emisor, Servidor: receptor
ColaTclObservador::ColaTclObservador (Ptr<QueueDisc> cola_tcl )
{
  this->numPqEliminados = 0;
  this->numPqEnCola = 0;
  this->tamCola = cola_tcl->GetMaxSize().GetValue();;
  NS_LOG_DEBUG ("[COLA-TCL] Tamaño maximo: " << this->tamCola);

  // cola_tcl->TraceConnectWithoutContext ("Enqueue", MakeCallback(&ColaTclObservador::EncolaPq, this));    
  // cola_tcl->TraceConnectWithoutContext ("Dequeue", MakeCallback(&ColaTclObservador::DesencolaPq, this));  
  cola_tcl->TraceConnectWithoutContext ("Drop", MakeCallback(&ColaTclObservador::EliminaPq, this)); 
  cola_tcl->TraceConnectWithoutContext ("PacketsInQueue", MakeCallback(&ColaTclObservador::CambiaCola, this));   
}


void
ColaTclObservador::EncolaPq (Ptr< const QueueDiscItem> item)
{
  NS_LOG_FUNCTION (item);
  this->numPqEnCola ++;
  if (this->numPqEnCola == tamCola)
     NS_LOG_DEBUG ("[COLA-TCL] Paquete encolado.    Número de paquetes en cola: " << this->numPqEnCola << "  -- ¡¡¡COLA TCL LLENA!!!");
  else
     NS_LOG_DEBUG ("[COLA-TCL] Paquete encolado.    Número de paquetes en cola: " << this->numPqEnCola);
}


void
ColaTclObservador::DesencolaPq (Ptr< const QueueDiscItem> item)
{
  NS_LOG_FUNCTION (item);
  this->numPqEnCola --;
  NS_LOG_DEBUG ("[COLA-TCL] Paquete desencolado. Número de paquetes en cola: " << this->numPqEnCola);
}

void
ColaTclObservador::EliminaPq (Ptr< const QueueDiscItem> item)
{
  //NS_LOG_FUNCTION (item);
  NS_LOG_DEBUG ("[COLA-TCL] Paquete eliminado.   Número de paquetes en cola: " << this->numPqEnCola);
  this->numPqEliminados ++;
  // NS_LOG_DEBUG ("[COLA-TCL] Número de paquetes eliminados: " << this->numPqEliminados);
}

void
ColaTclObservador::CambiaCola (uint32_t oldValue, uint32_t newValue)
{
  // NS_LOG_FUNCTION (oldValue << newValue);
 if (oldValue > newValue)
     // NS_LOG_DEBUG ("[COLA-TCL] Cambia cola: " << oldValue << "  --->  " << newValue << "  (desencola)");
  if (oldValue < newValue){
    if (newValue == 102)
        NS_LOG_DEBUG ("[COLA-TCL] Cambia cola: " << oldValue << "  --->  " << newValue << "  (encola)  -- ¡¡¡COLA TCL LLENA!!!");
    // else
       // NS_LOG_DEBUG ("[COLA-TCL] Cambia cola: " << oldValue << "  --->  " << newValue << "  (encola)");
  }
}