#include "ns3/ptr.h"
#include "ns3/random-variable-stream.h"
#include "ns3/average.h"
#include "ns3/udp-echo-client.h"
#include "ns3/udp-server.h"
#include "etiquetaTiempo.h"

using namespace ns3;

class ObservadorPaquetes
{
public:
  // Constructor: recibe como parámetros todos los elementos a los que necesita 
  // suscribirse para realizar su función (emisor: servidor y receptor: cliente) 
          ObservadorPaquetes(Ptr<Node> emisor, Ptr<UdpServer> receptor); 

  // Método para atender la subscripción: Comienzo de ta TX
  void    Transmision(Ptr<const Packet> paquete);

  // Método para atender la subscripción: Comienzo de ta RX
  void    Recepcion(Ptr<const Packet> paquete);
  
  Time    RetardoMedio();

  uint32_t GetCuentaTx();

  uint32_t GetCuentaRx();

  uint32_t GetPerdidos();


private:
  Average<double> retardo_acumulado;
  uint32_t cuenta_tx;
  uint32_t cuenta_rx;
};
