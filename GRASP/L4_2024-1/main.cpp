#include <iostream>
#include <climits>
#include <algorithm>
#include <ctime>
#include <vector>
using namespace std;
#define ITERACIONES 100000
#define ALPHA 0.3

struct Tabla {
    int id;
    int capacidad;
    int velocidad;
};

struct Disco {
    int id;
    int capacidad;
    int velocidad_original;
    int velocidad_residual;
    vector<Tabla> tablas;
};

bool compara_tabla(const Tabla& a, const Tabla& b) {
    return a.velocidad > b.velocidad;
}

int buscar_indice_tabla(const vector<Tabla>& v_tabla, double limite_RCL) {
    int indice = 0;
    for (int i = 0; i < v_tabla.size(); i++) {
        if (v_tabla[i].velocidad >= limite_RCL) {
            indice++;
        }
    }
    return (indice == 0) ? 1 : indice;
}

bool compara_disco(const Disco& a, const Disco& b) {
    return a.velocidad_residual > b.velocidad_residual;
}

int buscar_indice_disco(const vector<Disco>& v_disco, double limite_RCL, int velocidad_requerida) {
    int indice = 0;
    for (int i = 0; i < v_disco.size(); i++) {
        if (v_disco[i].velocidad_residual >= limite_RCL and v_disco[i].velocidad_residual >= velocidad_requerida) {
            indice++;
        }
    }
    return (indice == 0) ? 1 : indice;
}

void grasp_almacenamiento(Tabla* tabla, int n_tabla, Disco* disco, int n_disco) {
    srand(time(NULL));
    vector<Disco> v_disco_solucion;
    int maximizar_minimo = INT_MIN;

    for (int i = 0; i < ITERACIONES; i++) {
        vector<Tabla> v_tabla(tabla, tabla + n_tabla);
        vector<Disco> v_disco(disco, disco + n_disco);
        bool solucion_valida = true;

        sort(v_tabla.begin(), v_tabla.end(), compara_tabla);
        while (not v_tabla.empty()) {
            int beta_tabla = v_tabla.front().velocidad;
            int tau_tabla = v_tabla.back().velocidad;
            double limite_RCL_tabla = beta_tabla - ALPHA * (beta_tabla - tau_tabla);
            int indice_RCL_tabla = buscar_indice_tabla(v_tabla, limite_RCL_tabla);
            int indice_random_tabla = rand() % indice_RCL_tabla;
            Tabla tabla_actual = v_tabla[indice_random_tabla];

            sort(v_disco.begin(), v_disco.end(), compara_disco);
            if (v_disco.front().velocidad_residual < tabla_actual.velocidad) {
                solucion_valida = false;
                break;
            }

            int beta_disco = v_disco.front().velocidad_residual;
            int tau_disco = v_disco.back().velocidad_residual;
            double limite_RCL_disco = beta_disco - ALPHA * (beta_disco - tau_disco);
            int indice_RCL_disco = buscar_indice_disco(v_disco, limite_RCL_disco, tabla_actual.velocidad);
            int indice_random_disco = rand() % indice_RCL_disco;
            Disco disco_actual = v_disco[indice_random_disco];

            v_disco[indice_random_disco].tablas.push_back(tabla_actual);
            v_disco[indice_random_disco].velocidad_residual -= tabla_actual.velocidad;
            v_tabla.erase(v_tabla.begin() + indice_random_tabla);
        }
        if (solucion_valida) {
            int velocidad_minima_parcial = INT_MAX;
            for (int i = 0; i < v_disco.size(); i++) {
                if (velocidad_minima_parcial > v_disco[i].velocidad_residual) {
                    velocidad_minima_parcial = v_disco[i].velocidad_residual;
                }
            }
            if (maximizar_minimo < velocidad_minima_parcial) {
                maximizar_minimo = velocidad_minima_parcial;
                v_disco_solucion.clear();
                v_disco_solucion.insert(v_disco_solucion.begin(), v_disco.begin(), v_disco.end());
            }
        }
    }
    cout << "Disco  |  Tablas\n";
    for (int i = 0; i < v_disco_solucion.size(); i++) {
        sort(v_disco_solucion.begin(), v_disco_solucion.end(),
            [](const Disco& a, const Disco& b) {return a.id < b.id; });
        cout << "  " << v_disco_solucion[i].id << "    |   ";
        for (int j = 0; j < v_disco_solucion[i].tablas.size(); j++) {
            cout << v_disco_solucion[i].tablas[j].id;
            if (j < v_disco_solucion[i].tablas.size() - 1) {
                cout << ", ";
            }
        }
        cout << endl;
    }
    cout << "Velocidad minima de grupo de " << maximizar_minimo << " IOPs\n";
}

int main() {
    Tabla tabla[] = {
        {1,  20, 150},
        {2,  10, 100},
        {3,  15,  80},
        {4, 100,  50},
        {5,  50, 120},
        {6, 100,  10},
    };
    Disco disco[] = {
        {1, 800, 250, 250, {}},
        {2, 750, 200, 200, {}},
        {3, 850, 200, 200, {}},
    };
    int n_tabla = sizeof(tabla) / sizeof(tabla[0]);
    int n_disco = sizeof(disco) / sizeof(disco[0]);

    grasp_almacenamiento(tabla, n_tabla, disco, n_disco);
    return 0;
}