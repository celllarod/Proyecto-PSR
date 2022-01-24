#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include "ns3/olsr-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/csma-module.h"
#include "ns3/bridge-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/inet-socket-address.h"
#include "ns3/random-variable-stream.h"
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
#include "ns3/packet-sink-helper.h"
#include "ns3/tcp-socket.h"
#include "ns3/udp-client-server-helper.h"
#include "ns3/udp-server.h"
#include "ns3/udp-echo-client.h"

using namespace ns3;
using namespace std;

#define TSTUDENT  2.1448
#define NUM_PUNTOS 20 
#define ITERACIONES 15 

#define TAM_COLA_TX "100p"
#define TAM_COLA_TCL 1000

NS_LOG_COMPONENT_DEFINE ("Bcube_grafica2");


// *************************
// ** Declaración métodos **
// *************************
Time escenario(uint32_t tamPaquete, Time intervalo, uint32_t maxPaquetes, Time delay_p2p, Time delay_csma, DataRate bitrate_p2p, DataRate bitrate_csma);
Punto punto (uint32_t tamPaquete, Time intervalo, uint32_t maxPaquetes, Time delay_p2p, Time delay_csma, DataRate bitrate_p2p, DataRate bitrate_csma);
Gnuplot2dDataset curva (uint32_t tamPaquete, Time intervalo, uint32_t maxPaquetes, Time delay_p2p, Time delay_csma, DataRate bitrate_p2p, DataRate bitrate_csma);
Gnuplot grafica (uint32_t tamPaquete, Time intervalo, uint32_t maxPaquetes, Time delay_p2p, Time delay_csma, DataRate bitrate_p2p, DataRate bitrate_csma);

// **********************
// ** Método principal **
// **********************
int main(int argc, char *argv[]){
  Time::SetResolution(Time::NS);

  uint32_t tamPaquete = 1357;
  Time intervalo ("100us");
  Time retardo_canal_p2p ("10ns");
  Time retardo_canal_csma ("100ns");
  uint32_t maxPaquetes = 5790;
  DataRate bitrate_p2p ("10Gbps");
  DataRate bitrate_csma ("10Mbps"); //300Mbps


  // Para ver las trazas udp añadimos a la linea de comandos: 'UdpClient=function:UdpServer=function'
  // Usamos la linea de comandos
  CommandLine cmd;
  cmd.AddValue("tamPaquete", "Tamaño de los paquetes enviados", tamPaquete);
  cmd.AddValue("intervalo", "Intervalo de envío", intervalo);
  cmd.AddValue("retardo_canal_p2p", "Retardo del canal p2p", retardo_canal_p2p);  
  cmd.AddValue("retardo_canal_csma", "Retardo del canal csma", retardo_canal_csma);  
  cmd.AddValue("bitrate_csma", "Tasa de envío de bits", bitrate_csma);
  cmd.AddValue("bitrate_p2p", "Tasa de envío de bits", bitrate_p2p);
  cmd.AddValue("maxPaquetes", "Máximo de paquetes a enviar", maxPaquetes);
  cmd.Parse(argc, argv);

  Gnuplot dibujo = grafica (tamPaquete, intervalo, maxPaquetes, retardo_canal_p2p, retardo_canal_csma, bitrate_p2p, bitrate_csma);
  
  std::ofstream fichero ("trabajo-bcube-grafica2.plt");
  dibujo.GenerateOutput (fichero);
  fichero << "pause -1" << std::endl;
  fichero.close (); 

}

// *********************************
// ** Metodo para crear escenario **
// *********************************
Time escenario(uint32_t tamPaquete, Time intervalo, uint32_t maxPaquetes, Time delay_p2p, Time delay_csma, DataRate bitrate_p2p, DataRate bitrate_csma){

    Simulator::Destroy ();
    // [NODOS]
    // pcX serán dispositivos finales, en ellos instalaremos las aplicaciones necesarias más adelante
    NodeContainer pc1,pc2,pc3,pc4,pc5,pc6,pc7,pc8,pc9,pc10,pc11,pc12,pc13,pc14,pc15,pc16,pcClient1;    
    
    // swX serán dispositivos sin capacidad de encaminamiento
    NodeContainer sw01, sw02, sw03, sw04, sw11, sw12, sw13, sw14;
    pc1.Create(1);
    pc2.Create(1);
    pc3.Create(1);
    pc4.Create(1);
    pc5.Create(1);
    pc6.Create(1);
    pc7.Create(1);
    pc8.Create(1);
    pc9.Create(1);
    pc10.Create(1);
    pc11.Create(1);
    pc12.Create(1);
    pc13.Create(1);
    pc14.Create(1);
    pc15.Create(1);
    pc16.Create(1);

    pcClient1.Create(1);
   
    sw01.Create(1);
    sw02.Create(1);
    sw03.Create(1);
    sw04.Create(1);
    sw11.Create(1);
    sw12.Create(1);
    sw13.Create(1);
    sw14.Create(1);

    NS_LOG_INFO("[Escenario] Nodos de todos los elementos creados.");

    InternetStackHelper pila;
    pila.Install (pc1);
    pila.Install (pc2);
    pila.Install (pc3);
    pila.Install (pc4);
    pila.Install (pc5);
    pila.Install (pc6);
    pila.Install (pc7);
    pila.Install (pc8);
    pila.Install (pc9);
    pila.Install (pc10);
    pila.Install (pc11);
    pila.Install (pc12);
    pila.Install (pc13);
    pila.Install (pc14);
    pila.Install (pc15);
    pila.Install (pc16);
    pila.Install (pcClient1);

    pila.Install (sw01);
    pila.Install (sw02);
    pila.Install (sw03);
    pila.Install (sw04);
    pila.Install (sw11);
    pila.Install (sw12);
    pila.Install (sw13);
    pila.Install (sw14);
    NS_LOG_INFO("[Escenario] Pila añadida a todos los dispositivos.");

    // [ENLACES]
    //Creamos los puentes, el primer valor indica el pc y los dos últimos dígitos el switch
    CsmaHelper h_csma;
    h_csma.SetChannelAttribute ("DataRate",DataRateValue (bitrate_csma));
    h_csma.SetChannelAttribute ("Delay", TimeValue (Time("100ns")));
    h_csma.SetDeviceAttribute ("Mtu", UintegerValue (2000));

    NodeContainer n101 = NodeContainer (pc1,sw01);
    NetDeviceContainer dsp101 = h_csma.Install(n101);

    NodeContainer n201 = NodeContainer (pc2,sw01);
    NetDeviceContainer dsp201 = h_csma.Install(n201);

    NodeContainer n301 = NodeContainer (pc3,sw01);
    NetDeviceContainer dsp301 = h_csma.Install(n301);

    NodeContainer n401 = NodeContainer (pc4,sw01);
    NetDeviceContainer dsp401 = h_csma.Install(n401);

    NodeContainer n502 = NodeContainer (pc5,sw02);
    NetDeviceContainer dsp502   = h_csma.Install(n502);

    NodeContainer n602 = NodeContainer (pc6,sw02);
    NetDeviceContainer dsp602 = h_csma.Install(n602);

    NodeContainer n702 = NodeContainer (pc7,sw02);
    NetDeviceContainer dsp702  = h_csma.Install(n702);

    NodeContainer n802 = NodeContainer (pc8,sw02);
    NetDeviceContainer dsp802  = h_csma.Install(n802);

    NodeContainer n903 = NodeContainer (pc9,sw03);
    NetDeviceContainer dsp903 = h_csma.Install(n903);

    NodeContainer n1003 = NodeContainer (pc10,sw03);
    NetDeviceContainer dsp1003 = h_csma.Install(n1003);

    NodeContainer n1103 = NodeContainer (pc11,sw03);
    NetDeviceContainer dsp1103 = h_csma.Install(n1103);

    NodeContainer n1203 = NodeContainer (pc12,sw03);
    NetDeviceContainer dsp1203 = h_csma.Install(n1203);

    NodeContainer n1304 = NodeContainer (pc13,sw04);
    NetDeviceContainer dsp1304 = h_csma.Install(n1304);

    NodeContainer n1404 = NodeContainer (pc14,sw04);
    NetDeviceContainer dsp1404 = h_csma.Install(n1404);

    NodeContainer n1504 = NodeContainer (pc15,sw04);
    NetDeviceContainer dsp1504 = h_csma.Install(n1504);

    NodeContainer n1604 = NodeContainer (pc16,sw04);
    NetDeviceContainer dsp1604 = h_csma.Install(n1604);

    NodeContainer cliente1 = NodeContainer (pcClient1,sw11);
    NetDeviceContainer cliente1dsp = h_csma.Install(cliente1);
   
    NS_LOG_INFO("[Escenario] Puentes creados entre switches y PCs.");

    // Configuramos el canal punto a punto
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute ("DataRate", DataRateValue (bitrate_p2p));
    p2p.SetChannelAttribute ("Delay", TimeValue (delay_p2p));

    // Añadimos el canal punto a punto a los dispositivos
    NodeContainer n111 = NodeContainer (pc1,sw11);
    NetDeviceContainer dsp111 = p2p.Install(n111);
    NodeContainer n511 = NodeContainer (pc5, sw11);
    NetDeviceContainer dsp511 = p2p.Install(n511);
    NodeContainer n911 = NodeContainer (pc9, sw11);
    NetDeviceContainer dsp911 = p2p.Install(n911); 
    NodeContainer n1311 = NodeContainer (pc13, sw11);
    NetDeviceContainer dsp1311 = p2p.Install(n1311);   


    NodeContainer n212 = NodeContainer (pc2,sw12);
    NetDeviceContainer dsp212 = p2p.Install(n212);
    NodeContainer n612 = NodeContainer (pc6,sw12);
    NetDeviceContainer dsp612 = p2p.Install(n612);
    NodeContainer n1012 = NodeContainer (pc10,sw12);
    NetDeviceContainer dsp1012 = p2p.Install(n1012);
    NodeContainer n1412 = NodeContainer (pc14,sw12);
    NetDeviceContainer dsp1412 = p2p.Install(n1412);   


    NodeContainer n313 = NodeContainer (pc3,sw13);
    NetDeviceContainer dsp313 = p2p.Install(n313);
    NodeContainer n713 = NodeContainer (pc7,sw13);
    NetDeviceContainer dsp713 = p2p.Install(n713);
    NodeContainer n1113 = NodeContainer (pc11,sw13);
    NetDeviceContainer dsp1113 = p2p.Install(n1113);
    NodeContainer n1513 = NodeContainer (pc15,sw13);
    NetDeviceContainer dsp1513 = p2p.Install(n1513);   


    NodeContainer n414 = NodeContainer (pc4,sw14);
    NetDeviceContainer dsp414 = p2p.Install(n414);
    NodeContainer n814 = NodeContainer (pc8,sw14);
    NetDeviceContainer dsp814 = p2p.Install(n814);
    NodeContainer n1214 = NodeContainer (pc12,sw14);
    NetDeviceContainer dsp1214 = p2p.Install(n1214);
    NodeContainer n1614 = NodeContainer (pc16,sw14);
    NetDeviceContainer dsp1614 = p2p.Install(n1614); 

    NS_LOG_INFO("[Escenario] Canales csma y punto a punto creados e instalados.");

    // [DIRECCIONAMIENTO IP]
    // Añadimos direcciones ip a los dispositivos
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.0.1.0", "255.255.255.0");
    Ipv4InterfaceContainer i1 = ipv4.Assign(dsp101);

    ipv4.SetBase("10.0.2.0", "255.255.255.0");
    Ipv4InterfaceContainer i2 = ipv4.Assign(dsp201);    

    ipv4.SetBase("10.0.3.0", "255.255.255.0");
    Ipv4InterfaceContainer i3 = ipv4.Assign(dsp301);

    ipv4.SetBase("10.0.4.0", "255.255.255.0");
    Ipv4InterfaceContainer i4 = ipv4.Assign(dsp401); 

    ipv4.SetBase("10.0.5.0", "255.255.255.0");
    Ipv4InterfaceContainer i5 = ipv4.Assign(dsp502);

    ipv4.SetBase("10.0.6.0", "255.255.255.0");
    Ipv4InterfaceContainer i6 = ipv4.Assign(dsp602); 

    ipv4.SetBase("10.0.7.0", "255.255.255.0");
    Ipv4InterfaceContainer i7 = ipv4.Assign(dsp702);

    ipv4.SetBase("10.0.8.0", "255.255.255.0");
    Ipv4InterfaceContainer i8 = ipv4.Assign(dsp802); 

    ipv4.SetBase("10.0.9.0", "255.255.255.0");
    Ipv4InterfaceContainer i9 = ipv4.Assign(dsp903);

    ipv4.SetBase("10.0.10.0", "255.255.255.0");
    Ipv4InterfaceContainer i10 = ipv4.Assign(dsp1003);    

    ipv4.SetBase("10.0.11.0", "255.255.255.0");
    Ipv4InterfaceContainer i11 = ipv4.Assign(dsp1103);

    ipv4.SetBase("10.0.12.0", "255.255.255.0");
    Ipv4InterfaceContainer i12 = ipv4.Assign(dsp1203); 

    ipv4.SetBase("10.0.13.0", "255.255.255.0");
    Ipv4InterfaceContainer i13 = ipv4.Assign(dsp1304);

    ipv4.SetBase("10.0.14.0", "255.255.255.0");
    Ipv4InterfaceContainer i14 = ipv4.Assign(dsp1404); 

    ipv4.SetBase("10.0.15.0", "255.255.255.0");
    Ipv4InterfaceContainer i15 = ipv4.Assign(dsp1504);

    ipv4.SetBase("10.0.16.0", "255.255.255.0");
    Ipv4InterfaceContainer i16 = ipv4.Assign(dsp1604);

    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer i1_1 = ipv4.Assign(dsp111);

    ipv4.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer i2_1 = ipv4.Assign(dsp212);    

    ipv4.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer i3_1 = ipv4.Assign(dsp313);

    ipv4.SetBase("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer i4_1 = ipv4.Assign(dsp414); 

    ipv4.SetBase("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer i5_1 = ipv4.Assign(dsp511);

    ipv4.SetBase("10.1.6.0", "255.255.255.0");
    Ipv4InterfaceContainer i6_1 = ipv4.Assign(dsp612); 

    ipv4.SetBase("10.1.7.0", "255.255.255.0");
    Ipv4InterfaceContainer i7_1 = ipv4.Assign(dsp713);

    ipv4.SetBase("10.1.8.0", "255.255.255.0");
    Ipv4InterfaceContainer i8_1 = ipv4.Assign(dsp814); 

    ipv4.SetBase("10.1.9.0", "255.255.255.0");
    Ipv4InterfaceContainer i9_1 = ipv4.Assign(dsp911);

    ipv4.SetBase("10.1.10.0", "255.255.255.0");
    Ipv4InterfaceContainer i10_1 = ipv4.Assign(dsp1012);    

    ipv4.SetBase("10.1.11.0", "255.255.255.0");
    Ipv4InterfaceContainer i11_1 = ipv4.Assign(dsp1113);

    ipv4.SetBase("10.1.12.0", "255.255.255.0");
    Ipv4InterfaceContainer i12_1 = ipv4.Assign(dsp1214); 

    ipv4.SetBase("10.1.13.0", "255.255.255.0");
    Ipv4InterfaceContainer i13_1 = ipv4.Assign(dsp1311);

    ipv4.SetBase("10.1.14.0", "255.255.255.0");
    Ipv4InterfaceContainer i14_1 = ipv4.Assign(dsp1412); 

    ipv4.SetBase("10.1.15.0", "255.255.255.0");
    Ipv4InterfaceContainer i15_1 = ipv4.Assign(dsp1513);

    ipv4.SetBase("10.1.16.0", "255.255.255.0");
    Ipv4InterfaceContainer i16_1 = ipv4.Assign(dsp1614);

    ipv4.SetBase("10.11.1.0", "255.255.255.0");
    Ipv4InterfaceContainer iClient1 = ipv4.Assign(cliente1dsp);

    NS_LOG_INFO ("[Escenario] Añadidas las direcciones IP a los PC's.");

 
    // [CLIENTE]
    Ptr<UdpServer> udp_cliente1 = CreateObject<UdpServer>();
    pcClient1.Get(0)->AddApplication(udp_cliente1);
    // Obtenemos ip y port del cliente para poder conectar al servidor con el mismo
    Ipv4Address ip_cliente1 = pcClient1.Get(0)->GetObject<Ipv4>()->GetAddress(1,0).GetLocal();
    UintegerValue  port_value1;
    udp_cliente1->GetAttribute("Port", port_value1);
    uint16_t port_cliente1 = port_value1.Get();
    NS_LOG_INFO("[Cliente_1]   ID: "     << pcClient1.Get(0)->GetId() <<
                          "  -- IP: "   << ip_cliente1 << 
                          " -- Port: " << port_cliente1);

    // [SERVIDOR] (fuente)
    Ptr<UdpEchoClient> udp_servidor1 = CreateObject<UdpEchoClient>();
    udp_servidor1 -> SetAttribute ("Interval", TimeValue(intervalo));
    udp_servidor1 -> SetAttribute ("PacketSize", UintegerValue(tamPaquete));
    udp_servidor1 -> SetAttribute ("MaxPackets", UintegerValue(maxPaquetes));

    pc1.Get(0) -> AddApplication(udp_servidor1);
    udp_servidor1->SetRemote(ip_cliente1, port_cliente1);
    NS_LOG_INFO("[Servidor_1]  ID: "   << pc1.Get(0)->GetId() <<
                        "   -- IP: "   << pc1.Get(0)->GetObject<Ipv4>()->GetAddress(1,0).GetLocal());

    // [COLA-SERVIDOR] (fuente)
    NS_LOG_INFO("----------------------------------------------------------------------------");
    // Obtenemos el puntero al TCL del nodo (OJO: puede ser nulo)
    Ptr<TrafficControlLayer> tcl = pc1.Get(0)->GetObject<TrafficControlLayer> ();

    // Para cada dispositivo en el nodo 
  
    Ptr<Queue<Packet>> cola_tx;
    Ptr<QueueDisc> cola_tcl;

    // Obtenemos el puntero al dispositivo
    Ptr<NetDevice> dispo = pc1.Get(0)->GetDevice(1);
    NS_LOG_INFO ("[COLA-TX]  Tipo dispositivo " << 1 << ": " << dispo->GetInstanceTypeId().GetName());

    // Sólo si el dispositivo es CsmaNetDevice buscamos la cola
    if ("ns3::CsmaNetDevice" == dispo->GetInstanceTypeId().GetName())
    {
      // Obtenemos el puntero a la cola de transmisión del dispositivo
      // (OJO: puede ser nulo).
      cola_tx = dispo->GetObject<CsmaNetDevice>()->GetQueue();
      if (cola_tx)
      {
        cola_tx->SetMaxSize(QueueSize(TAM_COLA_TX));
        NS_LOG_INFO ("[COLA-TX]  Cola en dispositivo " << 1 << " del nodo cliente 0: " << cola_tx->GetInstanceTypeId().GetName() << "  --  TAMAÑO MAXIMO: " << cola_tx->GetMaxSize());
      }
      else
        NS_LOG_INFO ("[COLA-TX]  Cola en dispositivo " << 1 << " del nodo cliente 0: Ninguna");
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
      

    // [TRAZAS PCAP]
    CsmaHelper h_csma_pcap;
    h_csma_pcap.EnablePcap("fuente1-bcube", pc1.Get(0)->GetDevice(1));
    h_csma_pcap.EnablePcap("sumidero1-bcube", pcClient1.Get(0)->GetDevice(1));

    // [TABLAS DE ENCAMINAMIENTO]
    NS_LOG_INFO("----------------------------------------------------------------------------");
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    NS_LOG_INFO("[Escenario] Se crean las tablas de reenvío.");
    NS_LOG_INFO("----------------------------------------------------------------------------");

    // [OBSERVADORES]
    // Añadimos observador al servidor 1
    ObservadorPaquetes observador1(pc1.Get(0), udp_cliente1);

    // [SIMULACION]
    NS_LOG_INFO ("[SIMULACION] Inicio de la simulación en el instante: " << Simulator::Now().GetSeconds() << "s\n");
    Simulator::Run ();
    NS_LOG_INFO ("[SIMULACION] Fin de la simulación en el instante: " << Simulator::Now().GetSeconds() << "s");
    NS_LOG_LOGIC("----------------------------------------------------------------------------");
    NS_LOG_LOGIC ("[RESULTADOS] Paquetes transmitidos: " << observador1.GetCuentaTx());
    NS_LOG_LOGIC ("[RESULTADOS] Paquetes recibidos   : " << udp_cliente1->GetReceived());
    NS_LOG_LOGIC ("[RESULTADOS] Paquetes perdidos    : " << observador1.GetPerdidos());
    double pq_perdidos1 = (observador1.GetPerdidos()*100)/observador1.GetCuentaTx();
    NS_LOG_LOGIC ("[RESULTADOS] \% paquetes perdidos  : " << pq_perdidos1 << "%" ); //<< std::setprecision(4)
    NS_LOG_LOGIC ("[RESULTADOS] Retardo medio        : " << observador1.RetardoMedio(). GetSeconds() << "s");

    return  Simulator::Now();
}


// ******************
// ** Método Punto **
// ******************
Punto punto (uint32_t tamPaquete, Time intervalo, uint32_t maxPaquetes, Time delay_p2p, Time delay_csma, DataRate bitrate_p2p, DataRate bitrate_csma){

    Average<double> ac_tiempo;
    double IC=0.0;
    for(uint32_t i=0 ; i<ITERACIONES ; i++){
      Time tiempo_sim = escenario(tamPaquete, intervalo, maxPaquetes, delay_p2p, delay_csma, bitrate_p2p, bitrate_csma);
      ac_tiempo.Update(tiempo_sim.GetMilliSeconds());
      NS_LOG_DEBUG ("[GRAFICA] \tSIMULACIÓN " << i+1 << " de " << ITERACIONES << " -- TIEMPO = " << tiempo_sim.GetMilliSeconds() << "ms");   
    }
    IC=TSTUDENT*sqrt(ac_tiempo.Var()/ac_tiempo.Count()); //Incertidumbre del 5%

    return Punto(bitrate_csma.GetBitRate()/1000000, ac_tiempo.Mean(), IC);
}


// ******************
// ** Método Curva **
// ******************
Gnuplot2dDataset curva (uint32_t tamPaquete, Time intervalo, uint32_t maxPaquetes, Time delay_p2p, Time delay_csma, DataRate bitrate_p2p, DataRate bitrate_csma){

    std::string nombrecurva = "Tiempo para que se cargue un 10% de un video de 5 minutos frente a la tasa del canal";
    Gnuplot2dDataset mi_curva(nombrecurva);
    mi_curva.SetStyle(Gnuplot2dDataset::LINES_POINTS);
    mi_curva.SetErrorBars(Gnuplot2dDataset::Y);

    DataRate tasa_canal_csma = bitrate_csma;

    for(uint32_t i=0 ; i<NUM_PUNTOS ; i++){
      Punto point = punto(tamPaquete, intervalo, maxPaquetes, delay_p2p, delay_csma, bitrate_p2p, tasa_canal_csma);
      NS_LOG_DEBUG ("[GRAFICA] \t--> PUNTO " << i+1 << " de " << NUM_PUNTOS << " -- TASA = " << tasa_canal_csma << " (Tasa,Tiempo,error) = (" << point.abscisa() <<", " << point.ordenada() <<", " << point.error() << ")");
      NS_LOG_DEBUG ("[GRAFICA] \t------------------------------------------------------------------------------");
      mi_curva.Add(point.abscisa(),point.ordenada(),point.error());
  	  tasa_canal_csma += 7.5e6;
    }

    return mi_curva;
}

// ********************
// ** Método Grafica **
// ********************
Gnuplot grafica (uint32_t tamPaquete, Time intervalo, uint32_t maxPaquetes, Time delay_p2p, Time delay_csma, DataRate bitrate_p2p, DataRate bitrate_csma){
     
    NS_LOG_DEBUG ("[GRAFICA] ------------------------------------------------------------------------------");
    NS_LOG_DEBUG ("[GRAFICA] CURVA " << 1 << " de " << 1);
    Gnuplot grafica;
    grafica.SetTitle("Gráfica Trabajo - BCube");
    grafica.SetLegend("Tasa del canal (Mbps)", "Tiempo (ms)");
    
    grafica.AddDataset(curva(tamPaquete, intervalo, maxPaquetes, delay_p2p, delay_csma, bitrate_p2p, bitrate_csma));
    
    return grafica;
}