#include "ns3/ptr.h"
#include "ns3/queue-disc.h"
#include "ns3/queue.h"
#include "ns3/random-variable-stream.h"
#include "ns3/average.h"
#include "colaTxObservador.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ColaTx");

// Cliente: emisor, Servidor: receptor
ColaTxObservador::ColaTxObservador (Ptr<Queue<Packet>> cola_tx)
{
  this->numPqEliminados = 0;
  this->numPqEnCola = 0;
  this->tamColaTx = cola_tx->GetMaxSize().GetValue();
  NS_LOG_DEBUG ("[COLA-TX]  Tamaño maximo: " << this->tamColaTx);

  cola_tx->TraceConnectWithoutContext ("Enqueue",
                                        MakeCallback(&ColaTxObservador::EncolaPq,
                                                     this));    
  cola_tx->TraceConnectWithoutContext ("Dequeue",
                                        MakeCallback(&ColaTxObservador::DesencolaPq,
                                                     this));  
  cola_tx->TraceConnectWithoutContext ("Drop",
                                        MakeCallback(&ColaTxObservador::EliminaPq,
                                                     this));  

  /* cola_tx->TraceConnectWithoutContext ("PacketsInQueue",
                                        MakeCallback(&ColaTxObservador::CambiaCola,
                                                     this));   */                                                
}


void
ColaTxObservador::EncolaPq (Ptr<const Packet> paquete)
{
  NS_LOG_FUNCTION (paquete);
  this->numPqEnCola ++;
  if (this->numPqEnCola == 100)
    NS_LOG_DEBUG ("[COLA-TX]  Paquete encolado.    Número de paquetes en cola: " << this->numPqEnCola << " -- ¡¡¡COLA TX LLENA!!!");
  else
    NS_LOG_DEBUG ("[COLA-TX]  Paquete encolado.    Número de paquetes en cola: " << this->numPqEnCola);
}


void
ColaTxObservador::DesencolaPq (Ptr<const Packet> paquete)
{
  NS_LOG_FUNCTION (paquete);
  this->numPqEnCola --;
  NS_LOG_DEBUG ("[COLA-TX]  Paquete desencolado. Número de paquetes en cola: " << this->numPqEnCola);
}


void
ColaTxObservador::EliminaPq (Ptr<const Packet> paquete)
{
  NS_LOG_FUNCTION (paquete);
  NS_LOG_DEBUG ("[COLA-TX]  Paquete eliminado.  Número de paquetes en cola: " << this->numPqEnCola << "Número de paquetes eliminados: " << this->numPqEliminados);
  this->numPqEliminados ++;
}


void
ColaTxObservador::CambiaCola (uint32_t oldValue, uint32_t newValue)
{
  NS_LOG_FUNCTION (oldValue << newValue);
  if (oldValue > newValue)
      NS_LOG_DEBUG ("[COLA-TX]  Cambia cola: " << oldValue << "  --->  " << newValue << "  (desencola)");
  if (oldValue < newValue){
    if (newValue == 100)
        NS_LOG_DEBUG ("[COLA-TX]  Cambia cola: " << oldValue << "  --->  " << newValue << "  (encola) -- ¡¡¡COLA TX LLENA!!!");
    else
        NS_LOG_DEBUG ("[COLA-TX]  Cambia cola: " << oldValue << "  --->  " << newValue << "  (encola)");
  }
}