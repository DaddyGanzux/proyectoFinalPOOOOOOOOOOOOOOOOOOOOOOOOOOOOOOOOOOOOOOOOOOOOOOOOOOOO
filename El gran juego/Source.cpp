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


//Genarador de números aleatorios by ChatGPT
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int generarNumeroAleatorio(int min, int max) //Generador de números aleatorios.
{
    static std::mt19937 generador(static_cast<unsigned int>(std::time(0)));
    std::uniform_int_distribution<int> distribucion(min, max);
    return distribucion(generador);
}

//Imprimir lento by ChatGPT
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void imprimirLento(const std::string& texto, int retrasoMilisegundos)
{
    for (char c : texto)
    {
        std::cout << c << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(retrasoMilisegundos));
    }
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

//Objeto Poción by ChatGPT
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Pocion : public Item {
    string descripcion;
    int curacion;

public:
    Pocion(string _nombre, int _cantidad, string _descripcion, int _curacion)
        : Item(_nombre, "pocion", _cantidad), descripcion(_descripcion), curacion(_curacion) {}

    void usar() override {
        cout << "Usaste " << nombre << ": " << descripcion << " +" << curacion << " HP.\n";
    }

    void mostrarInfo() const override {
        Item::mostrarInfo();
        cout << "   Efecto: " << descripcion << " (+ " << curacion << " HP)\n";
    }

    int getCuracion() const override {
        return curacion;
    }
};

//Clase Arma by ChatGPT
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Arma : public Item {
    int dano;

public:
    Arma(string _nombre, int _cantidad, int _dano)
        : Item(_nombre, "arma", _cantidad), dano(_dano) {}

    void usar() override {
        cout << "Equipas " << nombre << ". Daño de ataque: " << dano << endl;
    }

    int getDano() const override { return dano; }

    void mostrarInfo() const override {
        Item::mostrarInfo();
        cout << "   Daño de ataque: " << dano << endl;
    }
};

//Clase Jugador con ayuda de ChatGPT
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Jugador 
{
    string nombre;
    int salud;
    int saludMaxima;
    int ataqueBase;
    int defensa;
    int oro;
    Item* armaEquipada = nullptr;
    vector<Item*> inventario;

public:
    Jugador(string _nombre)
        : nombre(_nombre), salud(200), saludMaxima(300), ataqueBase(generarNumeroAleatorio(10,20)), defensa(15), oro(50) {}

    void agregarItem(Item* item) 
    {
        for (auto& i : inventario) 
        {
            if (i->getNombre() == item->getNombre()) 
            {
                i->reducirCantidad(-item->getCantidad());
                delete item;
                return;
            }
        }
        inventario.push_back(item);
    }

    void mostrarInventario() const 
    {
        cout << "--- Inventario de " << nombre << " ---\n";
        if (inventario.empty()) {
            cout << "Inventario vacío.\n";
            return;
        }
        for (size_t i = 0; i < inventario.size(); ++i) {
            cout << i + 1 << ". ";
            inventario[i]->mostrarInfo();
        }
    }

    void usarItem() 
    {
        mostrarInventario();
        if (inventario.empty()) return;

        int eleccion;
        cout << "Selecciona el número del item a usar:\n";
        cin >> eleccion;
        eleccion--;

        if (eleccion >= 0 && eleccion < inventario.size()) 
        {
            inventario[eleccion]->usar();

            salud += inventario[eleccion]->getCuracion();
            if (salud > saludMaxima) salud = saludMaxima;

            if (inventario[eleccion]->getTipo() == "arma") 
            {
                armaEquipada = inventario[eleccion];
            }

            inventario[eleccion]->reducirCantidad(1);
            if (inventario[eleccion]->getCantidad() <= 0) {
                if (inventario[eleccion] == armaEquipada) armaEquipada = nullptr;
                delete inventario[eleccion];
                inventario.erase(inventario.begin() + eleccion);
                cout << "Item eliminado del inventario.\n";
            }
        }
        else 
        {
            cout << "Opción inválida.\n";
        }
    }

    int getAtaque() const 
    {
        if (armaEquipada) return ataqueBase + armaEquipada->getDano();
        return ataqueBase;
    }

    void mostrarEstado() const 
    {
        cout << nombre << " - Salud: " << salud << "/" << saludMaxima
            << ", Ataque: " << getAtaque() << ", Defensa: " << defensa
            << ", Oro: " << oro << endl;
    }

    bool estaVivo() const { return salud > 0; }
    void recibirDanio(int danio) 
    {
        int dañoReal = max(0, danio - defensa);
        salud -= dañoReal;
        if (salud < 0) salud = 0;
        cout << nombre << " recibió " << dañoReal << " de daño. Salud restante: " << salud << endl;
    }

    void ganarOro(int cantidad) { oro += cantidad; }
    bool gastarOro(int cantidad) 
    {
        if (oro >= cantidad)
        {
            oro -= cantidad;
            return true;
        }
        return false;
    }
    int getOro() const { return oro; }

    ~Jugador() 
    {
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
    }

    bool estaVivo() const 
    {
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
class Tienda
{
    struct Producto
    {
        string nombre;
        string descripcion;
        int curacion;
        int precio;
    };

    struct ProductoArma
    {
        string nombre;
        int dano;
        int precio;
    };

    vector<Producto> stock =
    {
        { "Poción de Vida", "Restaura 50 HP", 50, 30 },
        { "Elixir Menor", "Restaura 30 HP", 30, 20 },
        { "Poción Mayor", "Restaura 100 HP", 100, 50 }
    };

    vector<ProductoArma> stockArmas =
    {
        { "Espada de Madera", generarNumeroAleatorio(10,40), 45},
        { "Espada de Hierro", generarNumeroAleatorio(20,55), 60},
        { "Espada de Diamante", generarNumeroAleatorio(30,70), 85},
        { "Hacha de Madera", generarNumeroAleatorio(20,30), 55},
        { "Hacha de Hierro", generarNumeroAleatorio(30,45), 70},
        { "Hacha de Diamante", generarNumeroAleatorio(40,60), 95}
    };

public:
    void comprar(Jugador& jugador)
    {
        int opcion;
        while (true)
        {
            system("pause");
            system("cls");

            cout << "--- Tienda ---\n";
            cout << "Oro disponible: " << jugador.getOro() << endl;
            cout << "1. Comprar pociones 2. Comprar armas 3. Salir Opción:\n";
            cin >> opcion;

            if (opcion == 1)
            {
                system("pause");
                system("cls");

                cout << "--- Pociones ---\n";
                for (size_t i = 0; i < stock.size(); ++i)
                {
                    cout << i + 1 << ". " << stock[i].nombre << " - " << stock[i].descripcion
                        << " | Cura: " << stock[i].curacion << " | Precio: " << stock[i].precio << " oro\n";
                }
                cout << stock.size() + 1 << ". Volver";
                cin >> opcion;
                if (opcion >= 1 && opcion <= static_cast<int>(stock.size()))
                {
                    const Producto& p = stock[opcion - 1];
                    if (jugador.gastarOro(p.precio))
                    {
                        jugador.agregarItem(new Pocion(p.nombre, 1, p.descripcion, p.curacion));
                        cout << "Has comprado " << p.nombre << "!\n";
                    }
                    else
                    {
                        cout << "No tienes suficiente oro.\n";
                    }
                }
            }
            else if (opcion == 2)
            {
                system("pause");
                system("cls");

                cout << "--- Armas ---\n";
                for (size_t i = 0; i < stockArmas.size(); ++i)
                {
                    cout << i + 1 << ". " << stockArmas[i].nombre << " | Daño: " << stockArmas[i].dano
                        << " | Precio: " << stockArmas[i].precio << " oro\n";
                }
                cout << stockArmas.size() + 1 << ". Volver";
                cin >> opcion;
                if (opcion >= 1 && opcion <= static_cast<int>(stockArmas.size()))
                {
                    const ProductoArma& a = stockArmas[opcion - 1];
                    if (jugador.gastarOro(a.precio))
                    {
                        jugador.agregarItem(new Arma(a.nombre, 1, a.dano));
                        cout << "Has comprado " << a.nombre << "!\n";
                    }
                    else
                    {
                        cout << "No tienes suficiente oro.\n";
                    }
                }
            }
            else
            {
                break;
            }
        }
    }
};

//Sitema de peleas
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void combate(Jugador& jugador, Enemigo& enemigo)
{
    system("cls");

    cout << "--- ¡Comienza el combate! ---\n";
    jugador.mostrarEstado();
    enemigo.mostrarEstado();

    while (jugador.estaVivo() && enemigo.estaVivo())
    {
        int opcion;

        system("cls");

        cout << "Elige acción:  1. Atacar  2. Usar objeto\n";
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
        cout << "¡Has derrotado al " << enemigo.getNombre() << "! Ganaste " << oroGanado << " de oro.\n";
    }
    else
    {
        cout << "Has sido derrotado...\n";
    }
}

//Ya ahora si el jueguito
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
    int option;
    string textoVacio;
    string Lento;

    while (true) 
    {
        system("cls");

        string despertar = "Despiertas en un lugar extraño.\n";
        imprimirLento(despertar, 40);

        string primeraOpcion = "Decides 1- Levantarte   2- Seguir durmiendo.\n";
        imprimirLento(primeraOpcion, 30);
        cin >> option;

        switch (option)
        {
        case 1:

            Lento = "Muy bien con optimismo, te esperan buenas cosas en este viaje, recuerda que al que madruga Dios le ayuda.\n";
            imprimirLento(Lento, 40);
            break;

        case 2:
            Lento = "Pues no papito aqui no se hace lo que quieras mientras vivas en esta casa me haras caso MI CODIGO!!! MIS REGLAS!!!\n";
            imprimirLento(Lento, 40);

            break;

        default:
            cout << "opcion en silla de ruedas (invalida), intentalo de nuevo\n";

            system("pause");

            continue;
        }

        system("pause");
        break;

    }

    system("cls");

    Lento = "Bueno, ya estamos levantaditos asi que \n";
    imprimirLento(Lento, 40);

    Lento = "...\n";
    imprimirLento(Lento, 1000);

    Lento = "RWAAAAAAAAAAAAAAAAR\n";
    imprimirLento(Lento, 20);

    system("pause");
    system("cls");

    Lento = "flush\n";
    imprimirLento(Lento, 20);
    Lento = "flush\n";
    imprimirLento(Lento, 20);
    Lento = "flush\n";
    imprimirLento(Lento, 20);

    system("pause");
    system("cls");

    Lento = "WOW, viste ese dragon, nunca lo habia visto antes espero que no sea un problema\n";
    imprimirLento(Lento, 40);

    Lento = " Que dices? QUE QUE QUIERES HACER!?\n";
    imprimirLento(Lento, 40);

    Lento = "1- Ir a matarlo 2- Ir a matarlo 3- Ir a matarlo 4- Ir a matarlo n- Ir a matarlo\n";
    imprimirLento(Lento, 30);
    getline(cin, textoVacio);
    
	Lento = "Bueno, si tu lo dices ya que no hay otra opcion, vamos a matarlo\n";
	imprimirLento(Lento, 40);

	Lento = "Creo que hay una aldea más adelante, deberiamos ir\n";
	imprimirLento(Lento, 40);

	Lento = "Creo que el camino era dos para enfente, uno a la derecha, otro para enfrente y uno a la izquierda.\n";
	imprimirLento(Lento, 40);

	system("pause"); 

    while (true || option == !4) 
    {
        system("cls");

        string primeraOpcion = "Que quieres hacer? 1- Abrir Inventario   2- Ir para enfrente  3- Ir para la izquierda  4- Ir para la derecha.\n";
        imprimirLento(primeraOpcion, 30);
        cin >> option;

        switch (option)
        {
        case 1:

            system("cls");
			Jugador.mostrarInventario();

            break;

        case 2:

            system("cls");
            Enemigo enemigo;
            combate(Jugador, enemigo);

            break;

		case 3:

            system("cls");
            Enemigo enemigo;
            combate(Jugador, enemigo);

            break;

		case 4:

            continue;

        default:
            cout << "opcion en silla de ruedas (invalida), intentalo de nuevo\n";

            system("pause");

            continue;
        }

        system("pause");
        break;

    }


    return 0;
}