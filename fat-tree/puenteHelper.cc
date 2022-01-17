
#include "ns3/csma-helper.h"
#include "ns3/bridge-helper.h"
#include "puenteHelper.h"

using namespace ns3;

Ptr<Node> PuenteHelper (NodeContainer nodosLan, NetDeviceContainer & d_nodosLan)
{
  NetDeviceContainer d_puertosBridge;
  CsmaHelper         h_csma;
  BridgeHelper       h_bridge;
  Ptr<Node>          puente = CreateObject<Node> ();

  for (NodeContainer::Iterator indice = nodosLan.Begin ();
       indice != nodosLan.End ();
       indice++)
    {
      NetDeviceContainer enlace = h_csma.Install (NodeContainer (*indice, puente));
      d_nodosLan.Add (enlace.Get(0));
      d_puertosBridge.Add (enlace.Get(1));
    }
  h_bridge.Install (puente, d_puertosBridge);

  return puente;
}
