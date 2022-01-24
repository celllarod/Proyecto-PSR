#include "ns3/ptr.h"
#include "ns3/queue-disc.h"
#include "ns3/queue.h"
#include "ns3/random-variable-stream.h"
#include "ns3/average.h"

using namespace ns3;

// Clase que hará de observadora TCL (se subscribirá a trazas de fuente de los elementos de red)
class ColaTclObservador
{
public:
  // Constructor: recibe como parámetros todos los elementos a los que necesita 
  // suscribirse para realizar su función (cola_tx) 
          ColaTclObservador (Ptr<QueueDisc> cola_ct); 

  // Método para atender los encolados
  void    EncolaPq (Ptr< const QueueDiscItem> item);

  // Método para atender los desencolados
  void    DesencolaPq (Ptr< const QueueDiscItem> item);

  // Método para atender los paquetes eliminados
  void    EliminaPq (Ptr< const QueueDiscItem> item);

  // Método para atender cuando cambia cola
  void    CambiaCola (uint32_t oldValue, uint32_t newValue);
   
private:
  uint32_t numPqEnCola;
  uint32_t numPqEliminados;
  uint32_t tamCola;
};
