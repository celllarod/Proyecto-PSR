
class Punto
{
 public:
  // Constructor a partir de los tres valores a almacenar.
  Punto (double, double, double);
  // Devuelve el valor almacenado en m_abscisa.
  double abscisa ();
  // Devuelve el valor almacenado en m_ordenada.
  double ordenada ();
  // Devuelve el valor almacenado en m_error.
  double error ();

 private:
  // Valor correspondiente al eje de abscisas.
  double m_abscisa;
  // Valor correspondiente al eje de ordenadas.
  double m_ordenada;
  // Valor correspondiente al intervalo de confianza.
  double m_error;
};
    
   
