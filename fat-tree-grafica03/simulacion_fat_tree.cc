#include "ns3/node-container.h"
#include "ns3/node.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/nstime.h"
#include "ns3/net-device-container.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-interface-container.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/application-container.h"
#include "ns3/uinteger.h"
#include "ns3/core-module.h"
#include "ns3/error-model.h"
#include "ns3/trace-helper.h"
#include "ns3/csma-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/on-off-helper.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/tcp-socket.h"
#include "ns3/udp-client-server-helper.h"
#include "ns3/udp-server.h"
#include "ns3/udp-echo-client.h"
#include "observadorPaquetes.h"
#include "ns3/queue-size.h"
#include "ns3/queue.h"
#include "ns3/queue-disc.h"
#include "ns3/traffic-control-layer.h"
#include "colaTxObservador.h"
#include "colaTclObservador.h"
#include "ns3/gnuplot.h"
#include "punto.h"
#include "ns3/average.h"
#include "ns3/point-to-point-net-device.h"


using namespace ns3;
using std::vector;

#define MASCARA   "255.255.255.0"
#define TAM_COLA_TX "100p"
#define TAM_COLA_TCL 1000


// ****
#define NUM_PUNTOS 20 //8
#define NUM_CURVAS 1
#define ITERACIONES 15 //5
// Para 30 muestras y un 95% de certidumbre (0.025 -- 30-1 --> 2.0452)
// Para 5 muestras y un 95% de certidumbre  (0.025 -- 5-1 --> 2.7765)
// Para 15 muestras y un 95% de certidumbre (0.025 -- 15-1 --> 2.1448)

#define TSTUDENT 2.1448

// ****

NS_LOG_COMPONENT_DEFINE ("Fat_tree");


// *************************
// ** Declaración métodos **
// *************************
void printRoutingTable (Ptr<Node> node);
Time escenario (DataRate tasaEnvioCsma, Time retardoCsma, uint32_t mtuCsma, DataRate tasaEnvioP2P, Time retardoP2P,
                              DataRate tasaEnvioFuente, uint32_t tamPaqFuente, Time intervaloEnvio, uint32_t maxPq);
Gnuplot grafica (DataRate tasaEnvioCsma, Time retardoCsma, uint32_t mtuCsma, DataRate tasaEnvioP2P, Time retardoP2P,
                 DataRate tasaEnvioFuente, uint32_t tamPaqFuente, Time intervaloEnvio, uint32_t maxPq);
Gnuplot2dDataset  curva (DataRate tasaEnvioCsma, Time retardoCsma, uint32_t mtuCsma, DataRate tasaEnvioP2P, Time retardoP2P,
                         DataRate tasaEnvioFuente, uint32_t tamPaqFuente, Time intervaloEnvio, uint32_t maxPq);
Punto punto (DataRate tasaEnvioCsma, Time retardoCsma, uint32_t mtuCsma, DataRate tasaEnvioP2P, Time retardoP2P,
             DataRate tasaEnvioFuente, uint32_t tamPaqFuente, Time intervaloEnvio, uint32_t maxPq);


// *********************************
// ** Metodo para crear escenario **
// *********************************
Time 
escenario ( DataRate tasaEnvioCsma  ,
            Time     retardoCsma    ,
            uint32_t mtuCsma        ,
            DataRate tasaEnvioP2P   ,
            Time     retardoP2P     ,
            DataRate tasaEnvioFuente,
            uint32_t tamPaqFuente   ,
            Time     intervaloEnvio ,
            uint32_t maxPq      
          )
{ 
  Simulator::Destroy();
  // [NODOS]
  // Servidores
  NodeContainer pc1_1, pc1_2, pc1_3, pc1_4, pc2_1, pc2_2, pc2_3, pc2_4, pc3_1, pc3_2, pc3_3, pc3_4, pc4_1, pc4_2, pc4_3, pc4_4;
  // Switches de los pods
  NodeContainer  sw1_1, sw1_2, sw1_3, sw1_4, sw2_1, sw2_2, sw2_3, sw2_4, sw3_1, sw3_2, sw3_3, sw3_4, sw4_1, sw4_2, sw4_3, sw4_4;
  // Switches del Core
  NodeContainer swC1, swC2, swC3, swC4;

  pc1_1.Create(1);
  pc1_2.Create(1);
  pc1_3.Create(1);
  pc1_4.Create(1);
  pc2_1.Create(1);
  pc2_2.Create(1);
  pc2_3.Create(1);
  pc2_4.Create(1);
  pc3_1.Create(1);
  pc3_2.Create(1);
  pc3_3.Create(1);
  pc3_4.Create(1);
  pc4_1.Create(1);
  pc4_2.Create(1);
  pc4_3.Create(1);
  pc4_4.Create(1);
  // ----
  sw1_1.Create(1);
  sw1_2.Create(1);
  sw1_3.Create(1);
  sw1_4.Create(1);
  sw2_1.Create(1);
  sw2_2.Create(1);
  sw2_3.Create(1);
  sw2_4.Create(1);
  sw3_1.Create(1);
  sw3_2.Create(1);
  sw3_3.Create(1);
  sw3_4.Create(1);
  sw4_1.Create(1);
  sw4_2.Create(1);
  sw4_3.Create(1);
  sw4_4.Create(1);
  // ----
  swC1.Create(1);
  swC2.Create(1);
  swC3.Create(1);
  swC4.Create(1);

  NS_LOG_INFO("[Escenario] Nodos de todos los elementos creados.");

  // [HELPERs]
  // Helper para la instalación de las pilas de protocolos
  InternetStackHelper h_pila;
  h_pila.SetIpv6StackInstall (false); // Desactivamos la pila correspondiente a IPv6
  // Pila de protocolos de los switches del Core
  h_pila.Install(swC1);
  h_pila.Install(swC2);
  h_pila.Install(swC3);
  h_pila.Install(swC4);
  NS_LOG_INFO("[Escenario] Pila de los switches del Core instanciada.");

  // Helper para la creación de los enlaces csma entre los equipos finales y los switches del lower layer
  CsmaHelper h_csma;
  h_csma.SetChannelAttribute ("DataRate",DataRateValue (tasaEnvioCsma));
  h_csma.SetChannelAttribute ("Delay", TimeValue (retardoCsma));
  h_csma.SetDeviceAttribute ("Mtu", UintegerValue (mtuCsma));

  // Helper para la creación de los enlaces p2p entre los switches
  PointToPointHelper h_p2p;
  h_p2p.SetDeviceAttribute ("DataRate", DataRateValue (tasaEnvioP2P));
  h_p2p.SetChannelAttribute ("Delay", TimeValue (retardoP2P));

  // Helper para el direccionamiento IPv4
  Ipv4AddressHelper h_ipv4;

  // ************************************** POD 1 **************************************
  NS_LOG_INFO("\n[Escenario] POD 1 ----------------------------------------------------------------------");
  // [PILA DE PROTOCOLOS] Pod 1
  h_pila.Install(pc1_1);
  h_pila.Install(pc1_2);
  h_pila.Install(pc1_3);
  h_pila.Install(pc1_4);
  h_pila.Install(sw1_1);
  h_pila.Install(sw1_2);
  h_pila.Install(sw1_3);
  h_pila.Install(sw1_4);
  NS_LOG_INFO("[Escenario] Pila de los switches y equipos finales del pod 1 instanciada.");

  // [ENLACES] Pod 1
  NodeContainer n1_1 = NodeContainer (pc1_1,sw1_1);
  NodeContainer n1_2 = NodeContainer (pc1_2,sw1_1);
  NodeContainer n1_3 = NodeContainer (pc1_3,sw1_2);
  NodeContainer n1_4 = NodeContainer (pc1_4,sw1_2);

  NodeContainer n1_13 = NodeContainer (sw1_1,sw1_3);
  NodeContainer n1_14 = NodeContainer (sw1_1,sw1_4);
  NodeContainer n1_23 = NodeContainer (sw1_2,sw1_3);
  NodeContainer n1_24 = NodeContainer (sw1_2,sw1_4);

  NodeContainer n1_3c1 = NodeContainer (sw1_3,swC1);
  NodeContainer n1_3c2 = NodeContainer (sw1_3,swC2);
  NodeContainer n1_4c3 = NodeContainer (sw1_4,swC3);
  NodeContainer n1_4c4 = NodeContainer (sw1_4,swC4);

  // Enlace CSMA entre los equipos finales y los switches del lower layer
  NetDeviceContainer d1_1 = h_csma.Install(n1_1);
  NetDeviceContainer d1_2 = h_csma.Install(n1_2);
  NetDeviceContainer d1_3 = h_csma.Install(n1_3);
  NetDeviceContainer d1_4 = h_csma.Install(n1_4);
  NS_LOG_INFO("[Escenario] Enlaces CSMA entre los equipos finales y los switches del lower layer creados.");

  // Enlace P2P entre los switches del lower layer y los switches del upper layer
  NetDeviceContainer d1_13 = h_p2p.Install(n1_13);
  NetDeviceContainer d1_14 = h_p2p.Install(n1_14);
  NetDeviceContainer d1_23 = h_p2p.Install(n1_23);
  NetDeviceContainer d1_24 = h_p2p.Install(n1_24);
  NS_LOG_INFO("[Escenario] Enlaces P2P entre los switches del lower layer y los switches del upper layer creados.");

  // Enlace P2P entre los switches del upper layer y los switches del Core
  NetDeviceContainer d1_3c1 = h_p2p.Install(n1_3c1);
  NetDeviceContainer d1_3c2 = h_p2p.Install(n1_3c2);
  NetDeviceContainer d1_4c3 = h_p2p.Install(n1_4c3);
  NetDeviceContainer d1_4c4 = h_p2p.Install(n1_4c4);
  NS_LOG_INFO("[Escenario] Enlaces P2P entre los switches del upper layer y los switches del Core creados.");

  // [DIRECCIONAMIENTO IP] Pod 1 
  h_ipv4.SetBase("10.1.0.0", MASCARA);
  Ipv4InterfaceContainer i1_1 = h_ipv4.Assign(d1_1);

  h_ipv4.SetBase("10.1.1.0", MASCARA);
  Ipv4InterfaceContainer i1_2 = h_ipv4.Assign(d1_2);

  h_ipv4.SetBase("10.1.2.0", MASCARA);
  Ipv4InterfaceContainer i1_3 = h_ipv4.Assign(d1_3);

  h_ipv4.SetBase("10.1.3.0", MASCARA);
  Ipv4InterfaceContainer i1_4 = h_ipv4.Assign(d1_4);

  h_ipv4.SetBase("10.1.4.0", MASCARA);
  Ipv4InterfaceContainer i1_13 = h_ipv4.Assign(d1_13);

  h_ipv4.SetBase("10.1.5.0", MASCARA);
  Ipv4InterfaceContainer i1_14 = h_ipv4.Assign(d1_14);

  h_ipv4.SetBase("10.1.6.0", MASCARA);
  Ipv4InterfaceContainer i1_23 = h_ipv4.Assign(d1_23);

  h_ipv4.SetBase("10.1.7.0", MASCARA);
  Ipv4InterfaceContainer i1_24 = h_ipv4.Assign(d1_24);

  h_ipv4.SetBase("10.1.8.0", MASCARA);
  Ipv4InterfaceContainer i1_3c1 = h_ipv4.Assign(d1_3c1);

  h_ipv4.SetBase("10.1.9.0", MASCARA);
  Ipv4InterfaceContainer i1_3c2 = h_ipv4.Assign(d1_3c2);
  
  h_ipv4.SetBase("10.1.10.0", MASCARA);
  Ipv4InterfaceContainer i1_4c3 = h_ipv4.Assign(d1_4c3);

  h_ipv4.SetBase("10.1.11.0", MASCARA);
  Ipv4InterfaceContainer i1_4c4 = h_ipv4.Assign(d1_4c4);

  NS_LOG_INFO("[Escenario] Direcciones IPv4 asignadas a los equipos del pod 1.");

  // ************************************** POD 2 **************************************
  NS_LOG_INFO("\n[Escenario] POD 2 ----------------------------------------------------------------------");
  // [PILA DE PROTOCOLOS] Pod 2
  h_pila.Install(pc2_1);
  h_pila.Install(pc2_2);
  h_pila.Install(pc2_3);
  h_pila.Install(pc2_4);
  h_pila.Install(sw2_1);
  h_pila.Install(sw2_2);
  h_pila.Install(sw2_3);
  h_pila.Install(sw2_4);
  NS_LOG_INFO("[Escenario] Pila de los switches y equipos finales del pod 2 instanciada.");

  // [ENLACES] Pod 2
  NodeContainer n2_1 = NodeContainer (pc2_1,sw2_1);
  NodeContainer n2_2 = NodeContainer (pc2_2,sw2_1);
  NodeContainer n2_3 = NodeContainer (pc2_3,sw2_2);
  NodeContainer n2_4 = NodeContainer (pc2_4,sw2_2);

  NodeContainer n2_13 = NodeContainer (sw2_1,sw2_3);
  NodeContainer n2_14 = NodeContainer (sw2_1,sw2_4);
  NodeContainer n2_23 = NodeContainer (sw2_2,sw2_3);
  NodeContainer n2_24 = NodeContainer (sw2_2,sw2_4);

  NodeContainer n2_3c1 = NodeContainer (sw2_3,swC1);
  NodeContainer n2_3c2 = NodeContainer (sw2_3,swC2);
  NodeContainer n2_4c3 = NodeContainer (sw2_4,swC3);
  NodeContainer n2_4c4 = NodeContainer (sw2_4,swC4);

  // Enlace CSMA entre los equipos finales y los switches del lower layer
  NetDeviceContainer d2_1 = h_csma.Install(n2_1);
  NetDeviceContainer d2_2 = h_csma.Install(n2_2);
  NetDeviceContainer d2_3 = h_csma.Install(n2_3);
  NetDeviceContainer d2_4 = h_csma.Install(n2_4);
  NS_LOG_INFO("[Escenario] Enlaces CSMA entre los equipos finales y los switches del lower layer creados.");

  // Enlace P2P entre los switches del lower layer y los switches del upper layer
  NetDeviceContainer d2_13 = h_p2p.Install(n2_13);
  NetDeviceContainer d2_14 = h_p2p.Install(n2_14);
  NetDeviceContainer d2_23 = h_p2p.Install(n2_23);
  NetDeviceContainer d2_24 = h_p2p.Install(n2_24);
  NS_LOG_INFO("[Escenario] Enlaces P2P entre los switches del lower layer y los switches del upper layer creados.");

  // Enlace P2P entre los switches del upper layer y los switches del Core
  NetDeviceContainer d2_3c1 = h_p2p.Install(n2_3c1);
  NetDeviceContainer d2_3c2 = h_p2p.Install(n2_3c2);
  NetDeviceContainer d2_4c3 = h_p2p.Install(n2_4c3);
  NetDeviceContainer d2_4c4 = h_p2p.Install(n2_4c4);
  NS_LOG_INFO("[Escenario] Enlaces P2P entre los switches del upper layer y los switches del Core creados.");

  // [DIRECCIONAMIENTO IP] Pod 2 
  h_ipv4.SetBase("10.2.0.0", MASCARA);
  Ipv4InterfaceContainer i2_1 = h_ipv4.Assign(d2_1);

  h_ipv4.SetBase("10.2.1.0", MASCARA);
  Ipv4InterfaceContainer i2_2 = h_ipv4.Assign(d2_2);

  h_ipv4.SetBase("10.2.2.0", MASCARA);
  Ipv4InterfaceContainer i2_3 = h_ipv4.Assign(d2_3);

  h_ipv4.SetBase("10.2.3.0", MASCARA);
  Ipv4InterfaceContainer i2_4 = h_ipv4.Assign(d2_4);

  h_ipv4.SetBase("10.2.4.0", MASCARA);
  Ipv4InterfaceContainer i2_13 = h_ipv4.Assign(d2_13);

  h_ipv4.SetBase("10.2.5.0", MASCARA);
  Ipv4InterfaceContainer i2_14 = h_ipv4.Assign(d2_14);

  h_ipv4.SetBase("10.2.6.0", MASCARA);
  Ipv4InterfaceContainer i2_23 = h_ipv4.Assign(d2_23);

  h_ipv4.SetBase("10.2.7.0", MASCARA);
  Ipv4InterfaceContainer i2_24 = h_ipv4.Assign(d2_24);

  h_ipv4.SetBase("10.2.8.0", MASCARA);
  Ipv4InterfaceContainer i2_3c1 = h_ipv4.Assign(d2_3c1);

  h_ipv4.SetBase("10.2.9.0", MASCARA);
  Ipv4InterfaceContainer i2_3c2 = h_ipv4.Assign(d2_3c2);
  
  h_ipv4.SetBase("10.2.10.0", MASCARA);
  Ipv4InterfaceContainer i2_4c3 = h_ipv4.Assign(d2_4c3);

  h_ipv4.SetBase("10.2.11.0", MASCARA);
  Ipv4InterfaceContainer i2_4c4 = h_ipv4.Assign(d2_4c4);

  NS_LOG_INFO("[Escenario] Direcciones IPv4 asignadas a los equipos del pod 2.");


  // ************************************** POD 3 **************************************
  NS_LOG_INFO("\n[Escenario] POD 3 ----------------------------------------------------------------------");
  // [PILA DE PROTOCOLOS] Pod 3
  h_pila.Install(pc3_1);
  h_pila.Install(pc3_2);
  h_pila.Install(pc3_3);
  h_pila.Install(pc3_4);
  h_pila.Install(sw3_1);
  h_pila.Install(sw3_2);
  h_pila.Install(sw3_3);
  h_pila.Install(sw3_4);
  NS_LOG_INFO("[Escenario] Pila de los switches y equipos finales del pod 3 instanciada.");

  // [ENLACES] Pod 3
  NodeContainer n3_1 = NodeContainer (pc3_1,sw3_1);
  NodeContainer n3_2 = NodeContainer (pc3_2,sw3_1);
  NodeContainer n3_3 = NodeContainer (pc3_3,sw3_2);
  NodeContainer n3_4 = NodeContainer (pc3_4,sw3_2);

  NodeContainer n3_13 = NodeContainer (sw3_1,sw3_3);
  NodeContainer n3_14 = NodeContainer (sw3_1,sw3_4);
  NodeContainer n3_23 = NodeContainer (sw3_2,sw3_3);
  NodeContainer n3_24 = NodeContainer (sw3_2,sw3_4);

  NodeContainer n3_3c1 = NodeContainer (sw3_3,swC1);
  NodeContainer n3_3c2 = NodeContainer (sw3_3,swC2);
  NodeContainer n3_4c3 = NodeContainer (sw3_4,swC3);
  NodeContainer n3_4c4 = NodeContainer (sw3_4,swC4);

  // Enlace CSMA entre los equipos finales y los switches del lower layer
  NetDeviceContainer d3_1 = h_csma.Install(n3_1);
  NetDeviceContainer d3_2 = h_csma.Install(n3_2);
  NetDeviceContainer d3_3 = h_csma.Install(n3_3);
  NetDeviceContainer d3_4 = h_csma.Install(n3_4);
  NS_LOG_INFO("[Escenario] Enlaces CSMA entre los equipos finales y los switches del lower layer creados.");

  // Enlace P2P entre los switches del lower layer y los switches del upper layer
  NetDeviceContainer d3_13 = h_p2p.Install(n3_13);
  NetDeviceContainer d3_14 = h_p2p.Install(n3_14);
  NetDeviceContainer d3_23 = h_p2p.Install(n3_23);
  NetDeviceContainer d3_24 = h_p2p.Install(n3_24);
  NS_LOG_INFO("[Escenario] Enlaces P2P entre los switches del lower layer y los switches del upper layer creados.");

  // Enlace P2P entre los switches del upper layer y los switches del Core
  NetDeviceContainer d3_3c1 = h_p2p.Install(n3_3c1);
  NetDeviceContainer d3_3c2 = h_p2p.Install(n3_3c2);
  NetDeviceContainer d3_4c3 = h_p2p.Install(n3_4c3);
  NetDeviceContainer d3_4c4 = h_p2p.Install(n3_4c4);
  NS_LOG_INFO("[Escenario] Enlaces P2P entre los switches del upper layer y los switches del Core creados.");

  // [DIRECCIONAMIENTO IP] Pod 3 
  h_ipv4.SetBase("10.3.0.0", MASCARA);
  Ipv4InterfaceContainer i3_1 = h_ipv4.Assign(d3_1);

  h_ipv4.SetBase("10.3.1.0", MASCARA);
  Ipv4InterfaceContainer i3_2 = h_ipv4.Assign(d3_2);

  h_ipv4.SetBase("10.3.2.0", MASCARA);
  Ipv4InterfaceContainer i3_3 = h_ipv4.Assign(d3_3);

  h_ipv4.SetBase("10.3.3.0", MASCARA);
  Ipv4InterfaceContainer i3_4 = h_ipv4.Assign(d3_4);

  h_ipv4.SetBase("10.3.4.0", MASCARA);
  Ipv4InterfaceContainer i3_13 = h_ipv4.Assign(d3_13);

  h_ipv4.SetBase("10.3.5.0", MASCARA);
  Ipv4InterfaceContainer i3_14 = h_ipv4.Assign(d3_14);

  h_ipv4.SetBase("10.3.6.0", MASCARA);
  Ipv4InterfaceContainer i3_23 = h_ipv4.Assign(d3_23);

  h_ipv4.SetBase("10.3.7.0", MASCARA);
  Ipv4InterfaceContainer i3_24 = h_ipv4.Assign(d3_24);

  h_ipv4.SetBase("10.3.8.0", MASCARA);
  Ipv4InterfaceContainer i3_3c1 = h_ipv4.Assign(d3_3c1);

  h_ipv4.SetBase("10.3.9.0", MASCARA);
  Ipv4InterfaceContainer i3_3c2 = h_ipv4.Assign(d3_3c2);
  
  h_ipv4.SetBase("10.3.10.0", MASCARA);
  Ipv4InterfaceContainer i3_4c3 = h_ipv4.Assign(d3_4c3);

  h_ipv4.SetBase("10.3.11.0", MASCARA);
  Ipv4InterfaceContainer i3_4c4 = h_ipv4.Assign(d3_4c4);

  NS_LOG_INFO("[Escenario] Direcciones IPv4 asignadas a los equipos del pod 3.");


  // ************************************** POD 4 **************************************
  NS_LOG_INFO("\n[Escenario] POD 4 ----------------------------------------------------------------------");
  // [PILA DE PROTOCOLOS] Pod 4
  h_pila.Install(pc4_1);
  h_pila.Install(pc4_2);
  h_pila.Install(pc4_3);
  h_pila.Install(pc4_4);
  h_pila.Install(sw4_1);
  h_pila.Install(sw4_2);
  h_pila.Install(sw4_3);
  h_pila.Install(sw4_4);
  NS_LOG_INFO("[Escenario] Pila de los switches y equipos finales del pod 4 instanciada.");

  // [ENLACES] Pod 4
  NodeContainer n4_1 = NodeContainer (pc4_1,sw4_1);
  NodeContainer n4_2 = NodeContainer (pc4_2,sw4_1);
  NodeContainer n4_3 = NodeContainer (pc4_3,sw4_2);
  NodeContainer n4_4 = NodeContainer (pc4_4,sw4_2);

  NodeContainer n4_13 = NodeContainer (sw4_1,sw4_3);
  NodeContainer n4_14 = NodeContainer (sw4_1,sw4_4);
  NodeContainer n4_23 = NodeContainer (sw4_2,sw4_3);
  NodeContainer n4_24 = NodeContainer (sw4_2,sw4_4);

  NodeContainer n4_3c1 = NodeContainer (sw4_3,swC1);
  NodeContainer n4_3c2 = NodeContainer (sw4_3,swC2);
  NodeContainer n4_4c3 = NodeContainer (sw4_4,swC3);
  NodeContainer n4_4c4 = NodeContainer (sw4_4,swC4);

  // Enlace CSMA entre los equipos finales y los switches del lower layer
  NetDeviceContainer d4_1 = h_csma.Install(n4_1);
  NetDeviceContainer d4_2 = h_csma.Install(n4_2);
  NetDeviceContainer d4_3 = h_csma.Install(n4_3);
  NetDeviceContainer d4_4 = h_csma.Install(n4_4);
  NS_LOG_INFO("[Escenario] Enlaces CSMA entre los equipos finales y los switches del lower layer creados.");

  // Enlace P2P entre los switches del lower layer y los switches del upper layer
  NetDeviceContainer d4_13 = h_p2p.Install(n4_13);
  NetDeviceContainer d4_14 = h_p2p.Install(n4_14);
  NetDeviceContainer d4_23 = h_p2p.Install(n4_23);
  NetDeviceContainer d4_24 = h_p2p.Install(n4_24);
  NS_LOG_INFO("[Escenario] Enlaces P2P entre los switches del lower layer y los switches del upper layer creados.");

  // Enlace P2P entre los switches del upper layer y los switches del Core
  NetDeviceContainer d4_3c1 = h_p2p.Install(n4_3c1);
  NetDeviceContainer d4_3c2 = h_p2p.Install(n4_3c2);
  NetDeviceContainer d4_4c3 = h_p2p.Install(n4_4c3);
  NetDeviceContainer d4_4c4 = h_p2p.Install(n4_4c4);
  NS_LOG_INFO("[Escenario] Enlaces P2P entre los switches del upper layer y los switches del Core creados.");

  // [DIRECCIONAMIENTO IP] Pod 4 
  h_ipv4.SetBase("10.4.0.0", MASCARA);
  Ipv4InterfaceContainer i4_1 = h_ipv4.Assign(d4_1);

  h_ipv4.SetBase("10.4.1.0", MASCARA);
  Ipv4InterfaceContainer i4_2 = h_ipv4.Assign(d4_2);

  h_ipv4.SetBase("10.4.2.0", MASCARA);
  Ipv4InterfaceContainer i4_3 = h_ipv4.Assign(d4_3);

  h_ipv4.SetBase("10.4.3.0", MASCARA);
  Ipv4InterfaceContainer i4_4 = h_ipv4.Assign(d4_4);

  h_ipv4.SetBase("10.4.4.0", MASCARA);
  Ipv4InterfaceContainer i4_13 = h_ipv4.Assign(d4_13);

  h_ipv4.SetBase("10.4.5.0", MASCARA);
  Ipv4InterfaceContainer i4_14 = h_ipv4.Assign(d4_14);

  h_ipv4.SetBase("10.4.6.0", MASCARA);
  Ipv4InterfaceContainer i4_23 = h_ipv4.Assign(d4_23);

  h_ipv4.SetBase("10.4.7.0", MASCARA);
  Ipv4InterfaceContainer i4_24 = h_ipv4.Assign(d4_24);

  h_ipv4.SetBase("10.4.8.0", MASCARA);
  Ipv4InterfaceContainer i4_3c1 = h_ipv4.Assign(d4_3c1);

  h_ipv4.SetBase("10.4.9.0", MASCARA);
  Ipv4InterfaceContainer i4_3c2 = h_ipv4.Assign(d4_3c2);
  
  h_ipv4.SetBase("10.4.10.0", MASCARA);
  Ipv4InterfaceContainer i4_4c3 = h_ipv4.Assign(d4_4c3);

  h_ipv4.SetBase("10.4.11.0", MASCARA);
  Ipv4InterfaceContainer i4_4c4 = h_ipv4.Assign(d4_4c4);

  NS_LOG_INFO("[Escenario] Direcciones IPv4 asignadas a los equipos del pod 4.");

  // ************************************** POD 1 **************************************
  // [CLIENTE]
  NodeContainer c_cliente1;
  c_cliente1.Create(1);

  h_pila.Install(c_cliente1);
  NodeContainer n_cli_1 = NodeContainer (c_cliente1, swC1);
  NetDeviceContainer d_cli_1 = h_csma.Install(n_cli_1);

  h_ipv4.SetBase("78.30.23.0", MASCARA);
  Ipv4InterfaceContainer i_cli_1 = h_ipv4.Assign(d_cli_1);

  NS_LOG_INFO("\n----------------------------------------------------------------------------");
  NS_LOG_INFO("\n[Cliente_1] Cliente 1 conectado al switch 1 del core.");

  // [CLI-SRV]
  Ptr<Node> n_servidor1 = pc1_1.Get(0);     // Fuente
  Ptr<Node> n_cliente1 = c_cliente1.Get(0);  // Sumidero

  // [CLIENTE] (sumidero)
  Ptr<UdpServer> udp_cliente1 = CreateObject<UdpServer>();
  n_cliente1->AddApplication(udp_cliente1);
  // Obtenemos ip y port del cliente para poder conectar al servidor con el mismo
  Ipv4Address ip_cliente1 = n_cliente1->GetObject<Ipv4>()->GetAddress(1,0).GetLocal();
  UintegerValue  port_value1;
  udp_cliente1->GetAttribute("Port", port_value1);
  uint16_t port_cliente1 = port_value1.Get();
  NS_LOG_INFO("[Cliente_1]   ID: "     << n_cliente1->GetId() <<
                        "  -- IP: "   << ip_cliente1 << 
                        " -- Port: " << port_cliente1);

  // [SERVIDOR] (fuente)
  Ptr<UdpEchoClient> udp_servidor1 = CreateObject<UdpEchoClient>();
  udp_servidor1 -> SetAttribute ("Interval", TimeValue(intervaloEnvio));
  udp_servidor1 -> SetAttribute ("PacketSize", UintegerValue(tamPaqFuente));
  udp_servidor1 -> SetAttribute ("MaxPackets", UintegerValue(maxPq));

  n_servidor1 -> AddApplication(udp_servidor1);
  udp_servidor1->SetRemote(ip_cliente1, port_cliente1);
  NS_LOG_INFO("[Servidor_1]  ID: "   << n_servidor1->GetId() <<
                      "  -- IP: "   << n_servidor1->GetObject<Ipv4>()->GetAddress(1,0).GetLocal());

  // [COLA-SERVIDOR] (fuente)
  NS_LOG_INFO("\n----------------------------------------------------------------------------");
  // Obtenemos el puntero al TCL del nodo (OJO: puede ser nulo)
  Ptr<TrafficControlLayer> tcl = n_servidor1->GetObject<TrafficControlLayer> ();

  // Para cada dispositivo en el nodo 
  for (uint32_t indice=0; indice<n_servidor1->GetNDevices(); indice++)
    {
      Ptr<Queue<Packet>> cola_tx;
      Ptr<QueueDisc> cola_tcl;

      // Obtenemos el puntero al dispositivo
      Ptr<NetDevice> dispo = n_servidor1->GetDevice(indice);
      NS_LOG_INFO ("[COLA-TX]  Tipo dispositivo " << indice << ": " << dispo->GetInstanceTypeId().GetName());

      // Sólo si el dispositivo es CsmaNetDevice buscamos la cola
      if ("ns3::CsmaNetDevice" == dispo->GetInstanceTypeId().GetName())
      {
        // Obtenemos el puntero a la cola de transmisión del dispositivo
        // (OJO: puede ser nulo).
        cola_tx = dispo->GetObject<CsmaNetDevice>()->GetQueue();
        if (cola_tx)
        {
          cola_tx->SetMaxSize(QueueSize(TAM_COLA_TX));
          NS_LOG_INFO ("[COLA-TX]  Cola en dispositivo " << indice << " del nodo cliente 0: " << cola_tx->GetInstanceTypeId().GetName() << "  --  TAMAÑO MAXIMO: " << cola_tx->GetMaxSize());
        }
        else
          NS_LOG_INFO ("[COLA-TX]  Cola en dispositivo " << indice << " del nodo cliente 0: Ninguna");
      }

      // Si hay TCL buscamos la cola de control del dispositivo
      if (tcl)
      {
        // Obtenemos el puntero a la cola (OJO: puede ser nulo)
        // utilizando el método del TCL.
        cola_tcl = tcl->GetRootQueueDiscOnDevice(dispo);
      
        if (cola_tcl) {
          cola_tcl->SetMaxSize(QueueSize(PACKETS, TAM_COLA_TCL));
          NS_LOG_INFO ("[COLA-TCL] Cola TCL              del nodo cliente 0: " << cola_tcl->GetInstanceTypeId().GetName () << "       --  TAMAÑO MAXIMO: " << cola_tcl->GetMaxSize());
        }
        else
          NS_LOG_INFO ("[COLA-TCL] Según TCL: Sin cola");
      }

      if(cola_tx && cola_tcl) {
        //[TRAZAS COLA TX]
        ColaTxObservador colaTxObs (cola_tx);
        //[TRAZAS COLA TCL]
        ColaTclObservador colaTclObs (cola_tcl);
      }
    }

  // [TRAZAS PCAP]
  CsmaHelper h_csma_pcap;
  h_csma_pcap.EnablePcap("fuente1", n_servidor1->GetDevice(1));
  h_csma_pcap.EnablePcap("sumidero1", n_cliente1->GetDevice(1));
  
  // [CAIDA ENLACE]
   // Caida enlace que une el sw1 con el sw3 del POD 1
  uint32_t indice_if_sw1 = d1_13.Get(0)->GetObject<PointToPointNetDevice>()->GetIfIndex();
  Ptr<Ipv4> ip_sw_1 = sw1_1.Get(0)->GetObject<Ipv4>();
  Time instanteCaida("0.625s");
  Simulator::Schedule (instanteCaida, &Ipv4::SetDown, ip_sw_1, indice_if_sw1);
  NS_LOG_INFO("[Caida] Programamos la caida del enlace en el instante: " << instanteCaida );

  // [TABLAS DE ENCAMINAMIENTO]
  NS_LOG_INFO("\n----------------------------------------------------------------------------");
  Ipv4GlobalRoutingHelper::PopulateRoutingTables();
  NS_LOG_INFO("[Escenario] Se crean las tablas de reenvío.");
  //printRoutingTable(sw1_1.Get(0));
  NS_LOG_INFO("\n----------------------------------------------------------------------------");

  // [OBSERVADORES]
  ObservadorPaquetes observadorPq1 (n_servidor1, udp_cliente1);
 
  // ----------------------------------------------
  // Simulator::Stop (Time("60s"));
  NS_LOG_INFO ("\n[SIMULACION] Inicio de la simulación en el instante: " << Simulator::Now().GetSeconds() << "s\n");
  Simulator::Run ();
  NS_LOG_INFO ("[SIMULACION] Fin de la simulación en el instante: " << Simulator::Now().GetSeconds() << "s\n");
  NS_LOG_LOGIC("\n----------------------------------------------------------------------------");
  NS_LOG_LOGIC ("[RESULTADOS] Paquetes transmitidos: " << observadorPq1.GetCuentaTx());
  NS_LOG_LOGIC ("[RESULTADOS] Paquetes recibidos   : " << udp_cliente1->GetReceived());
  NS_LOG_LOGIC ("[RESULTADOS] Paquetes perdidos    : " << observadorPq1.GetPerdidos());
  double pq_perdidos1 = (observadorPq1.GetPerdidos()*100)/observadorPq1.GetCuentaTx();
  NS_LOG_LOGIC ("[RESULTADOS] \% paquetes perdidos  : " << pq_perdidos1 << "%" ); //<< std::setprecision(4)
  NS_LOG_LOGIC ("[RESULTADOS] Retardo medio        : " << observadorPq1.RetardoMedio(). GetSeconds() << "s");

  return Simulator::Now();
}

// ******************************
// ** Método para imprimir FIB **
// ******************************
void printRoutingTable (Ptr<Node> node)
{
    Ipv4StaticRoutingHelper helper;
    Ptr<Ipv4> stack = node -> GetObject<Ipv4>();
    Ptr<Ipv4StaticRouting> staticrouting = helper.GetStaticRouting(stack);
    uint32_t numroutes=staticrouting->GetNRoutes();
    Ipv4RoutingTableEntry entry;
    std::cout << "[Tabla de reenvío]: " << Names::FindName(node) <<"\n";
    std::cout << "Destino\t\tMascara\t\tGateway\t\tIf\n";
    for (uint32_t i =0 ; i<numroutes;i++) {
        entry =staticrouting->GetRoute(i);
        std::cout << entry.GetDestNetwork()  << "\t" << entry.GetDestNetworkMask() << "\t" << entry.GetGateway() << "\t\t" << entry.GetInterface() << "\n";
     }
    return;
}


// **********************
// ** Método principal **
// **********************
int main (int argc, char *argv [])
{
  // Variables por línea de comandos
  DataRate tasaEnvioCsma ("10Mbps");  // De base serían 300Mbps
  Time retardoCsma ("100ns"); 
  uint32_t mtuCsma (2000);
  DataRate tasaEnvioP2P ("10Gbps");
  Time retardoP2P ("10ns"); 
  DataRate tasaEnvioFuente ("20Mbps");
  uint32_t tamPaqFuente (1357);
  Time intervaloEnvio (Time("100us"));
  uint32_t maxPq (5790);

  // Línea de comandos
  CommandLine cmd;
  cmd.AddValue ("tasaEnvioCsma", "Tasa de envío de los canales CSMA.", tasaEnvioCsma);
  cmd.AddValue ("retardoCsma", "Retardo de los canales CSMA.", retardoCsma);
  cmd.AddValue ("mtuCsma", "MTU de los canales CSMA.", mtuCsma);
  cmd.AddValue ("tasaEnvioP2P", "Tasa de envío de los canales P2P.", tasaEnvioP2P);
  cmd.AddValue ("retardoP2P", "Retardo de los canales P2P.", retardoP2P);
  cmd.AddValue ("tasaEnvioFuente", "Tasa de envío del servidor.", tasaEnvioFuente);
  cmd.AddValue ("tamPaqFuente", "Tamaño de los paquetes que envía el servidor.", tamPaqFuente);
  cmd.AddValue ("intervaloEnvio", "Intervalo entre paquetes enviados por la fuente.", intervaloEnvio);
  cmd.AddValue ("maxPq", "Número máximo de paquetes a enviar.", maxPq);

  cmd.Parse (argc, argv); 
  cmd.Parse (argc, argv); 

  Time::SetResolution (Time::NS);  

  // Creamos a gráfica
  Gnuplot dibujo =  grafica(tasaEnvioCsma, retardoCsma, mtuCsma, tasaEnvioP2P, retardoP2P, tasaEnvioFuente, tamPaqFuente, intervaloEnvio, maxPq); 

  // Generamos los ficheros con la gráfica
  std::ofstream fichero ("grafica03.plt");
  dibujo.GenerateOutput(fichero);
  fichero << "pause -1" << std::endl;
  fichero.close ();
  
  return 0;
}

// ******************
// ** Método Punto **
// ******************
Punto
punto ( DataRate tasaEnvioCsma  ,
        Time     retardoCsma    ,
        uint32_t mtuCsma        ,
        DataRate tasaEnvioP2P   ,
        Time     retardoP2P     ,
        DataRate tasaEnvioFuente,
        uint32_t tamPaqFuente   ,
        Time     intervaloEnvio ,
        uint32_t maxPq      
      )
{
    Average<double> ac_tiempo;
    
    // Simulación Z del punto j
    for (uint32_t iteracion = 0; iteracion<ITERACIONES; iteracion++){
        Time tiempo_sim = escenario(tasaEnvioCsma, retardoCsma, mtuCsma, tasaEnvioP2P, retardoP2P, tasaEnvioFuente, tamPaqFuente, intervaloEnvio, maxPq);   
        NS_LOG_DEBUG ("[GRAFICA] \tSIMULACIÓN " << iteracion+1 << " de " << ITERACIONES << " -- TIEMPO = " << tiempo_sim.GetMilliSeconds() << "ms");   
        ac_tiempo.Update(tiempo_sim.GetMilliSeconds());
    }

    // Error
    double error = TSTUDENT * sqrt (ac_tiempo.Var() / ac_tiempo.Count());
    Punto pto_curva (tasaEnvioCsma.GetBitRate()/1000000, ac_tiempo.Mean(), error);
    
    return pto_curva;
}

// ******************
// ** Método Curva **
// ******************
Gnuplot2dDataset
curva ( DataRate tasaEnvioCsma  ,
        Time     retardoCsma    ,
        uint32_t mtuCsma        ,
        DataRate tasaEnvioP2P   ,
        Time     retardoP2P     ,
        DataRate tasaEnvioFuente,
        uint32_t tamPaqFuente   ,
        Time     intervaloEnvio ,
        uint32_t maxPq      
      )
{
    Gnuplot2dDataset curva (" ");
    curva.SetStyle (Gnuplot2dDataset::LINES_POINTS);
    curva.SetErrorBars (Gnuplot2dDataset::Y);
    
    DataRate tasa_canal = tasaEnvioCsma;
    // Punto j de la curva i
    for (uint32_t numValores=0; numValores<NUM_PUNTOS; numValores++) {
        Punto pto = punto(tasa_canal, retardoCsma, mtuCsma, tasaEnvioP2P, retardoP2P, tasaEnvioFuente, tamPaqFuente, intervaloEnvio, maxPq);
        curva.Add(pto.abscisa(), pto.ordenada(), pto.error());
        NS_LOG_DEBUG ("[GRAFICA] \t--> PUNTO " << numValores+1 << " de " << NUM_PUNTOS << " -- TASA = " << tasa_canal << " (Tasa,Tiempo,error) = (" << pto.abscisa() <<", " << pto.ordenada() <<", " << pto.error() << ")");
        NS_LOG_DEBUG ("[GRAFICA] \t------------------------------------------------------------------------------");
        tasa_canal += 7.5e6; 
  }
  return curva;
}

// ********************
// ** Método Grafica **
// ********************
Gnuplot
grafica ( DataRate tasaEnvioCsma  ,
          Time     retardoCsma    ,
          uint32_t mtuCsma        ,
          DataRate tasaEnvioP2P   ,
          Time     retardoP2P     ,
          DataRate tasaEnvioFuente,
          uint32_t tamPaqFuente   ,
          Time     intervaloEnvio ,
          uint32_t maxPq      
        )
{
  Gnuplot grafica;
  grafica.SetTitle ("Tiempo para que se cargue un 10% de un video de 5 minutos frente a la tasa del canal");
  grafica.SetLegend ("Tasa del canal (Mbps)", "Tiempo (ms)");

  //Curva i (total 1)
  for (uint32_t numCurvas=0; numCurvas<NUM_CURVAS; numCurvas++){

    NS_LOG_DEBUG ("[GRAFICA] ------------------------------------------------------------------------------");
    NS_LOG_DEBUG ("[GRAFICA] CURVA " << numCurvas+1 << " de " << NUM_CURVAS);
    Gnuplot2dDataset crv = curva(tasaEnvioCsma, retardoCsma, mtuCsma, tasaEnvioP2P, retardoP2P, tasaEnvioFuente, tamPaqFuente, intervaloEnvio, maxPq);
    // Añadimos la curva a la gráfica
    grafica.AddDataset(crv);
  } 

  return grafica;   
}