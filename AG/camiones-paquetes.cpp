#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <iomanip>

using namespace std;

// --- Parámetros definidos según las instrucciones del Laboratorio 5 ---
#define TOTAL_GENERACIONES 5    // Máximo de iteraciones permitidas [cite: 33]
#define TAMANHO_POBLACION 10    // Población inicial de 10 individuos [cite: 32]
#define PORCENTAJE_PADRES 0.3   // Tasa de selección del 30% [cite: 32]
#define PUNTO_CORTE_CRUCE 0.5   // Tasa de casamiento del 50% [cite: 32]
#define PROB_MUTACION_BIT 0.3   // Probabilidad de mutación (mantenida de la plantilla)

// ESTRATEGIA: Evalúa la aptitud del individuo. Calcula el peso total de los paquetes asignados
// a todos los camiones. Al maximizar el peso transportado, minimizamos el espacio sin usar.
int calcular_fitness(const vector<int>& individuo, int* paquetes, int num_paquetes, int num_camiones) {
    int suma = 0; // Acumulador del peso total que llevan todos los camiones juntos

    // Recorremos cada camión uno por uno
    for (int t = 0; t < num_camiones; t++) {
        // Por cada camión, revisamos todos los paquetes disponibles
        for (int p = 0; p < num_paquetes; p++) {
            // Fórmula mágica: t * num_paquetes + p. Encuentra el gen exacto en el arreglo 1D.
            // Si ese gen es 1, significa que el paquete 'p' está dentro del camión 't'.
            if (individuo[t * num_paquetes + p] == 1) {
                suma += paquetes[p]; // Sumamos el peso de ese paquete al total
            }
        }
    }
    return suma; // Retornamos el peso total. A mayor peso, mejor es la solución.
}

// ESTRATEGIA: Verifica que el cromosoma sea una solución válida. Retorna 'true' si es inválida (aberración)
// por dos motivos: 1) Un camión excede su capacidad máxima. 2) Un paquete está clonado en varios camiones.
bool aberracion(const vector<int>& individuo, int* paquetes, int num_paquetes, int* camiones, int num_camiones) {
    // Prueba 1: ¿Algún camión explotó por exceso de peso?
    for (int t = 0; t < num_camiones; t++) {
        int uso = 0; // Peso que lleva el camión actual
        for (int p = 0; p < num_paquetes; p++) {
            // Buscamos si el paquete 'p' está en el camión 't'
            if (individuo[t * num_paquetes + p] == 1) uso += paquetes[p];
        }
        // Si el peso acumulado supera lo que soporta el camión, es una aberración
        if (uso > camiones[t]) return true;
    }

    // Prueba 2: ¿Algún paquete es un fantasma que está en dos camiones a la vez?
    for (int p = 0; p < num_paquetes; p++) {
        int cuenta = 0; // Contará en cuántos camiones está metido el paquete actual
        for (int t = 0; t < num_camiones; t++) {
            // Buscamos el paquete 'p' en el camión 't'
            if (individuo[t * num_paquetes + p] == 1) cuenta++;
        }
        // Si el mismo paquete está en más de 1 camión, es una clonación imposible (aberración)
        if (cuenta > 1) return true;
    }

    // Si pasa ambas pruebas sin activar el "return true", es una solución válida.
    return false;
}

// ESTRATEGIA: Transforma el valor del fitness en un porcentaje de supervivencia sobre el total de la población.
void calcular_supervivencia(const vector<vector<int>>& poblacion, vector<int>& supervivencia, int *paquetes, int num_paquetes, int num_camiones) {
    int suma_fitness = 0; // Suma de los puntajes de todos los individuos juntos

    // Sumamos el fitness de todos para saber cuál es el 100%
    for (int i = 0; i < poblacion.size(); i++) {
        suma_fitness += calcular_fitness(poblacion[i], paquetes, num_paquetes, num_camiones);
    }

    // Calculamos qué porcentaje le toca a cada individuo
    for (int i = 0; i < poblacion.size(); i++) {
        int super_fitness = 0;
        if (suma_fitness > 0) {
            // Regla de 3 simple: (Mi_Fitness * 100) / Fitness_Total
            super_fitness = round(100 * (double) calcular_fitness(poblacion[i], paquetes, num_paquetes, num_camiones) / suma_fitness);
        }
        supervivencia.push_back(super_fitness); // Guardamos su porcentaje (ej: 15%)
    }
}

// ESTRATEGIA: Llena un arreglo de 100 posiciones con los índices de los individuos según su % de supervivencia.
void cargar_ruleta(const vector<int>& supervivencia, int* ruleta) {
    int contador = 0; // Índice para llenar las 100 casillas de la ruleta

    for (int i = 0; i < supervivencia.size() && contador < 100; i++) {
        // Si el individuo 'i' tiene 15% de supervivencia, le damos 15 casillas en la ruleta
        for (int j = 0; j < supervivencia[i] && contador < 100; j++) {
            ruleta[contador++] = i;
        }
    }
    // Rellenamos con -1 las casillas vacías en caso de que los redondeos no sumen 100 exactos
    while (contador < 100) ruleta[contador++] = -1;
}

// ESTRATEGIA: Selecciona a los padres girando la ruleta aleatoria en base a la tasa de selección.
void seleccion(const vector<vector<int>>& poblacion, vector<vector<int>>& padres, int* paquetes, int num_paquetes, int* camiones, int num_camiones) {
    int ruleta[100];
    for (int i = 0; i < 100; i++) ruleta[i] = -1; // Vaciamos la ruleta
    vector<int> supervivencia;

    // Preparamos la ruleta con los porcentajes actuales
    calcular_supervivencia(poblacion, supervivencia, paquetes, num_paquetes, num_camiones);
    cargar_ruleta(supervivencia, ruleta);

    // Cantidad de padres a elegir: 30% de la población (10 * 0.3 = 3 padres) [cite: 32]
    int n_padres = round(poblacion.size() * PORCENTAJE_PADRES);

    for (int i = 0; i < n_padres; i++) {
        int ticket = rand() % 100; // Tiramos la bolita: número aleatorio entre 0 y 99
        if (ruleta[ticket] != -1) {
            // El individuo dueño de esa casilla gana y se convierte en padre
            padres.push_back(poblacion[ruleta[ticket]]);
        }
    }
}

// ESTRATEGIA: Genera individuos con genes binarios (0 y 1) de forma aleatoria hasta alcanzar el tamaño de población.
void generar_poblacion(vector<vector<int>>& poblacion, int* paquetes, int num_paquetes, int* camiones, int num_camiones) {
    int contador = 0;
    // El largo del cromosoma es la multiplicación de camiones y paquetes
    int tam_cromosoma = num_paquetes * num_camiones;

    // Repetimos hasta tener 10 individuos válidos
    while (contador < TAMANHO_POBLACION) {
        vector<int> individuo;
        for (int i = 0; i < tam_cromosoma; i++) {
            individuo.push_back(rand() % 2); // Genera un 0 o un 1 al azar
        }
        // Solo lo aceptamos si NO es una aberración (es decir, respeta la física)
        if (not aberracion(individuo, paquetes, num_paquetes, camiones, num_camiones)) {
            poblacion.push_back(individuo);
            contador++;
        }
    }
}

// ESTRATEGIA: Junta la primera mitad del padre con la segunda mitad de la madre.
void crear_hijo(const vector<int>& padre, const vector<int>& madre, vector<int>& hijo) {
    // Calculamos el punto exacto donde cortar (50% de la longitud del cromosoma) [cite: 32]
    int punto_corte = round(padre.size() * PUNTO_CORTE_CRUCE);

    // Copiamos la primera parte del padre al hijo
    for (int i = 0; i < punto_corte; i++) hijo.push_back(padre[i]);
    // Copiamos la segunda parte de la madre al hijo
    for (int i = punto_corte; i < madre.size(); i++) hijo.push_back(madre[i]);
}

// ESTRATEGIA: Cruza a los padres seleccionados. Si los hijos resultantes son soluciones válidas, los añade a la población.
void casamiento(vector<vector<int>>& poblacion, vector<vector<int>>&padres, int* paquetes, int num_paquetes, int* camiones, int num_camiones) {
    // Todos los padres se cruzan con todos los demás padres
    for (int i = 0; i < padres.size(); i++) {
        for (int j = 0; j < padres.size(); j++) {
            if (i != j) { // Evitamos que un padre se cruce consigo mismo
                vector<int> hijo;
                crear_hijo(padres[i], padres[j], hijo); // Generamos la mezcla

                // Si el hijo no rompe las reglas físicas, entra a la población
                if (not aberracion(hijo, paquetes, num_paquetes, camiones, num_camiones))
                    poblacion.push_back(hijo);
            }
        }
    }
}

// ESTRATEGIA: Convierte el cromosoma a base decimal usando álgebra de bits para poder usarlo como llave de identificación.
int decimal(const vector<int>& individuo) {
    int numero = 0;
    for (int i = 0; i < individuo.size(); i++) {
        // Matemática binaria: multiplica el gen (0 o 1) por 2 elevado a la posición 'i'
        numero += individuo[i] * pow(2, i);
    }
    return numero;
}

// ESTRATEGIA: Remueve individuos duplicados mapeando su valor decimal en un diccionario, garantizando diversidad.
void mata_clon(vector<vector<int>>& poblacion) {
    map<int, vector<int>> individuo_unico; // Diccionario que no permite llaves repetidas

    for (int i = 0; i < poblacion.size(); i++) {
        int num_decimal = decimal(poblacion[i]); // Convertimos a número
        individuo_unico[num_decimal] = poblacion[i]; // Si hay clones, se chancan (sobrescriben)
    }

    poblacion.clear(); // Vaciamos la población vieja con clones
    // La rellenamos solo con los sobrevivientes únicos del diccionario
    for (auto it = individuo_unico.begin(); it != individuo_unico.end(); it++) {
        poblacion.push_back(it->second);
    }
}

// ESTRATEGIA: Introduce mutaciones aleatorias invirtiendo bits en los padres para explorar nuevas soluciones.
void mutacion(vector<vector<int>>& poblacion, vector<vector<int>>& padres, int* paquetes, int num_paquetes, int* camiones, int num_camiones) {
    int num_mutaciones = round(padres.size() * PROB_MUTACION_BIT); // Calculamos cuántas mutaciones hacer
    for (int i = 0; i < padres.size(); i++) {
        int contador = 0;
        while (contador < num_mutaciones) {
            int gen = rand() % padres[i].size(); // Elegimos un gen al azar
            // Si es 0 lo vuelve 1, si es 1 lo vuelve 0
            padres[i][gen] = (padres[i][gen] == 0) ? 1 : 0;
            contador++;
        }
        // Si el mutante sigue siendo válido, entra a la población
        if (not aberracion(padres[i], paquetes, num_paquetes, camiones, num_camiones)) {
            poblacion.push_back(padres[i]);
        }
    }
}

// ESTRATEGIA: Invierte completamente todos los bits de un cromosoma. Agrega diversidad extrema.
void inversion(vector<vector<int>>& poblacion, vector<vector<int>>& padres, int* paquetes, int num_paquetes, int* camiones, int num_camiones) {
    for (int i = 0; i < padres.size(); i++) {
        for (int j = 0; j < padres[i].size(); j++) {
            // Voltea absolutamente todo el cromosoma (los 0s a 1s y los 1s a 0s)
            padres[i][j] = (padres[i][j] == 0) ? 1 : 0;
        }
        // Solo entra si tiene sentido físico
        if (not aberracion(padres[i], paquetes, num_paquetes, camiones, num_camiones)) {
            poblacion.push_back(padres[i]);
        }
    }
}

// ESTRATEGIA: Ordena a la población de mejor a peor según su fitness y recorta excedentes para mantener el límite poblacional.
void regenera_poblacion(vector<vector<int>>& poblacion, int* paquetes, int num_paquetes, int* camiones, int num_camiones) {
    mata_clon(poblacion); // Quitamos repetidos primero

    // Función sort de C++: Ordena poniendo a los de mayor fitness primero
    sort(poblacion.begin(), poblacion.end(), [paquetes, num_paquetes, num_camiones](const vector<int>& a, const vector<int>& b) {
        return calcular_fitness(a, paquetes, num_paquetes, num_camiones) > calcular_fitness(b, paquetes, num_paquetes, num_camiones);
    });

    // Si la población creció a más de 10 por los hijos, matamos a los peores (los del final de la lista)
    if (poblacion.size() > TAMANHO_POBLACION) {
        poblacion.erase(poblacion.begin() + TAMANHO_POBLACION, poblacion.end());
    }
}

// ESTRATEGIA: Imprime los resultados finales mostrando el formato solicitado en el examen
// y calculando el desperdicio considerando a todos los camiones.
void muestra_mejor(vector<vector<int>>& poblacion, int* paquetes, int num_paquetes, int* camiones, int num_camiones) {
    int mejor = 0; // Índice del mejor individuo

    // Buscamos quién tiene el puntaje más alto
    for (int i = 0; i < poblacion.size(); i++) {
        if (calcular_fitness(poblacion[mejor], paquetes, num_paquetes, num_camiones) < calcular_fitness(poblacion[i], paquetes, num_paquetes, num_camiones))
            mejor = i; // Encontramos un nuevo campeón
    }

    cout << "\n=== RESPUESTA OPTIMIZADA ===\n";
    // setw es para que las columnas salgan alineadas y bonitas
    cout << setw(10) << "Camiones" << setw(15) << "Paquetes\n";

    int suma_capacidad_total = 0;
    int desperdicio_total = 0;

    // Imprimimos camión por camión
    for (int t = 0; t < num_camiones; t++) {
        cout << setw(10) << t + 1 << setw(8) << " ";
        int uso_camion = 0;
        bool primero = true; // Variable para poner las comitas (,) de forma correcta

        for (int p = 0; p < num_paquetes; p++) {
            // Volvemos a usar la fórmula para leer el genoma del campeón
            if (poblacion[mejor][t * num_paquetes + p] == 1) {
                if (!primero) cout << ","; // Imprime coma si no es el primer paquete
                cout << p + 1; // Imprime el número de paquete (1 al 6)
                uso_camion += paquetes[p];
                primero = false;
            }
        }
        cout << endl; // Salto de línea para el siguiente camión
        suma_capacidad_total += camiones[t]; // Acumulamos capacidad total para el cálculo final
    }

    // Desperdicio total = Capacidad de todos los camiones - Lo que realmente logramos meter
    desperdicio_total = suma_capacidad_total - calcular_fitness(poblacion[mejor], paquetes, num_paquetes, num_camiones);
    cout << "\nEspacio total desperdiciado: " << desperdicio_total << " Kg." << endl; // [cite: 21]
}

// ESTRATEGIA: Motor del Algoritmo Genético. Inicializa, cruza, muta y evoluciona la población durante el N° de iteraciones.
void mochila_AG(int* paquetes, int num_paquetes, int* camiones, int num_camiones) {
    srand(time(NULL)); // Semilla aleatoria
    vector<vector<int>> poblacion;

    cout << "=== PROCESANDO ALGORITMO GENETICO ===" << endl;

    // 1. Crear el grupo inicial
    generar_poblacion(poblacion, paquetes, num_paquetes, camiones, num_camiones);

    // 2. Hacerlos evolucionar por 5 iteraciones [cite: 33]
    for (int i = 0; i < TOTAL_GENERACIONES; i++) {
        vector<vector<int>> padres;

        seleccion(poblacion, padres, paquetes, num_paquetes, camiones, num_camiones);
        casamiento(poblacion, padres, paquetes, num_paquetes, camiones, num_camiones);
        mata_clon(poblacion);
        mutacion(poblacion, padres, paquetes, num_paquetes, camiones, num_camiones);
        inversion(poblacion, padres, paquetes, num_paquetes, camiones, num_camiones);
        regenera_poblacion(poblacion, paquetes, num_paquetes, camiones, num_camiones);
    }

    // 3. Mostrar el mejor resultado
    muestra_mejor(poblacion, paquetes, num_paquetes, camiones, num_camiones);
}

int main() {
    // Entradas correctas según la tabla del PDF de TUPIACU S.A.C [cite: 20, 24, 26]
    int paquetes[] = {150, 100, 180, 50, 120, 10};
    int num_paquetes = sizeof(paquetes) / sizeof(paquetes[0]); // Truco para sacar la cantidad de elementos

    int camiones[] = {250, 200, 200, 100};
    int num_camiones = sizeof(camiones) / sizeof(camiones[0]);

    // Arrancamos el algoritmo
    mochila_AG(paquetes, num_paquetes, camiones, num_camiones);

    return 0; // Fin del programa
}