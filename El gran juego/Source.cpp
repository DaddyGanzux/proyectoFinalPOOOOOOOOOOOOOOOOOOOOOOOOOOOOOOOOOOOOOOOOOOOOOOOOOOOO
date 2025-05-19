// El gran juego

#include <iostream>
#include <Locale>
#include <time.h>
#include <codecvt>
#include <string>
#include <thread>
#include <random>
#include <ctime>
#include <string>
#include <vector>
#include <cstdlib>

using namespace std;


//Genarador de números aleatorios 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int generarNumeroAleatorio(int min, int max) //Generador de números aleatorios.
{
    static mt19937 generador(static_cast<unsigned int>(std::time(0)));
    uniform_int_distribution<int> distribucion(min, max);
    return distribucion(generador);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Clase Jugador
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Jugador
{
    private:

        int oroInicialJugador;
        string nombreJugador;
        int salud;
        int ataque;
        int defensa;
        int zi;

        Jugador(std::string nombreJugador, int salud, int ataque, int defensa)
        {
            this->salud = salud;
        
        }


public:
    Jugador()
    {
        nombreJugador = nombreJugador;
        oroInicialJugador = 200;
    }

    void funcMostrarInfoJugador()
    {
        cout << "Nombre: " << nombreJugador << " Oro inicial: " << oroInicialJugador << endl;
    }

    int getOroJugador()
    {
        return oroInicialJugador;
    }

    void setRestartOroJugador(Pocion paramPocion)
    {
        oroInicialJugador -= paramPocion.getPrecioPocion();
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Generador de enemigos
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Enemigo
{


private:

    string nombreEnemigo;
    int vidaEnemigo;
    int escudoEnemigo;
    int ataqueEnemigo;
    int velocidadEnemigo;

    //lista para posibles nombres de enemigos
    vector<string>NombresPosiblesEnemigo = { "Goblin", "Orco", "Troll", "Bandido", "Esqueleto", "Asesino" }; //se inicia con posicion 0

public:

    //constructor de la clase enemigo, tiene que llevar el mismo nombre de la variable atribuida al constructor
    Enemigo()
    {
        //Seleccionar nombre aleatorio del vector 
        nombreEnemigo = NombresPosiblesEnemigo.at(1 + rand() % 5);
        vidaEnemigo = rand() % (150 - 50) + 50; // Generar un número random entre 50 y 150.
        escudoEnemigo = 100;
        ataqueEnemigo = rand() % (200 - 80) + 80;
        velocidadEnemigo = rand() % (15 - 5) + 5;

    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Ssitema de peleas
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void atacar(Jugador& nombreEnemigo)
    {
        int daño = max(0, ataque - nombreEnemigo.defensa);
        daño += generarNumeroAleatorio(0, 29);
        nombreEnemigo.salud -= daño;
        cout << nombreJugador << " ataca a " << nombreEnemigo.nombreJugador << " causando " << daño << " de daño." << endl;
        if (nombreEnemigo.salud < 0) nombreEnemigo.salud = 0;
    }

    // Para mostrar el estado
    void mostrarEstado() const 
    {
        cout << nombreJugador << " - Salud: " << salud << endl;
    }

    // Para verificar si está vivo
    bool estaVivo() const 
    {
        return salud > 0;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//Objeto Poción 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Pocion
{
private:
    int cantidadPocion;
    int precioPocion;
    int nivelPocion;
    string descripcionEfectoPocion;
    string nombrePocion;

public:
    Pocion(int _cantidadPocion, int _precioPocion, int _nivelPocion, string _descripcionEfectoPocion, string _nombrePocion)
    {
        cantidadPocion = _cantidadPocion;
        precioPocion = _precioPocion;
        nivelPocion = _nivelPocion;
        descripcionEfectoPocion = _descripcionEfectoPocion;
        nombrePocion = _nombrePocion;
    }

    void funcMostrarInfoPocion()
    {
        cout << " Nombre: " << nombrePocion << ", nvl: " << nivelPocion << ", desc: " << descripcionEfectoPocion << ", cantidad: " << cantidadPocion << endl;
        cout << "Precio: " << precioPocion << " de oro" << endl;
    }

    int getPrecioPocion()
    {
        return precioPocion;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Objeto Tienda
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////M//////

class Tienda
{
private:
    vector<Pocion> listaPociones;

public:
    Tienda()
    {
        listaPociones.push_back(Pocion(3, 50, 2, "Restaura 50 puntos de vida", "Pocion de vida"));
        listaPociones.push_back(Pocion(8, 100, 5, "Restaura 40 puntos de mana", "Pocion de mana"));
        listaPociones.push_back(Pocion(2, 150, 10, "Restaura 100 puntos de vida", "Elixir mayor de vida"));
    }

    void funcMostrarMenu()
    {
        cout << "---- Bienvenido a la tienda de pociones ----" << endl;
        cout << "Selecciona un numero del menu:" << endl;

        for (size_t i = 0; i < listaPociones.size(); i++)
        {
            cout << i + 1 << ".- ";
            listaPociones[i].funcMostrarInfoPocion();
        }
        cout << listaPociones.size() + 1 << ".- Salir" << endl;
    }

    void funcComprarPocion(Jugador& paramJugador)
    {
        int opcionSeleccionada;
        do
        {
            funcMostrarMenu();
            cout << "Tu oro actual: " << paramJugador.getOroJugador() << endl;
            cout << "Opcion: ";
            cin >> opcionSeleccionada;

            if (opcionSeleccionada < 1 || opcionSeleccionada > listaPociones.size() + 1)
            {
                cout << "Opcion invalida. Intenta de nuevo." << endl;
                continue;
            }

            if (opcionSeleccionada == listaPociones.size() + 1)
            {
                cout << "Gracias por visitar la tienda." << endl;
                break;
            }

            Pocion pocionSeleccionada = listaPociones[opcionSeleccionada - 1];

            if (paramJugador.getOroJugador() >= pocionSeleccionada.getPrecioPocion())
            {
                paramJugador.setRestartOroJugador(pocionSeleccionada);
                cout << "Has comprado la pocion exitosamente!" << endl;
            }
            else
            {
                cout << "No tienes suficiente oro para comprar esta pocion." << endl;
            }

        } while (true);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

