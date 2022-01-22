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


using namespace ns3;

#define MASCARA   "255.255.255.0"
#define TAM_COLA_TX "100p"
#define TAM_COLA_TCL 1000

NS_LOG_COMPONENT_DEFINE ("Fat_tree");


// *************************
// ** Declaración métodos **
// *************************
void printRoutingTable (Ptr<Node> node);
ObservadorPaquetes escenario (DataRate tasaEnvioCsma, Time retardoCsma, uint32_t mtuCsma, DataRate tasaEnvioP2P, Time retardoP2P,
                              DataRate tasaEnvioFuente, uint32_t tamPaqFuente, Time intervaloEnvio, uint32_t maxPq);

// *********************************
// ** Metodo para crear escenario **
// *********************************
ObservadorPaquetes
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

  /* comprobaciones_ip(pc1_1, pc1_2, pc1_3, pc1_4, pc2_1, pc2_2, pc2_3, pc2_4, pc3_1, pc3_2, pc3_3, pc3_4, pc4_1, pc4_2,
                    pc4_3, pc4_4, sw1_1, sw1_2, sw1_3, sw1_4, sw2_1, sw2_2, sw2_3, sw2_4, sw3_1, sw3_2, sw3_3, sw3_4,
                    sw4_1, sw4_2, sw4_3, sw4_4, swC1, swC3, swC2, swC4);
  */

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

  // ************************************** POD 2 **************************************
  // [CLIENTE]
  NodeContainer c_cliente2;
  c_cliente2.Create(1);

  h_pila.Install(c_cliente2);
  NodeContainer n_cli_2 = NodeContainer (c_cliente2, swC1);
  NetDeviceContainer d_cli_2 = h_csma.Install(n_cli_2);

  h_ipv4.SetBase("78.30.24.0", MASCARA);
  Ipv4InterfaceContainer i_cli_2 = h_ipv4.Assign(d_cli_2);

  NS_LOG_INFO("\n----------------------------------------------------------------------------");
  NS_LOG_INFO("\n[Cliente_2] Cliente 2 conectado al switch 2 del core.");

  // [CLI-SRV]
  Ptr<Node> n_servidor2 = pc2_1.Get(0);     // Fuente
  Ptr<Node> n_cliente2 = c_cliente2.Get(0);  // Sumidero

  // [CLIENTE] (sumidero)
  Ptr<UdpServer> udp_cliente2 = CreateObject<UdpServer>();
  n_cliente2->AddApplication(udp_cliente2);
  // Obtenemos ip y port del cliente para poder conectar al servidor con el mismo
  Ipv4Address ip_cliente2 = n_cliente2->GetObject<Ipv4>()->GetAddress(1,0).GetLocal();
  UintegerValue  port_value2;
  udp_cliente2->GetAttribute("Port", port_value2);
  uint16_t port_cliente2 = port_value2.Get();
  NS_LOG_INFO("[Cliente_2]   ID: "     << n_cliente2->GetId() <<
                        "  -- IP: "   << ip_cliente2 << 
                        " -- Port: " << port_cliente2);

  // [SERVIDOR] (fuente)
  Ptr<UdpEchoClient> udp_servidor2 = CreateObject<UdpEchoClient>();
  udp_servidor2 -> SetAttribute ("Interval", TimeValue(intervaloEnvio));
  udp_servidor2 -> SetAttribute ("PacketSize", UintegerValue(tamPaqFuente));
  udp_servidor2 -> SetAttribute ("MaxPackets", UintegerValue(maxPq));

  n_servidor2 -> AddApplication(udp_servidor2);
  udp_servidor2->SetRemote(ip_cliente2, port_cliente2);
  NS_LOG_INFO("[Servidor_2]  ID: "   << n_servidor2->GetId() <<
                      "  -- IP: "   << n_servidor2->GetObject<Ipv4>()->GetAddress(1,0).GetLocal());

  // ************************************** POD 3 **************************************
  // [CLIENTE]
  NodeContainer c_cliente3;
  c_cliente3.Create(1);

  h_pila.Install(c_cliente3);
  NodeContainer n_cli_3 = NodeContainer (c_cliente3, swC1);
  NetDeviceContainer d_cli_3 = h_csma.Install(n_cli_3);

  h_ipv4.SetBase("78.30.25.0", MASCARA);
  Ipv4InterfaceContainer i_cli_3 = h_ipv4.Assign(d_cli_3);

  NS_LOG_INFO("\n----------------------------------------------------------------------------");
  NS_LOG_INFO("\n[Cliente_3] Cliente 3 conectado al switch 3 del core.");

  // [CLI-SRV]
  Ptr<Node> n_servidor3 = pc3_1.Get(0);     // Fuente
  Ptr<Node> n_cliente3 = c_cliente3.Get(0);  // Sumidero

  // [CLIENTE] (sumidero)
  Ptr<UdpServer> udp_cliente3 = CreateObject<UdpServer>();
  n_cliente3->AddApplication(udp_cliente3);
  // Obtenemos ip y port del cliente para poder conectar al servidor con el mismo
  Ipv4Address ip_cliente3 = n_cliente3->GetObject<Ipv4>()->GetAddress(1,0).GetLocal();
  UintegerValue  port_value3;
  udp_cliente3->GetAttribute("Port", port_value3);
  uint16_t port_cliente3 = port_value3.Get();
  NS_LOG_INFO("[Cliente_3]   ID: "     << n_cliente3->GetId() <<
                        "  -- IP: "   << ip_cliente3 << 
                        " -- Port: " << port_cliente3);

  // [SERVIDOR] (fuente)
  Ptr<UdpEchoClient> udp_servidor3 = CreateObject<UdpEchoClient>();
  udp_servidor3 -> SetAttribute ("Interval", TimeValue(intervaloEnvio));
  udp_servidor3 -> SetAttribute ("PacketSize", UintegerValue(tamPaqFuente));
  udp_servidor3 -> SetAttribute ("MaxPackets", UintegerValue(maxPq));

  n_servidor3 -> AddApplication(udp_servidor3);
  udp_servidor3->SetRemote(ip_cliente3, port_cliente3);
  NS_LOG_INFO("[Servidor_3]  ID: "   << n_servidor3->GetId() <<
                      "  -- IP: "   << n_servidor3->GetObject<Ipv4>()->GetAddress(1,0).GetLocal());

  // ************************************** POD 4 **************************************
  // [CLIENTE]
  NodeContainer c_cliente4;
  c_cliente4.Create(1);

  h_pila.Install(c_cliente4);
  NodeContainer n_cli_4 = NodeContainer (c_cliente4, swC1);
  NetDeviceContainer d_cli_4 = h_csma.Install(n_cli_4);

  h_ipv4.SetBase("78.30.26.0", MASCARA);
  Ipv4InterfaceContainer i_cli_4 = h_ipv4.Assign(d_cli_4);

  NS_LOG_INFO("\n----------------------------------------------------------------------------");
  NS_LOG_INFO("\n[Cliente_4] Cliente 4 conectado al switch 4 del core.");

  // [CLI-SRV]
  Ptr<Node> n_servidor4 = pc4_1.Get(0);     // Fuente
  Ptr<Node> n_cliente4 = c_cliente4.Get(0);  // Sumidero

  // [CLIENTE] (sumidero)
  Ptr<UdpServer> udp_cliente4 = CreateObject<UdpServer>();
  n_cliente4->AddApplication(udp_cliente4);
  // Obtenemos ip y port del cliente para poder conectar al servidor con el mismo
  Ipv4Address ip_cliente4 = n_cliente4->GetObject<Ipv4>()->GetAddress(1,0).GetLocal();
  UintegerValue  port_value4;
  udp_cliente4->GetAttribute("Port", port_value4);
  uint16_t port_cliente4 = port_value4.Get();
  NS_LOG_INFO("[Cliente_4]   ID: "     << n_cliente4->GetId() <<
                        "  -- IP: "   << ip_cliente4 << 
                        " -- Port: " << port_cliente4);

  // [SERVIDOR] (fuente)
  Ptr<UdpEchoClient> udp_servidor4 = CreateObject<UdpEchoClient>();
  udp_servidor4 -> SetAttribute ("Interval", TimeValue(intervaloEnvio));
  udp_servidor4 -> SetAttribute ("PacketSize", UintegerValue(tamPaqFuente));
  udp_servidor4 -> SetAttribute ("MaxPackets", UintegerValue(maxPq));

  n_servidor4 -> AddApplication(udp_servidor4);
  udp_servidor4->SetRemote(ip_cliente4, port_cliente4);
  NS_LOG_INFO("[Servidor_4]  ID: "   << n_servidor4->GetId() <<
                      "  -- IP: "   << n_servidor4->GetObject<Ipv4>()->GetAddress(1,0).GetLocal());

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

  h_csma_pcap.EnablePcap("fuente2", n_servidor2->GetDevice(1));
  h_csma_pcap.EnablePcap("sumidero2", n_cliente2->GetDevice(1));

  h_csma_pcap.EnablePcap("fuente3", n_servidor3->GetDevice(1));
  h_csma_pcap.EnablePcap("sumidero3", n_cliente3->GetDevice(1));

  h_csma_pcap.EnablePcap("fuente4", n_servidor4->GetDevice(1));
  h_csma_pcap.EnablePcap("sumidero4", n_cliente4->GetDevice(1));
  
  // [TABLAS DE ENCAMINAMIENTO]
  NS_LOG_INFO("\n----------------------------------------------------------------------------");
  Ipv4GlobalRoutingHelper::PopulateRoutingTables();
  NS_LOG_INFO("[Escenario] Se crean las tablas de reenvío.");
  //printRoutingTable(sw1_1.Get(0));
  NS_LOG_INFO("\n----------------------------------------------------------------------------");

  // [OBSERVADORES]
  ObservadorPaquetes observadorPq1 (n_servidor1, udp_cliente1);
  ObservadorPaquetes observadorPq2 (n_servidor2, udp_cliente2);
  ObservadorPaquetes observadorPq3 (n_servidor3, udp_cliente3);
  ObservadorPaquetes observadorPq4 (n_servidor4, udp_cliente4);
  

  // ----------------------------------------------
  // Simulator::Stop (Time("60s"));
  NS_LOG_INFO ("\n[SIMULACION] Inicio de la simulación en el instante: " << Simulator::Now().GetSeconds() << "s\n");
  Simulator::Run ();
  NS_LOG_INFO ("[SIMULACION] Fin de la simulación en el instante: " << Simulator::Now().GetSeconds() << "s\n");
  NS_LOG_INFO("\n----------------------------------------------------------------------------");
  NS_LOG_LOGIC ("[RESULTADOS] Paquetes transmitidos: " << observadorPq1.GetCuentaTx());
  NS_LOG_LOGIC ("[RESULTADOS] Paquetes recibidos   : " << udp_cliente1->GetReceived());
  NS_LOG_LOGIC ("[RESULTADOS] Paquetes perdidos    : " << observadorPq1.GetPerdidos());
  double pq_perdidos1 = (observadorPq1.GetPerdidos()*100)/observadorPq1.GetCuentaTx();
  NS_LOG_LOGIC ("[RESULTADOS] \% paquetes perdidos  : " << pq_perdidos1 << "%" ); //<< std::setprecision(4)
  NS_LOG_LOGIC ("[RESULTADOS] Retardo medio        : " << observadorPq1.RetardoMedio(). GetSeconds() << "s");
  NS_LOG_INFO("\n----------------------------------------------------------------------------");
  NS_LOG_LOGIC ("[RESULTADOS] Paquetes transmitidos: " << observadorPq2.GetCuentaTx());
  NS_LOG_LOGIC ("[RESULTADOS] Paquetes recibidos   : " << udp_cliente2->GetReceived());
  NS_LOG_LOGIC ("[RESULTADOS] Paquetes perdidos    : " << observadorPq2.GetPerdidos());
  double pq_perdidos2 = (observadorPq2.GetPerdidos()*100)/observadorPq2.GetCuentaTx();
  NS_LOG_LOGIC ("[RESULTADOS] \% paquetes perdidos  : " << pq_perdidos2 << "%" ); //<< std::setprecision(4)
  NS_LOG_LOGIC ("[RESULTADOS] Retardo medio        : " << observadorPq2.RetardoMedio(). GetSeconds() << "s");
  NS_LOG_INFO("\n----------------------------------------------------------------------------");
  NS_LOG_LOGIC ("[RESULTADOS] Paquetes transmitidos: " << observadorPq3.GetCuentaTx());
  NS_LOG_LOGIC ("[RESULTADOS] Paquetes recibidos   : " << udp_cliente3->GetReceived());
  NS_LOG_LOGIC ("[RESULTADOS] Paquetes perdidos    : " << observadorPq3.GetPerdidos());
  double pq_perdidos3 = (observadorPq3.GetPerdidos()*100)/observadorPq3.GetCuentaTx();
  NS_LOG_LOGIC ("[RESULTADOS] \% paquetes perdidos  : " << pq_perdidos3 << "%" ); //<< std::setprecision(4)
  NS_LOG_LOGIC ("[RESULTADOS] Retardo medio        : " << observadorPq3.RetardoMedio(). GetSeconds() << "s");
  NS_LOG_INFO("\n----------------------------------------------------------------------------");  
  NS_LOG_LOGIC ("[RESULTADOS] Paquetes transmitidos: " << observadorPq4.GetCuentaTx());
  NS_LOG_LOGIC ("[RESULTADOS] Paquetes recibidos   : " << udp_cliente4->GetReceived());
  NS_LOG_LOGIC ("[RESULTADOS] Paquetes perdidos    : " << observadorPq4.GetPerdidos());
  double pq_perdidos4 = (observadorPq4.GetPerdidos()*100)/observadorPq4.GetCuentaTx();
  NS_LOG_LOGIC ("[RESULTADOS] \% paquetes perdidos  : " << pq_perdidos4 << "%" ); //<< std::setprecision(4)
  NS_LOG_LOGIC ("[RESULTADOS] Retardo medio        : " << observadorPq4.RetardoMedio(). GetSeconds() << "s");



  return observadorPq1;
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
  DataRate tasaEnvioCsma ("300Mbps");
  Time retardoCsma ("100ns");
  uint32_t mtuCsma (2000);
  DataRate tasaEnvioP2P ("10Gbps");
  Time retardoP2P ("10ns");
  DataRate tasaEnvioFuente ("20Mbps");
  uint32_t tamPaqFuente (1357);
  Time intervaloEnvio (Time("100us")); //0.0001
  uint32_t maxPq (11579);

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

  escenario(tasaEnvioCsma, retardoCsma, mtuCsma, tasaEnvioP2P, retardoP2P, tasaEnvioFuente, tamPaqFuente, intervaloEnvio, maxPq); 
  
  return 0;
}

