#include <iostream>
#include <ctime>
#include <vector>
#include <algorithm>
#include <limits>
using namespace std;
#define ITERACIONES 1000
#define ALPHA 0.3

struct Envio {
    char ciudad;
    int costo;
};

struct Motorizado {
    int id;
    int costo;
};

struct Transporte {
    char ciudad;
    vector<Motorizado> motorizado;
};

struct Solucion {
    char ciudad;
    int motorizado;
};

void ordenar_ciudades_paralelo(Envio* envio, Transporte* transporte, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (envio[j].costo < envio[j + 1].costo) {
                swap(envio[j], envio[j + 1]);
                swap(transporte[j], transporte[j + 1]);
            }
        }
    }
}

bool compara(const Motorizado& a, const Motorizado& b) {
    return a.costo < b.costo;
}

void imprimir_transporte(const vector<Transporte>& v_transporte) {
    for (int i = 0; i < v_transporte.size(); i++) {
        cout << "Ciudad: " << v_transporte[i].ciudad << " => ";
        for (int j = 0; j < v_transporte[i].motorizado.size(); j++) {
            cout << "("<< v_transporte[i].motorizado[j].id << ", " << v_transporte[i].motorizado[j].costo << ") ";
        }
        cout << endl;
    }
}

int buscar_indice_envio(const vector<Envio>& v_envio, double limite_RCL_envio) {
    int indice = 0;
    for (int i = 0; i < v_envio.size(); i++) {
        if (v_envio[i].costo >= limite_RCL_envio) {
            indice++;
        }
    }
    return (indice == 0) ? 1 : indice;
}

int buscar_indice(const vector<Motorizado>& motorizado, double limite_RCL) {
    int indice = 0;
    for (int i = 0; i < motorizado.size(); i++) {
        if (motorizado[i].costo <= limite_RCL) {
            indice++;
        }
    }
    return (indice == 0) ? 1 : indice;
}

void grasp_empresa_courier(Envio* envio, int n_envio, Transporte* transporte, int n_transporte, int K) {
    srand(time(NULL));
    vector<Solucion> v_mejor_solucion;
    int mejor_solucion = INT_MAX;

    ordenar_ciudades_paralelo(envio, transporte, n_envio);
    for (int i = 0; i < n_transporte; i++) {
        sort(transporte[i].motorizado.begin(), transporte[i].motorizado.end(), compara);
    }

    for (int i = 0; i < ITERACIONES; i++) {
        int limite_costo = K;
        vector<Envio> v_envio;
        v_envio.insert(v_envio.begin(), envio, envio + n_envio);
        vector<Transporte> v_transporte;
        v_transporte.insert(v_transporte.begin(), transporte, transporte + n_transporte);
        // imprimir_transporte(v_transporte);

        vector<Solucion> v_solucion;
        int solucion = 0;
        int solucion_valida = true;
        while (not v_envio.empty()) {
            int beta_envio = v_envio.front().costo;
            int tau_envio = v_envio.back().costo;
            double limite_RCL_envio = beta_envio - ALPHA * (beta_envio - tau_envio);
            int indice_RCL_envio = buscar_indice_envio(v_envio, limite_RCL_envio);
            int indice_random_envio = rand() % indice_RCL_envio;

            if (v_transporte[indice_random_envio].ciudad == v_envio[indice_random_envio].ciudad) {
                bool asignado = false;
                while (not v_transporte[indice_random_envio].motorizado.empty()) {
                    int beta = v_transporte[indice_random_envio].motorizado.front().costo;
                    int tau = v_transporte[indice_random_envio].motorizado.back().costo;
                    double limite_RCL = beta + ALPHA * (tau - beta);
                    int indice_RCL = buscar_indice(v_transporte[indice_random_envio].motorizado, limite_RCL);
                    int indice_random = rand() % indice_RCL;

                    int costo_transporte = v_envio[indice_random_envio].costo;
                    int costo_motorizado = v_transporte[indice_random_envio].motorizado[indice_random].costo;
                    if (costo_transporte + costo_motorizado <= limite_costo) {
                        solucion += costo_transporte + costo_motorizado;
                        v_solucion.push_back({v_envio[indice_random_envio].ciudad, v_transporte[indice_random_envio].motorizado[indice_random].id});
                        v_envio.erase(v_envio.begin() + indice_random_envio);
                        v_transporte.erase(v_transporte.begin() + indice_random_envio);
                        asignado = true;
                        break;
                    }
                    v_transporte[indice_random_envio].motorizado.erase(v_transporte[indice_random_envio].motorizado.begin() + indice_random);
                }
                if (asignado == false) {
                    solucion_valida = false;
                    break;
                }
            }
        }
        if (solucion_valida and mejor_solucion > solucion) {
            mejor_solucion = solucion;
            v_mejor_solucion.clear();
            v_mejor_solucion.insert(v_mejor_solucion.begin(), v_solucion.begin(), v_solucion.end());
        }
    }
    cout << "Ciudad | Motorizado\n";
    for (int i = 0; i < v_mejor_solucion.size(); i++) {
        cout << "| " << v_mejor_solucion[i].ciudad << "    |   " << v_mejor_solucion[i].motorizado << endl;
    }
    cout << "Costo Total: " << mejor_solucion << endl;
}

int main() {
    Envio envio[] = {
        {'A', 16},
        {'B', 15},
        {'C', 12},
        {'D', 18}
    };
    Transporte transporte[] = {
        {'A', {{1, 3}, {2, 5}, {3, 7}, {4, 8}, {5, 4}}},
        {'B', {{1, 4}, {2, 2}, {3, 5}, {4, 7}, {5, 3}}},
        {'C', {{1, 5}, {2, 3}, {3, 4}, {4, 5}, {5, 7}}},
        {'D', {{1, 6}, {2, 4}, {3, 2}, {4, 1}, {5, 5}}}
    };
    int n_envio = sizeof(envio) / sizeof(envio[0]);
    int n_transporte = sizeof(transporte) / sizeof(transporte[0]);
    int K = 20;

    grasp_empresa_courier(envio, n_envio, transporte, n_transporte, K);
    return 0;
}