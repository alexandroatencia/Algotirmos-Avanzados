#include <iostream>
#include <climits>
#include <vector>
#include <ctime>
#include <algorithm>
using namespace std;
#define ITERACIONES 10000
#define ALPHA 0.3

struct Tabla {
    int id;
    int capacidad;
    int velocidad;
};

struct Disco {
    int id;
    int capacidad;
    int velocidad_residual;
    int velocidad;
    vector<int> tablas;
};

bool comparar_tablas(const Tabla& a, const Tabla& b) {
    return a.velocidad > b.velocidad;
}

int buscar_indice_tabla(const vector<Tabla>& v_tablas, double limite_RCL_tabla) {
    int indice = 0;
    for (int i = 0; i < v_tablas.size(); i++) {
        if (v_tablas[i].velocidad >= limite_RCL_tabla) {
            indice++;
        }
    }
    return (indice == 0) ? 1 : indice;
}

bool compara_discos(const Disco& a, const Disco& b) {
    return a.velocidad_residual > b.velocidad_residual;
}

int buscar_indice_disco(const vector<Disco>& v_disco, double limite_RCL_disco) {
    int indice = 0;
    for (int i = 0; i < v_disco.size(); i++) {
        if (v_disco[i].velocidad >= limite_RCL_disco) {
            indice++;
        }
    }
    return (indice == 0) ? 1 : indice;
}

void grasp_discos_tablas(Tabla* tablas, int n_tablas, Disco* discos, int n_discos) {
    srand(time(NULL));
    vector<Disco> v_discos_solucion;
    int menor_velocidad_disco = INT_MIN;

    sort(tablas, tablas + n_tablas, comparar_tablas);
    for (int i = 0; i < ITERACIONES; i++) {
        vector<Tabla> v_tablas(tablas, tablas + n_tablas);
        vector<Disco> v_discos(discos, discos + n_discos);
        int velocidad_menor_parcial = INT_MAX;
        while (not v_tablas.empty()) {
            double beta_tabla = v_tablas.front().velocidad;
            double tau_tabla = v_tablas.back().velocidad;
            double limite_RCL_tabla = beta_tabla - ALPHA * (beta_tabla - tau_tabla);
            int indice_RCL_tabla = buscar_indice_tabla(v_tablas, limite_RCL_tabla);
            int indice_random_tabla = rand() % indice_RCL_tabla;

            sort(v_discos.begin(), v_discos.end(), compara_discos);
            double beta_disco = v_discos.front().velocidad_residual;
            double tau_disco = v_discos.back().velocidad_residual;
            double limite_RCL_disco = beta_disco - ALPHA * (beta_disco - tau_disco);
            int indice_RCL_disco = buscar_indice_disco(v_discos, limite_RCL_disco);
            int indice_random_disco = rand() % indice_RCL_disco;

            if (v_discos[indice_random_disco].tablas.empty()) {
                v_discos[indice_random_disco].velocidad_residual -= v_tablas[indice_random_tabla].velocidad;
                v_discos[indice_random_disco].tablas.push_back(v_tablas[indice_random_tabla].id);
                v_tablas.erase(v_tablas.begin() + indice_random_tabla);
                continue;
            }

            if (v_discos[indice_random_disco].velocidad_residual >= v_tablas[indice_random_tabla].velocidad) {
                v_discos[indice_random_disco].velocidad_residual -= v_tablas[indice_random_tabla].velocidad;
                v_discos[indice_random_disco].tablas.push_back(v_tablas[indice_random_tabla].id);
                v_tablas.erase(v_tablas.begin() + indice_random_tabla);
            }

        }
        for (int k = 0; k < v_discos.size(); k++) {
            if (v_discos[k].velocidad_residual < velocidad_menor_parcial) {
                velocidad_menor_parcial = v_discos[k].velocidad_residual;
            }
        }
        if (menor_velocidad_disco < velocidad_menor_parcial) {
            menor_velocidad_disco = velocidad_menor_parcial;
            v_discos_solucion.clear();
            v_discos_solucion.insert(v_discos_solucion.begin(), v_discos.begin(), v_discos.end());
        }
    }
    sort(v_discos_solucion.begin(), v_discos_solucion.end(), [](const Disco& a, const Disco& b) {
        return a.id < b.id;
    });
    cout << "Disco    |      Tablas" << endl;
    cout << "-------------------------" << endl;
    for (int i = 0; i < v_discos_solucion.size(); i++) {
        cout << v_discos_solucion[i].id << "        |       ";
        for (int j = 0; j < v_discos_solucion[i].tablas.size(); j++) {
            cout << v_discos_solucion[i].tablas[j];
            if (j < v_discos_solucion[i].tablas.size() - 1) {
                cout << ", ";
            }
        }
        cout << endl;
    }
}

int main() {
    Tabla tablas[] = {
        {1,  20, 150},
        {2,  10, 100},
        {3,  15,  80},
        {4, 100,  50},
        {5,  50, 120},
        {6, 100,  10},
    };
    Disco discos[] = {
        {1, 800, 250, 250},
        {2, 750, 200, 200},
        {3, 850, 200, 200},
    };
    int n_tablas = sizeof(tablas) / sizeof(Tabla);
    int n_discos = sizeof(discos)/sizeof(Disco);

    grasp_discos_tablas(tablas, n_tablas, discos, n_discos);
    return 0;
}



