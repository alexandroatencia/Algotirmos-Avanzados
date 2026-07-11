#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <ctime>

// --- CONFIGURACIÓN DEL ALGORITMO GENÉTICO (Perillas de Control) ---
#define TOTAL_GENERACIONES 10    // Cuántas veces va a evolucionar la población
#define TAMANO_POBLACION 4       // Cuántos individuos/soluciones viven a la vez
#define LARGO_CROMOSOMA 6        // Cuántos bits (genes) tiene cada individuo

#define PORCENTAJE_PADRES 0.5    // Qué fracción de la población pasa a ser padres (50%)
#define PUNTO_CORTE_CRUCE 0.5    // En qué punto del cromosoma se combinan los genes (a la mitad)
#define PROB_MUTACION_BIT 0.1    // Qué tan probable es que un bit cambie por azar (10%)

using namespace std;

// 1. FUNCIÓN DE FITNESS (Aptitud)
// Queremos contar cuántos '1' tiene el cromosoma (a más unos, mejor solución)
int calculafitness(vector<int> cromo) {
    int puntos = 0;
    for(int i = 0; i < cromo.size(); i++) {
        puntos += cromo[i]; 
    }
    return puntos;
}

// 2. MOSTRAR POBLACIÓN
void muestrapoblacion(vector<vector<int>> poblacion) {
    for(int i = 0; i < poblacion.size(); i++) {
        cout << "Individuo " << i << ": ";
        for(int j = 0; j < poblacion[i].size(); j++) {
            cout << poblacion[i][j] << " ";
        }
        cout << " | Fitness = " << calculafitness(poblacion[i]) << endl;
    }
}

// 3. GENERAR POBLACIÓN INICIAL ALEATORIA
void generapoblacion(vector<vector<int>> &poblacion) {
    for (int i = 0; i < TAMANO_POBLACION; i++) {
        vector<int> individuo;
        for (int j = 0; j < LARGO_CROMOSOMA; j++) {
            individuo.push_back(rand() % 2); // Genera 0 o 1 al azar
        }
        poblacion.push_back(individuo);
    }
}

// 4. SELECCIÓN (Método de la Ruleta)
void seleccion(vector<vector<int>> poblacion, vector<vector<int>> &padres) {
    int sumafitness = 0;
    for(int i = 0; i < poblacion.size(); i++) {
        sumafitness += calculafitness(poblacion[i]);
    }

    // Si el fitness total es 0, elegimos padres al azar
    if(sumafitness == 0) {
        for(int i = 0; i < TAMANO_POBLACION * PORCENTAJE_PADRES; i++) {
            padres.push_back(poblacion[rand() % poblacion.size()]);
        }
        return;
    }

    // Elegimos la cantidad de padres requerida
    int npadres = round(poblacion.size() * PORCENTAJE_PADRES);
    for (int p = 0; p < npadres; p++) {
        int dardo = rand() % sumafitness; // Lanzamos un dardo a la ruleta
        int suma_actual = 0;
        
        for(int i = 0; i < poblacion.size(); i++) {
            suma_actual += calculafitness(poblacion[i]);
            if(dardo < suma_actual) {
                padres.push_back(poblacion[i]); // Individuo elegido
                break;
            }
        }
    }
}

// 5. CRUCE O CASAMIENTO (Crossover)
void creahijo(vector<int> padre, vector<int> madre, vector<int> &hijo) {
    int punto_corte = round(padre.size() * PUNTO_CORTE_CRUCE); 

    // Mitad del padre
    for (int i = 0; i < punto_corte; i++)
        hijo.push_back(padre[i]);
    // Mitad de la madre
    for (int i = punto_corte; i < madre.size(); i++)
        hijo.push_back(madre[i]);
}

void casamiento(vector<vector<int>> &poblacion, vector<vector<int>> padres) {
    // Cruzamos a los padres seleccionados entre sí para generar nuevos hijos
    for(int i = 0; i < padres.size(); i++) {
        for(int j = i + 1; j < padres.size(); j++) {
            vector<int> hijo1, hijo2;
            creahijo(padres[i], padres[j], hijo1);
            creahijo(padres[j], padres[i], hijo2); // Cruce recíproco
            
            poblacion.push_back(hijo1);
            poblacion.push_back(hijo2);
        }
    }
}

// 6. MUTACIÓN
void mutacion(vector<vector<int>> &poblacion) {
    for (int i = 0; i < poblacion.size(); i++) {
        for (int j = 0; j < poblacion[i].size(); j++) {
            double probabilidad = (double)(rand() % 100) / 100.0;
            if (probabilidad < PROB_MUTACION_BIT) {
                // Invertimos el bit
                poblacion[i][j] = (poblacion[i][j] == 0) ? 1 : 0;
            }
        }
    }
}

// 7. REGENERAR POBLACIÓN (Mantener a los mejores y el tamaño fijo)
void regenerapoblacion(vector<vector<int>> &poblacion) {
    // Ordenamos de mayor a menor fitness
    sort(poblacion.begin(), poblacion.end(), [](const vector<int>& a, const vector<int>& b) {
        return calculafitness(a) > calculafitness(b);
    });

    // Cortamos los peores excedentes
    if (poblacion.size() > TAMANO_POBLACION) {
        poblacion.erase(poblacion.begin() + TAMANO_POBLACION, poblacion.end());
    }
}

// --- PROGRAMA PRINCIPAL ---
int main() {
    srand(time(NULL)); // Inicializa la semilla aleatoria para que cada ejecución sea distinta

    vector<vector<int>> poblacion;
    
    cout << "=== GENERANDO POBLACION INICIAL ===" << endl;
    generapoblacion(poblacion);
    muestrapoblacion(poblacion);
    cout << "-----------------------------------" << endl << endl;

    // BUCLE EVOLUTIVO
    for(int i = 0; i < TOTAL_GENERACIONES; i++) {
        cout << "=== ITERACION " << i + 1 << " ===" << endl;
        
        vector<vector<int>> padres;
        
        seleccion(poblacion, padres);
        casamiento(poblacion, padres);
        mutacion(poblacion);
        regenerapoblacion(poblacion);
        
        muestrapoblacion(poblacion);
        cout << "Mejor fitness de la iteracion: " << calculafitness(poblacion[0]) << endl;
        cout << "-----------------------------------" << endl << endl;
    }

    return 0;
}