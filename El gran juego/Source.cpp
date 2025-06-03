// El gran juego

#include <iostream>
#include <Locale>
#include <time.h>
#include <codecvt>
#include <thread>
#include <random>
#include <ctime>
#include <string>
#include <vector>
#include <cstdlib>
#include <limits>


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
    std::string nombre;
    std::string tipo;
    int cantidad;

public:
    Item(std::string _nombre, std::string _tipo, int _cantidad)
        : nombre(_nombre), tipo(_tipo), cantidad(_cantidad) {}

    virtual void usar() = 0;

    std::string getNombre() const { return nombre; }
    std::string getTipo() const { return tipo; }
    int getCantidad() const { return cantidad; }

    void reducirCantidad(int c) { cantidad -= c; }

    virtual void mostrarInfo() const {
        std:: cout << nombre << " | Tipo: " << tipo << " | Cantidad: " << cantidad << std::endl;
    }

    virtual int getCuracion() const { return 0; }
	virtual int getDano() const { return 0; }

    virtual ~Item() {}
};

//Objeto Poción ayda de ChatGPT
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Pocion : public Item {
    std::string descripcion;
    int curacion;

public:
    Pocion(std::string _nombre, int _cantidad, std::string _descripcion, int _curacion)
        : Item(_nombre, "pocion", _cantidad), descripcion(_descripcion), curacion(_curacion) {}

    void usar() override {
        std::cout << "Usaste " << nombre << ": " << descripcion << " +" << curacion << " HP.\n";
    }

    void mostrarInfo() const override {
        Item::mostrarInfo();
        std::cout << "   Efecto: " << descripcion << " (+ " << curacion << " HP)\n";
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
    Arma(std::string _nombre, int _cantidad, int _dano)
        : Item(_nombre, "arma", _cantidad), dano(_dano) {}

    void usar() override {
        std::cout << "Equipas " << nombre << ". Daño de ataque: " << dano << std::endl;
    }

    int getDano() const override { return dano; }

    void mostrarInfo() const override {
        Item::mostrarInfo();
        std::cout << "   Daño de ataque: " << dano << std::endl;
    }
};

//Clase Jugador con ayuda de ChatGPT
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Jugador 
{
    std::string nombre;
    int salud;
    int saludMaxima;
    int ataqueBase;
    int defensa;
    int oro;
    Item* armaEquipada = nullptr;
    std::vector<Item*> inventario;

public:
    Jugador(std::string _nombre)
        : nombre(_nombre), salud(200), saludMaxima(300), ataqueBase(26), defensa(15), oro(50) {}

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

public:
    void mostrarInventario() const 
    {
        std::cout << "--- Inventario de " << nombre << " ---\n";
        if (inventario.empty()) {
            std::cout << "Inventario vacío.\n";
            return;
        }
        for (size_t i = 0; i < inventario.size(); ++i) {
            std::cout << i + 1 << ". ";
            inventario[i]->mostrarInfo();
        }
    }

    void usarItem() 
    {
        mostrarInventario();
        if (inventario.empty()) return;

        int eleccion;
        std::cout << "Selecciona el número del item a usar:\n";
        std::cin >> eleccion;
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
                std::cout << "Item eliminado del inventario.\n";
            }
        }
        else 
        {
            std::cout << "Opción inválida.\n";
        }
    }

    int getAtaque() const 
    {
        if (armaEquipada) return ataqueBase + armaEquipada->getDano();
        return ataqueBase;
    }

    void mostrarEstado() const 
    {
        std::cout << nombre << " - Salud: " << salud << "/" << saludMaxima
            << ", Ataque: " << getAtaque() << ", Defensa: " << defensa
            << ", Oro: " << oro << std::endl;
    }

    bool estaVivo() const { return salud > 0; }
    void recibirDanio(int danio) 
    {
        int dañoReal = std::max(0, danio - defensa);
        salud -= dañoReal;
        if (salud < 0) salud = 0;
        std::cout << nombre << " recibió " << dañoReal << " de daño. Salud restante: " << salud << std::endl;
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

};

//Clase Enemigo
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Enemigo 
{
    std::string nombre;
    int salud;
    int ataque;
    int defensa;

public:
    Enemigo()
    {
        std::vector<std::string> nombres = { "Goblin", "Orco", "Troll", "Bandido", "Esqueleto", "Asesino" };
        nombre = nombres[generarNumeroAleatorio(0, nombres.size() - 1)];
        salud = generarNumeroAleatorio(80, 360);
        ataque = generarNumeroAleatorio(20, 65);
        defensa = generarNumeroAleatorio(0, 25);
    }

    virtual void mostrarEstado() const
    {
        std::cout << nombre << " - Salud: " << salud << ", Ataque: " << ataque << ", Defensa: " << defensa << std::endl;
    }

    bool estaVivo() const 
    {
        return salud > 0;
    }

    void recibirDanio(int danio)
    {
        int dañoReal = std::max(0, danio - defensa);
        salud -= dañoReal;
        if (salud < 0) salud = 0;
        std::cout << nombre << " recibió " << dañoReal << " de daño. Salud restante: " << salud << std::endl;

        system("pause");
    }

    int getAtaque() const { return ataque; }
    std::string getNombre() const { return nombre; }
};

//Clase mini Jefe
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class JefeFinal : public Enemigo {
private:
    std::string nombre;
    int salud;
    int ataque;
    int defensa;

public:

	JefeFinal() : nombre("Dragon"), salud(500), ataque(generarNumeroAleatorio(50,100)), defensa(40) {}
    
    void mostrarEstado() const override 
    {
        std::cout << "Jefe Final: " << nombre << "\n";
        std::cout << "Salud: " << salud << " | Ataque: " << ataque << " | Defensa: " << defensa << "\n";
    }
};

//Clase Tienda
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Tienda
{
    struct Producto
    {
        std::string nombre;
        std::string descripcion;
        int curacion;
        int precio;
    };

    struct ProductoArma
    {
        std::string nombre;
        int dano;
        int precio;
    };

    std::vector<Producto> stock =
    {
        { "Poción de Vida", "Restaura 50 HP", 50, 30 },
        { "Elixir Menor", "Restaura 30 HP", 30, 20 },
        { "Poción Mayor", "Restaura 100 HP", 100, 50 }
    };

    std::vector<ProductoArma> stockArmas =
    {
        { "Espada de Madera", generarNumeroAleatorio(20,50), 45},
        { "Espada de Hierro", generarNumeroAleatorio(30,65), 60},
        { "Espada de Diamante", generarNumeroAleatorio(40,80), 85},
        { "Hacha de Madera", generarNumeroAleatorio(30,40), 55},
        { "Hacha de Hierro", generarNumeroAleatorio(40,55), 70},
        { "Hacha de Diamante", generarNumeroAleatorio(50,70), 95}
    };

public:
    void comprar(Jugador& jugador)
    {
        int opcion;
        while (true)
        {
      
            std::cout << "--- Tienda ---\n";
            std::cout << "Oro disponible: " << jugador.getOro() << std::endl;
            std::cout << "1. Comprar pociones 2. Comprar armas 3. Salir Opción:\n";
            std::cin >> opcion;

            if (opcion == 1)
            {
                
                std::cout << "--- Pociones ---\n";
                for (size_t i = 0; i < stock.size(); ++i)
                {
                    std::cout << i + 1 << ". " << stock[i].nombre << " - " << stock[i].descripcion
                        << " | Cura: " << stock[i].curacion << " | Precio: " << stock[i].precio << " oro\n";
                }
                std::cout << stock.size() + 1 << ". Volver";
                std::cin >> opcion;
                if (opcion >= 1 && opcion <= static_cast<int>(stock.size()))
                {
                    const Producto& p = stock[opcion - 1];
                    if (jugador.gastarOro(p.precio))
                    {
                        jugador.agregarItem(new Pocion(p.nombre, 1, p.descripcion, p.curacion));
                        std::cout << "Has comprado " << p.nombre << "!\n";
                    }
                    else
                    {
                        std::cout << "No tienes suficiente oro.\n";
                    }
                }
            }
            else if (opcion == 2)
            {
               
                std::cout << "--- Armas ---\n";
                for (size_t i = 0; i < stockArmas.size(); ++i)
                {
                    std::cout << i + 1 << ". " << stockArmas[i].nombre << " | Daño: " << stockArmas[i].dano
                        << " | Precio: " << stockArmas[i].precio << " oro\n";
                }
                std::cout << stockArmas.size() + 1 << ". Volver";
                std::cin >> opcion;
                if (opcion >= 1 && opcion <= static_cast<int>(stockArmas.size()))
                {
                    const ProductoArma& a = stockArmas[opcion - 1];
                    if (jugador.gastarOro(a.precio))
                    {
                        jugador.agregarItem(new Arma(a.nombre, 1, a.dano));
                        std::cout << "Has comprado " << a.nombre << "!\n";
                    }
                    else
                    {
                        std::cout << "No tienes suficiente oro.\n";
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

    std::cout << "--- ¡Comienza el combate! ---\n";
    jugador.mostrarEstado();
    enemigo.mostrarEstado();

    while (jugador.estaVivo() && enemigo.estaVivo())
    {
        int opcion;

        std::cout << "Elige acción:  1. Atacar  2. Usar objeto\n";
        std::cin >> opcion;

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
            std::cout << "Opción inválida.\n";

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
        std::cout << "¡Has derrotado al " << enemigo.getNombre() << "! Ganaste " << oroGanado << " de oro.\n";
    }
    else
    {
        std::cout << "Perdiste Muak Muak Muuaaaaak\n";
        exit(0); // Termina el programa
    }
}


//Ya ahora si el jueguito
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
    Jugador jugador("Alfonzo");
    Tienda tienda;

    int option;
    int option2;
    int option3;
    int option4;
    int option5;
    int option6;
    int option7;

    std::string textoVacio;
    std::string Lento;

    Lento = "Despiertas en un lugar extraño.\n";
    imprimirLento(Lento, 40);

    do
    {

        Lento = "Decides 1- Levantarte   2- Seguir durmiendo.\n";
        imprimirLento(Lento, 30);
        std::cin >> option;

        //Forma parea que el codigo no se rompa en caso de ingresar una letra por chatsito GPT
        if (std::cin.fail())
        {
            std::cin.clear(); // Limpia el estado de error
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Descarta el resto de la línea
            std::cout << "opcion en silla de ruedas (invalida), intentalo de nuevo\n";
            continue;
        }

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

            std::cout << "opcion en silla de ruedas (invalida), intentalo de nuevo\n";
            break;
        }
    } while (option != 1 && option != 2);

    std::system("pause");
    std::system("cls");

    Lento = "Bueno, ya estamos levantaditos asi que \n";
    imprimirLento(Lento, 40);

    Lento = "...\n";
    imprimirLento(Lento, 1000);

    std::system("cls");

    Lento = "RWAAAAAAAAAAAAAAAAR\n";
    imprimirLento(Lento, 50);

    std::system("cls");

    Lento = "flush\n";
    imprimirLento(Lento, 20);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    Lento = "flush\n";
    imprimirLento(Lento, 20);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    Lento = "flush\n";
    imprimirLento(Lento, 20);

    std::system("pause");
    std::system("cls"),

        Lento = "WOW, viste ese dragon, nunca lo habia visto antes espero que no sea un problema\n";
    imprimirLento(Lento, 40);

    Lento = " Que dices? QUE QUE QUIERES HACER!?\n";
    imprimirLento(Lento, 40);

    Lento = "1- Ir a matarlo 2- Ir a matarlo 3- Ir a matarlo 4- Ir a matarlo n- Ir a matarlo\n";
    imprimirLento(Lento, 30);
    std::cin >> textoVacio;

    Lento = "Bueno, si tu lo dices ya que no hay otra opcion, vamos a matarlo\n";
    imprimirLento(Lento, 40);

    Lento = "Creo que hay una aldea más adelante, deberiamos ir pero tenemos que pasar por el bosque\n";
    imprimirLento(Lento, 40);

    Lento = "Pero solo hay un camino correcto...\n";
    imprimirLento(Lento, 40);

    do {                      //////////////////////////////////////////////// Enfrente /////////////////////////////////////////////////////////////////
        Enemigo enemigo;

        Lento = "Creo que el camino era dos para enfente, uno a la derecha, otro para enfrente y uno a la izquierda.\n";
        imprimirLento(Lento, 40);

        Lento = "Que quieres hacer? 1- Abrir Inventario   2- Ir para enfrente  3- Ir para la izquierda  4- Ir para la derecha.\n";
        imprimirLento(Lento, 30);
        std::cin >> option2;

        if (std::cin.fail())
        {
            std::cin.clear(); // Limpia el estado de error
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Descarta el resto de la línea
            std::cout << "opcion en silla de ruedas (invalida), intentalo de nuevo\n";
            continue;
        }

        switch (option2)
        {
        case 1:   ////////////////////// Ver inventario /////////////////////////////////////////////////////////////////

            jugador.mostrarInventario();
            break;

        case 2:  ////////////////////// Opcion correcta /////////////////////////////////////////////////////////////////

            break;

        case 3:  //////////////////// Opcion incorrecta /////////////////////////////////////////////////////////////////

            combate(jugador, enemigo);
            break;

        case 4:  ////////////////// Opcion incorrecta /////////////////////////////////////////////////////////////////

            combate(jugador, enemigo);
            break;

        default:  ////////////////// Opcion invalida /////////////////////////////////////////////////////////////////
            std::cout << "opcion en silla de ruedas (invalida), intentalo de nuevo\n";
            break;
        }

        break;

    } while (option2 == !2);

    Lento = "Bien\n";
    imprimirLento(Lento, 40);

    do {                      //////////////////////////////////////////////// Enfrente /////////////////////////////////////////////////////////////////
        Enemigo enemigo;

        Lento = "Que quieres hacer? 1- Abrir Inventario   2- Ir para enfrente  3- Ir para la izquierda  4- Ir para la derecha.\n";
        imprimirLento(Lento, 30);
        std::cin >> option3;

        if (std::cin.fail())
        {
            std::cin.clear(); // Limpia el estado de error
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Descarta el resto de la línea
            std::cout << "opcion en silla de ruedas (invalida), intentalo de nuevo\n";
            continue;
        }

        switch (option3)
        {
        case 1:  ////////////////////// Ver inventario /////////////////////////////////////////////////////////////////

            jugador.mostrarInventario();
            break;

        case 2:  //////////////////// Opcion correcta /////////////////////////////////////////////////////////////////

            break;

        case 3:  //////////////////// Opcion incorrecta /////////////////////////////////////////////////////////////////

            combate(jugador, enemigo);
            break;

        case 4:  //////////////////// Opcion incorrecta /////////////////////////////////////////////////////////////////


            combate(jugador, enemigo);
            break;

        default:  ////////////////// Opcion invalida /////////////////////////////////////////////////////////////////
            std::cout << "opcion en silla de ruedas (invalida), intentalo de nuevo\n";
            break;
        }
    } while (option3 == !2);

    Lento = "Sigue asi\n";
    imprimirLento(Lento, 40);

    do {                      //////////////////////////////////////////////// Derecha /////////////////////////////////////////////////////////////////
        Enemigo enemigo;

        Lento = "Que quieres hacer? 1- Abrir Inventario   2- Ir para enfrente  3- Ir para la izquierda  4- Ir para la derecha.\n";
        imprimirLento(Lento, 30);
        std::cin >> option4;

        if (std::cin.fail())
        {
            std::cin.clear(); // Limpia el estado de error
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Descarta el resto de la línea
            std::cout << "opcion en silla de ruedas (invalida), intentalo de nuevo\n";
            continue;
        }

        switch (option4)
        {
        case 1:  ////////////////////// Ver inventario /////////////////////////////////////////////////////////////////

            jugador.mostrarInventario();
            break;

        case 2:  //////////////////// Opcion correcta /////////////////////////////////////////////////////////////////

            combate(jugador, enemigo);
            break;

        case 3:  //////////////////// Opcion incorrecta /////////////////////////////////////////////////////////////////

            combate(jugador, enemigo);
            break;

        case 4:  //////////////////// Opcion incorrecta /////////////////////////////////////////////////////////////////

            break;

        default:  ////////////////// Opcion invalida /////////////////////////////////////////////////////////////////
            std::cout << "opcion en silla de ruedas (invalida), intentalo de nuevo\n";
            break;
        }
    } while (option4 == !4);

    Lento = "Ya casi llegamos\n";
    imprimirLento(Lento, 40);

    do {                      //////////////////////////////////////////////// Enfrente /////////////////////////////////////////////////////////////////
        Enemigo enemigo;

        Lento = "Que quieres hacer? 1- Abrir Inventario   2- Ir para enfrente  3- Ir para la izquierda  4- Ir para la derecha.\n";
        imprimirLento(Lento, 30);
        std::cin >> option5;

        if (std::cin.fail())
        {
            std::cin.clear(); // Limpia el estado de error
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Descarta el resto de la línea
            std::cout << "opcion en silla de ruedas (invalida), intentalo de nuevo\n";
            continue;
        }

        switch (option5)
        {
        case 1:  ////////////////////// Ver inventario /////////////////////////////////////////////////////////////////

            jugador.mostrarInventario();
            break;

        case 2:  //////////////////// Opcion correcta /////////////////////////////////////////////////////////////////

            break;

        case 3:  //////////////////// Opcion incorrecta /////////////////////////////////////////////////////////////////

            combate(jugador, enemigo);
            break;

        case 4:  //////////////////// Opcion incorrecta /////////////////////////////////////////////////////////////////


            combate(jugador, enemigo);
            break;

        default:  ////////////////// Opcion invalida /////////////////////////////////////////////////////////////////
            std::cout << "opcion en silla de ruedas (invalida), intentalo de nuevo\n";
            break;
        }
    } while (option == !2);

    Lento = "Falata solo uno más\n";
    imprimirLento(Lento, 40);


    do {                      //////////////////////////////////////////////// izquierda /////////////////////////////////////////////////////////////////
        Enemigo enemigo;

        Lento = "Que quieres hacer? 1- Abrir Inventario   2- Ir para enfrente  3- Ir para la izquierda  4- Ir para la derecha.\n";
        imprimirLento(Lento, 30);
        std::cin >> option6;

        if (std::cin.fail())
        {
            std::cin.clear(); // Limpia el estado de error
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Descarta el resto de la línea
            std::cout << "opcion en silla de ruedas (invalida), intentalo de nuevo\n";
            continue;
        }

        switch (option6)
        {
        case 1:  ////////////////////// Ver inventario /////////////////////////////////////////////////////////////////

            jugador.mostrarInventario();
            break;

        case 2:  //////////////////// Opcion correcta /////////////////////////////////////////////////////////////////

            combate(jugador, enemigo);
            break;

        case 3:  //////////////////// Opcion incorrecta /////////////////////////////////////////////////////////////////

            break;

        case 4:  //////////////////// Opcion incorrecta /////////////////////////////////////////////////////////////////

            combate(jugador, enemigo);
            break;

        default:  ////////////////// Opcion invalida /////////////////////////////////////////////////////////////////
            std::cout << "opcion en silla de ruedas (invalida), intentalo de nuevo\n";
            break;
        }
    } while (option6 == !3);

    Lento = "Bueno, ya que hemos llegado a la aldea\n";
    imprimirLento(Lento, 40);

    do { 
        Enemigo enemigo;

        Lento = "Que quieres hacer? 1- Abrir Inventario   2- Ir a la tienda  3- Ir a matar mountruos  4- Ir a matarc al dragon. \n";
        imprimirLento(Lento, 30);
        std::cin >> option7;

        if (std::cin.fail())
        {
            std::cin.clear(); // Limpia el estado de error
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Descarta el resto de la línea
            std::cout << "opcion en silla de ruedas (invalida), intentalo de nuevo\n";
            continue;
        }

        switch (option7)
        {
        case 1:  

            jugador.mostrarInventario();
            break;

        case 2:  

            tienda.comprar(jugador);
            break;

        case 3:  

            combate(jugador, enemigo);
            break;

        case 4:  

            break;

        default: 
            std::cout << "opcion en silla de ruedas (invalida), intentalo de nuevo\n";
            break;
        }
    } while (option7 == 1 || option7 == 2 || option == 3 || option == !4);

    JefeFinal dragon;
    combate(jugador, dragon);

	Lento = "Felicidades has ganado el juego, espero que te haya gustado, si quieres volver a jugar reinicia el programa.\n";
	imprimirLento(Lento, 40);

	return 0;
};