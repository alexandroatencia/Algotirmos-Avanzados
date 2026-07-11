#include <iostream>
#include <vector>
#include <map>
#include <ctime>
#include <cmath>
#include <algorithm>

// --- CONFIGURACIÓN DEL ALGORITMO GENÉTICO (Macros descriptivas) ---
#define TOTAL_GENERACIONES 10
#define TAMANO_POBLACION 20
#define PORCENTAJE_PADRES 0.5
#define PUNTO_CORTE_CRUCE 0.5
#define PROB_MUTACION_BIT 0.3

using namespace std;

// 1. FUNCIÓN DE FITNESS (Aptitud)
// Calcula el valor total de la mochila sumando los valores de los items seleccionados
int calculafitness(vector<int> cromo, int* valores) {
    int suma_valor = 0;
    for(int i = 0; i < cromo.size(); i++) {
        suma_valor += cromo[i] * valores[i];
    }
    return suma_valor;
}

// 2. ABERRACIÓN (Filtro de Restricciones)
// Verifica si un individuo viola la regla de peso máximo
bool aberracion(vector<int> ind, int limite_peso, int* valores) {
    // El profesor evalúa el peso usando la misma función calculafitness.
    // Si la suma supera el límite, es una aberración (solución inválida).
    return calculafitness(ind, valores) > limite_peso;
}

// 3. MOSTRAR POBLACIÓN
void muestrapoblacion(vector<vector<int>> poblacion, int* valores) {
    for(int i = 0; i < poblacion.size(); i++) {
        cout << "Individuo " << i << ": ";
        for(int j = 0; j < poblacion[i].size(); j++) {
            cout << poblacion[i][j] << " ";
        }
        cout << " | Fitness = " << calculafitness(poblacion[i], valores) << endl;
    }
}

// Convierte el cromosoma binario a decimal (usado para identificar clones)
int decimal(vector<int> ind) {
    int num = 0;
    for (int i = 0; i < ind.size(); i++)
        num += ind[i] * pow(2, i);
    return num;
}

// 4. MATA CLONES (Mantiene la diversidad)
void mataclon(vector<vector<int>> &poblacion) {
    map<int, vector<int>> indunicos;

    // Al usar el valor decimal como llave, el map elimina automáticamente los duplicados
    for(int i = 0; i < poblacion.size(); i++) {
        int dec = decimal(poblacion[i]);
        indunicos[dec] = poblacion[i];
    }
    
    poblacion.clear();
    for(auto it = indunicos.begin(); it != indunicos.end(); it++) {
        poblacion.push_back(it->second);
    }
}

// 5. GENERAR POBLACIÓN INICIAL
void generapoblacion(vector<vector<int>> &poblacion, int* valores, int largo_cromosoma, int limite_peso) {
    int cont = 0;
    srand(time(NULL));
    while (cont < TAMANO_POBLACION) {
        vector<int> individuo;
        for (int i = 0; i < largo_cromosoma; i++) {
            individuo.push_back(rand() % 2); // 0 o 1
        }
        // Solo se agrega a la población si NO es aberración
        if (!aberracion(individuo, limite_peso, valores)) {
            poblacion.push_back(individuo);
            cont++;
        }
    }
}

// Funciones auxiliares para la ruleta
void calculasupervivencia(vector<vector<int>> poblacion, vector<int> &supervivencia, int* valores) {
    int sumafitness = 0;

    for(int i = 0; i < poblacion.size(); i++)
        sumafitness += calculafitness(poblacion[i], valores);
        
    for (int i = 0; i < poblacion.size(); i++) {
        int superfit = 0;
        if (sumafitness > 0) {
            superfit = round(100 * (double)calculafitness(poblacion[i], valores) / sumafitness);
        }
        supervivencia.push_back(superfit);
    }
}

void cargaruleta(vector<int> supervivencia, int* ruleta) {
    int cont = 0;
    for (int i = 0; i < supervivencia.size(); i++) {
        for (int j = 0; j < supervivencia[i]; j++) {
            ruleta[cont++] = i;
        }
    }
}

// 6. SELECCIÓN (Ruleta)
void seleccion(vector<vector<int>> poblacion, vector<vector<int>> &padres, int* valores, int largo_cromosoma, int limite_peso) {
    int ruleta[100]{-1}; // Inicializa con -1
    vector<int> supervivencia;
    
    calculasupervivencia(poblacion, supervivencia, valores);
    cargaruleta(supervivencia, ruleta);
    
    int npadres = round(poblacion.size() * PORCENTAJE_PADRES);

    for (int i = 0; i < npadres; i++) {
        int ticket = rand() % 100;
        if (ruleta[ticket] != -1) {
            padres.push_back(poblacion[ruleta[ticket]]);
        }
    }
}

// 7. CRUCE (Crossover)
void creahijo(vector<int> padre, vector<int> madre, vector<int> &hijo) {
    int punto_corte = round(padre.size() * PUNTO_CORTE_CRUCE);

    // Primera mitad del padre
    for (int i = 0; i < punto_corte; i++)
        hijo.push_back(padre[i]);
    // Segunda mitad de la madre
    for (int i = punto_corte; i < madre.size(); i++)
        hijo.push_back(madre[i]);
}

void casamiento(vector<vector<int>> &poblacion, vector<vector<int>> padres, int* valores, int limite_peso) {
    for(int i = 0; i < padres.size(); i++) {
        for(int j = 0; j < padres.size(); j++) {
            if (i != j) { // No cruzar al padre consigo mismo
                vector<int> hijo;
                creahijo(padres[i], padres[j], hijo);
                
                // El hijo sobrevive solo si NO es aberración
                if (!aberracion(hijo, limite_peso, valores))
                    poblacion.push_back(hijo);
            }
        }
    }
}

// 8. REGENERAR POBLACIÓN (Mantiene tamaño y aplica elitismo)
void regenerapoblacion(vector<vector<int>> &poblacion, int* valores, int limite_peso){
    mataclon(poblacion);
    
    // --- 🐛 BUG CORREGIDO AQUÍ ---
    // En lugar de iterar sumando el fitness repetidas veces, 
    // se llama directo al cálculo final para ordenar de mayor a menor
    sort(poblacion.begin(), poblacion.end(),
         [valores](const vector<int>& a, const vector<int>& b) {
            return calculafitness(a, valores) > calculafitness(b, valores);
         });
         
    // Corta los peores excedentes para mantener el tamaño original
    if (poblacion.size() > TAMANO_POBLACION)
        poblacion.erase(poblacion.begin() + TAMANO_POBLACION, poblacion.end());
}

// 9. MUTACIÓN
void mutacion(vector<vector<int>> &poblacion, vector<vector<int>> padres, int* valores, int limite_peso) {
    int num_mutaciones = round(padres[0].size() * PROB_MUTACION_BIT);
    
    for (int i = 0; i < padres.size(); i++) {
        int cont = 0;
        while (cont < num_mutaciones) {
            int gen = rand() % padres[i].size();
            // Invierte un gen al azar
            padres[i][gen] = (padres[i][gen] == 0) ? 1 : 0;
            cont++;
        }
        // Si mutar no generó aberración, lo sumamos a la población
        if (!aberracion(padres[i], limite_peso, valores))
            poblacion.push_back(padres[i]);
    }
}

// 10. INVERSIÓN (Mutación Extrema)
void inversion(vector<vector<int>> &poblacion, vector<vector<int>> padres, int* valores, int limite_peso) {
    for (int i = 0; i < padres.size(); i++) {
        for (int j = 0; j < padres[i].size(); j++) {
            // Invierte todo el cromosoma entero
            padres[i][j] = (padres[i][j] == 0) ? 1 : 0;
        }
        if (!aberracion(padres[i], limite_peso, valores))
            poblacion.push_back(padres[i]);
    }
}

// Muestra al mejor individuo de la generación
int muestramejor(vector<vector<int>> poblacion, int* valores, int limite_peso){
    int mejor = 0;
    for(int i = 0; i < poblacion.size(); i++) {
        if(calculafitness(poblacion[mejor], valores) < calculafitness(poblacion[i], valores))
            mejor = i;
    }

    cout << endl << "🏆 La mejor solucion tiene Fitness = " << calculafitness(poblacion[mejor], valores) << endl;
    cout << "🧬 Cromosoma: ";
    for(int i = 0; i < poblacion[mejor].size(); i++)
        cout << poblacion[mejor][i] << " ";

    cout << endl;
    return limite_peso - calculafitness(poblacion[mejor], valores);
}

// --- ALGORITMO PRINCIPAL AG ---
void mochilaAG(int* valores, int largo_cromosoma, int limite_peso) {
    vector<vector<int>> poblacion;
    
    cout << "=== POBLACION INICIAL ===" << endl;
    generapoblacion(poblacion, valores, largo_cromosoma, limite_peso);
    muestrapoblacion(poblacion, valores);
    cout << endl;

    // BUCLE EVOLUTIVO
    for(int i = 0; i < TOTAL_GENERACIONES; i++) {
        cout << "--- GENERACION " << i + 1 << " ---" << endl;
        vector<vector<int>> padres;
        
        seleccion(poblacion, padres, valores, largo_cromosoma, limite_peso);
        casamiento(poblacion, padres, valores, limite_peso);
        mataclon(poblacion); // Eliminar repetidos ayuda a mantener diversidad
        mutacion(poblacion, padres, valores, limite_peso);
        inversion(poblacion, padres, valores, limite_peso);

        regenerapoblacion(poblacion, valores, limite_peso); // Cortar a los peores
        
        muestrapoblacion(poblacion, valores);
        muestramejor(poblacion, valores, limite_peso);
        cout << endl;
    }
}

int main() {
    // Array que representa el peso/valor de cada item (Mochila Simplificada)
    int valores[] = {1, 2, 2, 12, 4}; 
    int largo_cromosoma = sizeof(valores) / sizeof(valores[0]);
    int limite_peso = 15;

    mochilaAG(valores, largo_cromosoma, limite_peso);

    return 0;
}
