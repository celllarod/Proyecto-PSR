#include "ns3/log.h"
#include "punto.h"


NS_LOG_COMPONENT_DEFINE ("Punto");

using namespace ns3;


Punto::Punto (double abscisa,
              double ordenada,
              double error)
{
  NS_LOG_FUNCTION (abscisa << ordenada << error);

  m_abscisa = abscisa;
  m_ordenada = ordenada;
  m_error = error;
}

double
Punto::abscisa ()
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_abscisa;
}

double
Punto::ordenada ()
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_ordenada;
}

double
Punto::error ()
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_error;
}
