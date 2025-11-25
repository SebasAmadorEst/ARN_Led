#include <Arduino.h>

#define PIN_R 8
#define PIN_G 9
#define PIN_B 10

enum Color { ROJO, NEGRO };

struct Nodo {
    int dato;
    Color color;
    Nodo *izq, *der, *padre;
    Nodo(int val, Color c = ROJO) : dato(val), color(c), izq(nullptr), der(nullptr), padre(nullptr) {}
};

class ArbolRN {
private:
    Nodo* raiz = nullptr;

    void setColor(int r, int g, int b) {
        digitalWrite(PIN_R, r ? HIGH : LOW);
        digitalWrite(PIN_G, g ? HIGH : LOW);
        digitalWrite(PIN_B, b ? HIGH : LOW);
    }

    void flash(int r, int g, int b, const char* mensaje = "") {
        if (mensaje[0] != '\0') Serial.println(mensaje);
        setColor(r, g, b);
        delay(500);
        setColor(0, 0, 0);
    }

    Nodo* buscar(int val) {
        Nodo* actual = raiz;
        while (actual) {
            if (val == actual->dato) return actual;
            actual = (val < actual->dato) ? actual->izq : actual->der;
        }
        return nullptr;
    }

public:
    void apagar() { setColor(0, 0, 0); }

    void mostrarInsercion(int valor) {
        char msg[50];
        sprintf(msg, "Insertando nodo (%d) como ROJO", valor);
        flash(1, 0, 0, msg);
    }

    void mostrarInsercionSensor(int valor, bool esIluminacion) {
        const char* tipo = esIluminacion ? "sensorIluminacion" : "sensorTemperatura";
        const char* colorStr = esIluminacion ? "NEGRO" : "ROJO";

        char msg1[60], msg2[60];
        sprintf(msg1, "Nuevo dato de '%s' (%d) almacenado.", tipo, valor);
        sprintf(msg2, "Insertando \"%s\". Nodo es %s.", tipo, colorStr);

        Serial.println(msg1);
        Serial.println(msg2);

        if (esIluminacion) {
            flash(0, 1, 0, "");
        }
    }

    void mostrarColorNodo(Nodo* nodo, Color nuevoColor) {
        if (!nodo) return;
        char msg[50];
        const char* colorStr = (nuevoColor == ROJO) ? "ROJO" : "NEGRO";
        sprintf(msg, "Nodo (%d) recoloreado a %s", nodo->dato, colorStr);
        flash(nuevoColor == ROJO ? 1 : 0, nuevoColor == NEGRO ? 1 : 0, 0, msg);
    }

    void flashRotacionIzquierda()  { flash(0, 0, 1, "Rotación IZQUIERDA (azul)"); }
    void flashRotacionDerecha() {
        Serial.println(F("Rotación DERECHA (amarillo)"));
        digitalWrite(PIN_R, HIGH);
        analogWrite(PIN_G, 100);
        digitalWrite(PIN_B, LOW);
        delay(500);
        digitalWrite(PIN_R, LOW);
        analogWrite(PIN_G, 0);
        digitalWrite(PIN_B, LOW);
    }
    
    void rotarIzquierda(Nodo* x) {
        flashRotacionIzquierda();
        Nodo* y = x->der;
        x->der = y->izq;
        if (y->izq) y->izq->padre = x;
        y->padre = x->padre;
        if (!x->padre) raiz = y;
        else if (x == x->padre->izq) x->padre->izq = y;
        else x->padre->der = y;
        y->izq = x;
        x->padre = y;
    }

    void rotarDerecha(Nodo* y) {
        flashRotacionDerecha();
        Nodo* x = y->izq;
        y->izq = x->der;
        if (x->der) x->der->padre = y;
        x->padre = y->padre;
        if (!y->padre) raiz = x;
        else if (y == y->padre->der) y->padre->der = x;
        else y->padre->izq = x;
        x->der = y;
        y->padre = x;
    }

    void arreglarInsercion(Nodo* k) {
        while (k->padre && k->padre->color == ROJO) {
            if (k->padre == k->padre->padre->izq) {
                Nodo* tio = k->padre->padre->der;
                if (tio && tio->color == ROJO) {
                    k->padre->color = NEGRO;
                    tio->color = NEGRO;
                    k->padre->padre->color = ROJO;
                    mostrarColorNodo(k->padre, NEGRO);
                    mostrarColorNodo(tio, NEGRO);
                    mostrarColorNodo(k->padre->padre, ROJO);
                    k = k->padre->padre;
                } else {
                    if (k == k->padre->der) { k = k->padre; rotarIzquierda(k); }
                    k->padre->color = NEGRO;
                    k->padre->padre->color = ROJO;
                    mostrarColorNodo(k->padre, NEGRO);
                    mostrarColorNodo(k->padre->padre, ROJO);
                    rotarDerecha(k->padre->padre);
                }
            } else {
                Nodo* tio = k->padre->padre->izq;
                if (tio && tio->color == ROJO) {
                    k->padre->color = NEGRO;
                    tio->color = NEGRO;
                    k->padre->padre->color = ROJO;
                    mostrarColorNodo(k->padre, NEGRO);
                    mostrarColorNodo(tio, NEGRO);
                    mostrarColorNodo(k->padre->padre, ROJO);
                    k = k->padre->padre;
                } else {
                    if (k == k->padre->izq) { k = k->padre; rotarDerecha(k); }
                    k->padre->color = NEGRO;
                    k->padre->padre->color = ROJO;
                    mostrarColorNodo(k->padre, NEGRO);
                    mostrarColorNodo(k->padre->padre, ROJO);
                    rotarIzquierda(k->padre->padre);
                }
            }
        }
        if (raiz->color == ROJO) mostrarColorNodo(raiz, NEGRO);
        raiz->color = NEGRO;
    }

public:
    bool insertar(int val, Color colorDeseado = ROJO) {
        if (buscar(val) != nullptr) {
            Serial.print("Error: El valor ");
            Serial.print(val);
            Serial.println(" ya existe en el árbol.");
            flash(1, 0, 1, "Duplicado (no insertado)");
            return false;
        }

        bool esRaiz = (raiz == nullptr);

        if (colorDeseado == NEGRO) {
            Serial.println("Advertencia: No se permite nodo negro directo → se inserta como ROJO");
            flash(0, 1, 0, "");
        }

        mostrarInsercion(val);

        Nodo* nuevo = new Nodo(val, ROJO);
        Nodo* padre = nullptr;
        Nodo* actual = raiz;

        while (actual) {
            padre = actual;
            actual = (nuevo->dato < actual->dato) ? actual->izq : actual->der;
        }

        nuevo->padre = padre;
        if (!padre) raiz = nuevo;
        else if (nuevo->dato < padre->dato) padre->izq = nuevo;
        else padre->der = nuevo;

        if (esRaiz) {
            mostrarColorNodo(nuevo, NEGRO);
            raiz->color = NEGRO;
            return true;
        }

        arreglarInsercion(nuevo);
        return true;
    }

    bool insertarDesdeSensor(int val, bool esIluminacion) {
        if (buscar(val) != nullptr) {
            Serial.print("Error: Dato de sensor (");
            Serial.print(val);
            Serial.println(") ya existe.");
            flash(1, 0, 1, "Duplicado (sensor)");
            return false;
        }

        bool esRaiz = (raiz == nullptr);
        mostrarInsercionSensor(val, esIluminacion);
        mostrarInsercion(val);

        Nodo* nuevo = new Nodo(val, ROJO);
        Nodo* padre = nullptr;
        Nodo* actual = raiz;

        while (actual) {
            padre = actual;
            actual = (nuevo->dato < actual->dato) ? actual->izq : actual->der;
        }

        nuevo->padre = padre;
        if (!padre) raiz = nuevo;
        else if (nuevo->dato < padre->dato) padre->izq = nuevo;
        else padre->der = nuevo;

        if (esRaiz) {
            mostrarColorNodo(nuevo, NEGRO);
            raiz->color = NEGRO;
            return true;
        }

        arreglarInsercion(nuevo);
        return true;
    }

    void mostrarInOrden() {
        Serial.print("Árbol (in-order): ");
        if (!raiz) {
            Serial.println("(vacío)");
            return;
        }
        inOrden(raiz);
        Serial.println();
    }

private:
    void inOrden(Nodo* nodo) {
        if (!nodo) return;
        inOrden(nodo->izq);
        Serial.print(nodo->dato);
        Serial.print(nodo->color == ROJO ? "(R) " : "(N) ");
        inOrden(nodo->der);
    }

public:
    void borrarTodo() {
        while (raiz) {
            Nodo* temp = raiz;
            raiz = raiz->izq ? raiz->izq : raiz->der;
            delete temp;
        }
    }
};

ArbolRN arbol;
bool insercionAutomaticaActiva = false;
int opcionActual = 0;  // 4 = inserción aleatoria simple, 5 = simulación IoT

void flash(int r, int g, int b, const char* mensaje = "") {
    if (mensaje[0] != '\0') Serial.println(mensaje);
    digitalWrite(PIN_R, r ? HIGH : LOW);
    digitalWrite(PIN_G, g ? HIGH : LOW);
    digitalWrite(PIN_B, b ? HIGH : LOW);
    delay(500);
    digitalWrite(PIN_R, LOW);
    digitalWrite(PIN_G, LOW);
    digitalWrite(PIN_B, LOW);
}

void flashBlancoNuclear(const char* mensaje = "") {
    if (mensaje[0] != '\0') Serial.println(mensaje);
    digitalWrite(PIN_R, HIGH);
    analogWrite (PIN_G, 115);
    analogWrite (PIN_B, 255);
    delay(500);
    digitalWrite(PIN_R, LOW);
    analogWrite (PIN_G, 0);
    analogWrite (PIN_B, 0);
}

float leerNumeroReal(const char* mensaje) {
    while (true) {
        Serial.print(mensaje);
        while (Serial.available() == 0) delay(10);
        String entrada = Serial.readStringUntil('\n');
        entrada.trim();
        if (entrada.length() == 0) {
            Serial.println(F("ERROR: Entrada vacía."));
            continue;
        }
        bool tienePunto = false;
        bool valido = true;
        for (int i = 0; i < entrada.length(); i++) {
            char c = entrada[i];
            if (i == 0 && (c == '+' || c == '-')) continue;
            if (c == '.') {
                if (tienePunto || i == 0 || i == entrada.length()-1) { valido = false; break; }
                tienePunto = true;
                continue;
            }
            if (!isDigit(c)) { valido = false; break; }
        }
        if (valido) {
            float valor = entrada.toFloat();
            Serial.println(valor, 6);
            while (Serial.available() > 0) Serial.read();
            return valor;
        } else {
            Serial.println(F("\nERROR: Entrada inválida. Usa números reales: 25, -3.14, 0.5"));
            while (Serial.available() > 0) Serial.read();
        }
    }
}

int leerEnteroPositivo(const char* mensaje) {
    while (true) {
        Serial.print(mensaje);
        while (Serial.available() == 0) delay(10);
        int valor = Serial.parseInt();
        while (Serial.available()) Serial.read();
        if (valor >= 0) {
            Serial.println(valor);
            return valor;
        }
        Serial.println(F("ERROR: Valor no válido."));
    }
}

void setup() {
    Serial.begin(115200);
    randomSeed(analogRead(0));
    pinMode(PIN_R, OUTPUT);
    pinMode(PIN_G, OUTPUT);
    pinMode(PIN_B, OUTPUT);
    arbol.apagar();

    Serial.println(F("\n=== ÁRBOL ROJO-NEGRO - VERSIÓN FINAL IoT ==="));
    Serial.println(F("1. Insertar valor manual"));
    Serial.println(F("2. Insertar valor manual con sensores"));
    Serial.println(F("3. Eliminar árbol completo"));
    Serial.println(F("4. Iniciar inserción aleatoria (36-70)"));
    Serial.println(F("5. Iniciar simulación IoT (sensores + actuadores)"));
    Serial.println(F("6. Salir"));
}

void loop() {
    if (!insercionAutomaticaActiva) {
        Serial.println(F("\nElige una opción (1-6): "));
        int opcion = leerEnteroPositivo("");

        if (opcion == 1) {
            float valorReal = leerNumeroReal("Valor a insertar (puede ser decimal o negativo): ");
            int valorEntero = (valorReal == (int)valorReal) ? (int)valorReal : (int)round(valorReal);

            if (valorEntero < -999 || valorEntero > 999) {
                Serial.println(F("ERROR: Valor fuera de rango. Máximo 3 dígitos (-999 a 999)"));
                flashBlancoNuclear("Fuera de rango");
                arbol.mostrarInOrden();
                return;
            }

            if (valorReal != valorEntero) {
                Serial.print("Valor redondeado: ");
                Serial.println(valorEntero);
            } else {
                Serial.print("Valor entero: ");
                Serial.println(valorEntero);
            }

            int colorInt = leerEnteroPositivo("Color deseado (0=Rojo, 1=Negro): ");
            while (colorInt != 0 && colorInt != 1) {
                Serial.println(F("ERROR: Solo 0 o 1"));
                colorInt = leerEnteroPositivo("Color deseado (0=Rojo, 1=Negro): ");
            }
            arbol.insertar(valorEntero, colorInt == 1 ? NEGRO : ROJO);
            arbol.mostrarInOrden();

        } else if (opcion == 2) {
            float valorReal = leerNumeroReal("Valor del sensor (puede ser decimal o negativo): ");
            int valorEntero = (valorReal == (int)valorReal) ? (int)valorReal : (int)round(valorReal);

            if (valorEntero < -999 || valorEntero > 999) {
                Serial.println(F("ERROR: Valor del sensor fuera de rango (-999 a 999)"));
                flashBlancoNuclear("Sensor fuera de rango");
                arbol.mostrarInOrden();
                return;
            }

            if (valorReal != valorEntero) {
                Serial.print("Valor del sensor redondeado: ");
                Serial.println(valorEntero);
            } else {
                Serial.print("Valor del sensor (entero): ");
                Serial.println(valorEntero);
            }

            int tipoSensor = leerEnteroPositivo("Tipo de sensor (0=Temperatura, 1=Iluminacion): ");
            while (tipoSensor != 0 && tipoSensor != 1) {
                Serial.println(F("ERROR: Solo 0 o 1"));
                tipoSensor = leerEnteroPositivo("Tipo de sensor (0=Temperatura, 1=Iluminacion): ");
            }
            bool esIluminacion = (tipoSensor == 1);
            Serial.println(esIluminacion ? "Tipo: sensorIluminacion (NEGRO)" : "Tipo: sensorTemperatura (ROJO)");

            arbol.insertarDesdeSensor(valorEntero, esIluminacion);
            arbol.mostrarInOrden();

        } else if (opcion == 3) {
            arbol.borrarTodo();
            Serial.println("¡Árbol eliminado completamente!");

        } else if (opcion == 4) {
            Serial.println("¡Inserción aleatoria simple iniciada! (36-70)");
            Serial.println("Presiona 's' para detener");
            insercionAutomaticaActiva = true;
            opcionActual = 4;

        } else if (opcion == 5) {
            Serial.println("¡Simulación IoT iniciada!");
            Serial.println("Sensores: Temperatura (0-40), Iluminación (0-50)");
            Serial.println("Actuadores: Bomba (>30), Lámpara (<15)");
            Serial.println("Presiona 's' para detener");
            insercionAutomaticaActiva = true;
            opcionActual = 5;

        } else if (opcion == 6) {
            Serial.println("¡Chao!");
            while (true) delay(1000);
        }
    }

    // Modo inserción aleatoria simple (opción 4)
    if (insercionAutomaticaActiva && opcionActual == 4) {
        if (Serial.available()) {
            char c = Serial.read();
            if (c == 's' || c == 'S') {
                Serial.println("\nInserción aleatoria detenida.");
                insercionAutomaticaActiva = false;
                opcionActual = 0;
                return;
            }
        }
        int valor = random(36, 71);
        Serial.print("\n[Auto simple] Insertando: ");
        Serial.println(valor);
        arbol.insertar(valor, ROJO);
        arbol.mostrarInOrden();
        delay(1200);
    }

    // Modo simulación IoT (opción 5)
    if (insercionAutomaticaActiva && opcionActual == 5) {
        if (Serial.available()) {
            char c = Serial.read();
            if (c == 's' || c == 'S') {
                Serial.println("\nSimulación IoT detenida.");
                insercionAutomaticaActiva = false;
                opcionActual = 0;
                return;
            }
        }

        int temp = random(0, 41);
        int ilum = random(0, 51);

        Serial.print(F("\n[IoT] Temp: "));
        Serial.print(temp);
        Serial.print(F(" | Ilum: "));
        Serial.print(ilum);
        Serial.println();

        bool bombaActivada = false;
        bool lamparaActivada = false;

        if (temp > 30) {
            Serial.println("¡TEMPERATURA ALTA! → Activando BOMBA");
            arbol.insertarDesdeSensor(temp, false);
            bombaActivada = true;
        }

        if (ilum < 15) {
            Serial.println("¡ILUMINACIÓN BAJA! → Activando LÁMPARA");
            arbol.insertarDesdeSensor(ilum, true);
            lamparaActivada = true;
        }

        // Solo insertamos si algún actuador se activó
        if (!bombaActivada && !lamparaActivada) {
            Serial.println(F("   [OK] Todo en rango normal. Sin acción."));
        }

        arbol.mostrarInOrden();
        delay(2000);
    }
}