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
    static std::mt19937 generador(static_cast<unsigned int>(std::time(0)));
    std::uniform_int_distribution<int> distribucion(min, max);
    return distribucion(generador);
}

//Clase Item
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Item {
protected:
    string nombre;
    string tipo;
    int cantidad;

public:
    Item(string _nombre, string _tipo, int _cantidad)
        : nombre(_nombre), tipo(_tipo), cantidad(_cantidad) {}

    virtual void usar() = 0;

    string getNombre() const { return nombre; }
    string getTipo() const { return tipo; }
    int getCantidad() const { return cantidad; }

    void reducirCantidad(int c) { cantidad -= c; }

    virtual void mostrarInfo() const {
        cout << nombre << " | Tipo: " << tipo << " | Cantidad: " << cantidad << endl;
    }

    virtual int getCuracion() const { return 0; }

    virtual ~Item() {}
};

//Objeto Poción 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Pocion : public Item {
    string descripcion;
    int curacion;

public:
    Pocion(string _nombre, int _cantidad, string _descripcion, int _curacion)
        : Item(_nombre, "pocion", _cantidad), descripcion(_descripcion), curacion(_curacion) {}

    void usar() override {
        cout << "Usaste " << nombre << ": " << descripcion << " +" << curacion << " HP." << endl;
    }

    void mostrarInfo() const override {
        Item::mostrarInfo();
        cout << "   Efecto: " << descripcion << " (+ " << curacion << " HP)" << endl;
    }

    int getCuracion() const override {
        return curacion;
    }
};

//Clase Jugador
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Jugador {
    string nombre;
    int salud;
    int saludMaxima;
    int ataque;
    int defensa;
    int oro;
    vector<Item*> inventario;

public:
    Jugador(string _nombre)
        : nombre(_nombre), salud(200), saludMaxima(300),
        ataque(generarNumeroAleatorio(20,50)), defensa(25), oro(100) {}

    void agregarItem(Item* item) {
        for (auto& i : inventario) {
            if (i->getNombre() == item->getNombre()) {
                i->reducirCantidad(-item->getCantidad());
                delete item;
                return;
            }
        }
        inventario.push_back(item);
    }

    void mostrarInventario() const {
        cout << "\n--- Inventario de " << nombre << " ---\n";
        if (inventario.empty()) {
            cout << "Inventario vacío.\n";
            return;
        }
        for (size_t i = 0; i < inventario.size(); ++i) {
            cout << i + 1 << ". ";
            inventario[i]->mostrarInfo();
        }
    }

    void usarItem() {
        mostrarInventario();
        if (inventario.empty()) return;

        int eleccion;
        cout << "Selecciona el número del item a usar: ";
        cin >> eleccion;
        eleccion--;

        if (eleccion >= 0 && eleccion < inventario.size()) {
            inventario[eleccion]->usar();
            salud += inventario[eleccion]->getCuracion();
            if (salud > saludMaxima) salud = saludMaxima;

            inventario[eleccion]->reducirCantidad(1);

            if (inventario[eleccion]->getCantidad() <= 0) {
                delete inventario[eleccion];
                inventario.erase(inventario.begin() + eleccion);
                cout << "Item eliminado del inventario.\n";
            }
        }
    
        
        else {
            cout << "Opción inválida.\n";
        }
    }

    void mostrarEstado() const {
        cout << nombre << " - Salud: " << salud << "/" << saludMaxima
            << ", Ataque: " << ataque << ", Defensa: " << defensa
            << ", Oro: " << oro << endl;
    }

    bool estaVivo() const {
        return salud > 0;
    }

    void recibirDanio(int danio) {
        int dañoReal = max(0, danio - defensa);
        salud -= dañoReal;
        if (salud < 0) salud = 0;
        cout << nombre << " recibió " << dañoReal << " de daño. Salud restante: " << salud << endl;
    }

    int getAtaque() const { return ataque; }
    void ganarOro(int cantidad) { oro += cantidad; }
    bool gastarOro(int cantidad) {
        if (oro >= cantidad) {
            oro -= cantidad;
            return true;
        }
        return false;
    }

    int getOro() const { return oro; }

    ~Jugador() {
        for (auto& i : inventario) delete i;
    }
};

//Clase Enemigo
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Enemigo {
    string nombre;
    int salud;
    int ataque;
    int defensa;

public:
    Enemigo()
    {
        vector<string> nombres = { "Goblin", "Orco", "Troll", "Bandido", "Esqueleto", "Asesino" };
        nombre = nombres[generarNumeroAleatorio(0, nombres.size() - 1)];
        salud = generarNumeroAleatorio(80, 360);
        ataque = generarNumeroAleatorio(20, 65);
        defensa = generarNumeroAleatorio(10, 30);
    }

    void mostrarEstado() const
    {
        cout << nombre << " - Salud: " << salud << ", Ataque: " << ataque << ", Defensa: " << defensa << endl;

        system("pause");
    }

    bool estaVivo() const {
        return salud > 0;
    }

    void recibirDanio(int danio)
    {
        int dañoReal = max(0, danio - defensa);
        salud -= dañoReal;
        if (salud < 0) salud = 0;
        cout << nombre << " recibió " << dañoReal << " de daño. Salud restante: " << salud << endl;

        system("pause");
    }

    int getAtaque() const { return ataque; }
    string getNombre() const { return nombre; }
};

//Clase Tienda
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Tienda {
    struct Producto {
        string nombre;
        string descripcion;
        int curacion;
        int precio;
    };

    vector<Producto> stock = {
        { "Poción de Vida", "Restaura 50 HP", 50, 30 },
        { "Elixir Menor", "Restaura 30 HP", 30, 20 },
        { "Poción Mayor", "Restaura 100 HP", 100, 50 }
    };

public:
    void mostrarProductos() {
        cout << "\n--- Tienda de Pociones ---\n";
        for (size_t i = 0; i < stock.size(); ++i) {
            cout << i + 1 << ". " << stock[i].nombre << " - " << stock[i].descripcion
                << " | Cura: " << stock[i].curacion << " | Precio: " << stock[i].precio << " oro\n";
        }
        cout << stock.size() + 1 << ". Salir\n";
    }

    void comprar(Jugador& jugador) {
        int opcion;
        while (true) {
            mostrarProductos();
            cout << "Oro disponible: " << jugador.getOro() << endl;
            cout << "Selecciona el número del producto que deseas comprar: ";
            cin >> opcion;

            if (opcion == static_cast<int>(stock.size() + 1)) break;

            if (opcion >= 1 && opcion <= static_cast<int>(stock.size())) {
                const Producto& p = stock[opcion - 1];
                if (jugador.gastarOro(p.precio)) {
                    jugador.agregarItem(new Pocion(p.nombre, 1, p.descripcion, p.curacion));
                    cout << "Has comprado " << p.nombre << "!\n";
                }
                else {
                    cout << "No tienes suficiente oro.\n";
                }
            }
            else {
                cout << "Opción inválida.\n";
            }
        }
    }
};

//Sitema de peleas
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void combate(Jugador& jugador, Enemigo& enemigo)
{
    system("cls");

    cout << "\n--- ¡Comienza el combate! ---\n";
    jugador.mostrarEstado();
    enemigo.mostrarEstado();

    while (jugador.estaVivo() && enemigo.estaVivo())
    {
        int opcion;

        system("cls");

        cout << "\nElige acción:\n1. Atacar\n2. Usar objeto\nOpción: ";
        cin >> opcion;

        if (opcion == 1)
        {
            enemigo.recibirDanio(jugador.getAtaque());
        }
        else if (opcion == 2)
        {
            jugador.usarItem();
        }
        else
        {
            cout << "Opción inválida.\n";

            continue;
        }

        if (enemigo.estaVivo())
        {
            jugador.recibirDanio(enemigo.getAtaque());
        }
    }

    if (jugador.estaVivo())
    {
        int oroGanado = generarNumeroAleatorio(5, 15);
        jugador.ganarOro(oroGanado);
        cout << "\n¡Has derrotado al " << enemigo.getNombre() << "! Ganaste " << oroGanado << " de oro.\n";
    }
    else
    {
        cout << "\nHas sido derrotado...\n";
    }
}

int main()
{
                
}