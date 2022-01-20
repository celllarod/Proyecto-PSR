#include "ns3/object-base.h"
#include "ns3/log.h"
#include "ns3/onoff-application.h"
#include "observadorPaquetes.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ObservadorPaquetes");

// Cliente: emisor, Servidor: receptor
ObservadorPaquetes::ObservadorPaquetes (Ptr<Node> emisor, Ptr<UdpServer> receptor)
{
  retardo_acumulado.Reset();
  cuenta_tx = 0;
  cuenta_rx = 0;
  emisor->GetApplication(0)->GetObject<OnOffApplication>()->TraceConnectWithoutContext ("Tx", MakeCallback(&ObservadorPaquetes::Transmision, this));    
  receptor->TraceConnectWithoutContext ("Rx", MakeCallback(&ObservadorPaquetes::Recepcion, this));  
}


void
ObservadorPaquetes::Transmision (Ptr<const Packet> paquete)
{
  NS_LOG_FUNCTION (paquete);
  TimestampTag tsTag;
  tsTag.SetTimestamp(Simulator::Now());
  paquete->AddPacketTag(tsTag);
  cuenta_tx ++;
  NS_LOG_DEBUG ("[EMISOR]   Paquete enviado en el instante:      " << tsTag.GetTimestamp().GetSeconds() << "s" );
}


void
ObservadorPaquetes::Recepcion (Ptr<const Packet> paquete)
{
  NS_LOG_FUNCTION (paquete);  
  TimestampTag tsTag;
  if (paquete->PeekPacketTag(tsTag)){
    NS_LOG_DEBUG ("[RECEPTOR] Paquete recibido enviado en   :      " << tsTag.GetTimestamp().GetSeconds() << "s,   Recibido en: " << Simulator::Now().GetSeconds() << "s" );
    Time retardo = Simulator::Now() - tsTag.GetTimestamp() ;
    NS_LOG_DEBUG ("[RECEPTOR] Retardo del paquete recibido  :      " <<retardo.GetSeconds() << "s" );
    retardo_acumulado.Update(retardo.GetNanoSeconds());
    cuenta_rx ++;
  } else {
    NS_LOG_DEBUG ("[RECEPTOR] No se ha encontrado etiqueta de tipo TimeStamp en el paquete" );
  }
}


Time
ObservadorPaquetes::RetardoMedio ()
{
  NS_LOG_FUNCTION_NOARGS ();
  Time retardo_medio = NanoSeconds(retardo_acumulado.Mean());  
  return retardo_medio;
}


uint32_t 
ObservadorPaquetes::GetCuentaTx (){
  return cuenta_tx;
}


uint32_t 
ObservadorPaquetes::GetCuentaRx (){
  return cuenta_rx;
}

uint32_t 
ObservadorPaquetes::GetPerdidos (){
  return (cuenta_tx-cuenta_rx);
}

