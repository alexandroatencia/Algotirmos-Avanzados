#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <iomanip>

using namespace std;

// --- Parámetros definidos según las instrucciones del Laboratorio 5 ---
#define TOTAL_GENERACIONES 1000  // Máximo de iteraciones
#define TAMANHO_POBLACION 20     // Población inicial de 20 individuos
#define PORCENTAJE_PADRES 0.3    // Tasa de selección de 30%
#define PUNTO_CORTE_CRUCE 0.5    // Tasa de casamiento de 50%
#define PROB_MUTACION_BIT 0.3

// Estructura para almacenar de forma limpia la información de cada proyecto
struct Proyecto {
    int id;
    int costo;
    int ganancia;
    vector<int> predecesores;
};

// ESTRATEGIA: Calcula la aptitud (fitness) sumando las ganancias de los proyectos seleccionados.
// Se busca maximizar esta función objetivo.
int calcular_fitness(const vector<int>& individuo, const vector<Proyecto>& proyectos) {
    int ganancia_total = 0;
    for (int i = 0; i < individuo.size(); i++) {
        if (individuo[i] == 1) {
            ganancia_total += proyectos[i].ganancia;
        }
    }
    return ganancia_total;
}

// ESTRATEGIA: Evalúa si un individuo es inválido (aberración). Falla si excede el presupuesto P
// o si intenta seleccionar un proyecto sin haber seleccionado previamente a todos sus predecesores.
bool aberracion(const vector<int>& individuo, const vector<Proyecto>& proyectos, int presupuesto) {
    int costo_total = 0;

    for (int i = 0; i < individuo.size(); i++) {
        if (individuo[i] == 1) {
            costo_total += proyectos[i].costo;

            // Verificación de precedencias (los IDs empiezan en 1, los índices del arreglo en 0)
            for (int j = 0; j < proyectos[i].predecesores.size(); j++) {
                int id_predecesor = proyectos[i].predecesores[j];
                int indice_predecesor = id_predecesor - 1; // Ajuste de índice
                
                // Si el predecesor no está seleccionado (0), es una solución inválida
                if (individuo[indice_predecesor] == 0) {
                    return true; 
                }
            }
        }
    }
    
    // Verificación de presupuesto
    if (costo_total > presupuesto) {
        return true; 
    }

    return false; // Es una solución válida
}

// ESTRATEGIA: Convierte el fitness absoluto en una probabilidad de supervivencia relativa al total de la población.
void calcular_supervivencia(const vector<vector<int>>& poblacion, vector<int>& supervivencia, const vector<Proyecto>& proyectos) {
    int suma_fitness = 0;
    for (int i = 0; i < poblacion.size(); i++) {
        suma_fitness += calcular_fitness(poblacion[i], proyectos);
    }

    for (int i = 0; i < poblacion.size(); i++) {
        int super_fitness = 0;
        if (suma_fitness > 0) {
            super_fitness = round(100 * (double) calcular_fitness(poblacion[i], proyectos) / suma_fitness);
        }
        supervivencia.push_back(super_fitness);
    }
}

// ESTRATEGIA: Carga una ruleta (arreglo de 100 posiciones) distribuyendo tickets proporcionales al % de supervivencia.
void cargar_ruleta(const vector<int>& supervivencia, int* ruleta) {
    int contador = 0;
    for (int i = 0; i < supervivencia.size() && contador < 100; i++) {
        for (int j = 0; j < supervivencia[i] && contador < 100; j++) {
            ruleta[contador++] = i;
        }
    }
    while (contador < 100) ruleta[contador++] = -1;
}

// ESTRATEGIA: Selecciona aleatoriamente a los padres mediante el método de la ruleta, basándose en la tasa indicada (30%).
void seleccion(const vector<vector<int>>& poblacion, vector<vector<int>>& padres, const vector<Proyecto>& proyectos, int presupuesto) {
    int ruleta[100];
    for (int i = 0; i < 100; i++) ruleta[i] = -1;
    vector<int> supervivencia;

    calcular_supervivencia(poblacion, supervivencia, proyectos);
    cargar_ruleta(supervivencia, ruleta);

    int n_padres = round(poblacion.size() * PORCENTAJE_PADRES);

    for (int i = 0; i < n_padres; i++) {
        int ticket = rand() % 100;
        if (ruleta[ticket] != -1) {
            padres.push_back(poblacion[ruleta[ticket]]);
        }
    }
}

// ESTRATEGIA: Genera individuos binarios (0 y 1) de forma aleatoria, descartando aquellos que son aberraciones hasta completar la población.
void generar_poblacion(vector<vector<int>>& poblacion, const vector<Proyecto>& proyectos, int n, int presupuesto) {
    int contador = 0;
    while (contador < TAMANHO_POBLACION) {
        vector<int> individuo;
        for (int i = 0; i < n; i++) {
            individuo.push_back(rand() % 2); // 0 o 1
        }
        if (not aberracion(individuo, proyectos, presupuesto)) {
            poblacion.push_back(individuo);
            contador++;
        }
    }
}

// ESTRATEGIA: Realiza el cruce de un solo punto, combinando genes del padre y de la madre.
void crear_hijo(const vector<int>& padre, const vector<int>& madre, vector<int>& hijo) {
    int punto_corte = round(padre.size() * PUNTO_CORTE_CRUCE);

    for (int i = 0; i < punto_corte; i++) hijo.push_back(padre[i]);
    for (int i = punto_corte; i < madre.size(); i++) hijo.push_back(madre[i]);
}

// ESTRATEGIA: Combina todos los padres seleccionados entre sí, y si el hijo es físicamente posible, lo añade a la población.
void casamiento(vector<vector<int>>& poblacion, vector<vector<int>>&padres, const vector<Proyecto>& proyectos, int presupuesto) {
    for (int i = 0; i < padres.size(); i++) {
        for (int j = 0; j < padres.size(); j++) {
            if (i != j) {
                vector<int> hijo;
                crear_hijo(padres[i], padres[j], hijo);

                if (not aberracion(hijo, proyectos, presupuesto))
                    poblacion.push_back(hijo);
            }
        }
    }
}

// ESTRATEGIA: Convierte el cromosoma binario a base decimal usando álgebra de potencias para usarlo como identificador único.
int decimal(const vector<int>& individuo) {
    int numero = 0;
    for (int i = 0; i < individuo.size(); i++) {
        numero += individuo[i] * pow(2, i);
    }
    return numero;
}

// ESTRATEGIA: Usa un map para eliminar cromosomas duplicados (clones) y mantener la diversidad de soluciones.
void mata_clon(vector<vector<int>>& poblacion) {
    map<int, vector<int>> individuo_unico;
    for (int i = 0; i < poblacion.size(); i++) {
        int num_decimal = decimal(poblacion[i]);
        individuo_unico[num_decimal] = poblacion[i];
    }

    poblacion.clear();
    for (auto it = individuo_unico.begin(); it != individuo_unico.end(); it++) {
        poblacion.push_back(it->second);
    }
}

// ESTRATEGIA: Invierte bits aleatorios en los padres para explorar nuevas áreas del espacio de búsqueda.
void mutacion(vector<vector<int>>& poblacion, vector<vector<int>>& padres, const vector<Proyecto>& proyectos, int presupuesto) {
    int num_mutaciones = round(padres.size() * PROB_MUTACION_BIT);
    for (int i = 0; i < padres.size(); i++) {
        int contador = 0;
        while (contador < num_mutaciones) {
            int gen = rand() % padres[i].size();
            padres[i][gen] = (padres[i][gen] == 0) ? 1 : 0;
            contador++;
        }
        if (not aberracion(padres[i], proyectos, presupuesto)) {
            poblacion.push_back(padres[i]);
        }
    }
}

// ESTRATEGIA: Invierte completamente todos los bits del cromosoma para diversificación profunda.
void inversion(vector<vector<int>>& poblacion, vector<vector<int>>& padres, const vector<Proyecto>& proyectos, int presupuesto) {
    for (int i = 0; i < padres.size(); i++) {
        for (int j = 0; j < padres[i].size(); j++) {
            padres[i][j] = (padres[i][j] == 0) ? 1 : 0;
        }
        if (not aberracion(padres[i], proyectos, presupuesto)) {
            poblacion.push_back(padres[i]);
        }
    }
}

// ESTRATEGIA: Elimina clones, ordena la población por mejor fitness y poda los excedentes para mantener la población fija en 20.
void regenera_poblacion(vector<vector<int>>& poblacion, const vector<Proyecto>& proyectos, int presupuesto) {
    mata_clon(poblacion);

    sort(poblacion.begin(), poblacion.end(), [proyectos](const vector<int>& a, const vector<int>& b) {
        return calcular_fitness(a, proyectos) > calcular_fitness(b, proyectos);
    });

    if (poblacion.size() > TAMANHO_POBLACION) {
        poblacion.erase(poblacion.begin() + TAMANHO_POBLACION, poblacion.end());
    }
}

// ESTRATEGIA: Busca el mejor cromosoma de la generación final y formatea la salida según los requerimientos del examen.
void muestra_mejor(vector<vector<int>>& poblacion, const vector<Proyecto>& proyectos, int presupuesto) {
    int mejor = 0;
    for (int i = 0; i < poblacion.size(); i++) {
        if (calcular_fitness(poblacion[mejor], proyectos) < calcular_fitness(poblacion[i], proyectos))
            mejor = i;
    }

    cout << "\n=== RESULTADO DEL AG ===" << endl;
    cout << "Proyectos seleccionados: ";
    for (int i = 0; i < poblacion[mejor].size(); i++) {
        if (poblacion[mejor][i] == 1) {
            cout << proyectos[i].id;
        }
    }
    
    cout << "\nBeneficio total: " << calcular_fitness(poblacion[mejor], proyectos) << " (Millones de $)\n";
}

// ESTRATEGIA: Motor principal que ejecuta el ciclo de vida del algoritmo genético.
void algoritmo_genetico(const vector<Proyecto>& proyectos, int n, int presupuesto) {
    srand(time(NULL));
    vector<vector<int>> poblacion;

    cout << "Generando poblacion inicial..." << endl;
    generar_poblacion(poblacion, proyectos, n, presupuesto);

    for (int i = 0; i < TOTAL_GENERACIONES; i++) {
        vector<vector<int>> padres;

        seleccion(poblacion, padres, proyectos, presupuesto);
        casamiento(poblacion, padres, proyectos, presupuesto);
        mata_clon(poblacion);
        mutacion(poblacion, padres, proyectos, presupuesto);
        inversion(poblacion, padres, proyectos, presupuesto);
        regenera_poblacion(poblacion, proyectos, presupuesto);
    }
    
    muestra_mejor(poblacion, proyectos, presupuesto);
}

int main() {
    // Definimos los datos exactos que aparecen en la tabla del Laboratorio 5.
    vector<Proyecto> proyectos = {
        {1, 100, 200, {}},
        {2,  50, 300, {1}},
        {3, 150, 300, {1}},
        {4,  50, 400, {}},
        {5,  50, 200, {4}},
        {6, 150, 800, {2, 4}},
        {7, 100, 250, {}}
    };
    
    int n = proyectos.size();
    int presupuesto = 400; // Presupuesto asignado P=400 (Millones de $)

    algoritmo_genetico(proyectos, n, presupuesto);
    
    return 0;
}