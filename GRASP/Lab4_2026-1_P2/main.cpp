#include <algorithm>
#include <iostream>
#include <vector>
#include <ctime>
#include <climits>
using namespace std;
#define ITERACIONES 1000
#define ALPHA 0.2

using namespace std;

struct Pedido {
    char id;
    int peso;
};

struct Camion {
    int id;
    int peso;
    vector<Pedido> pedido;
};

bool compara(const Pedido& a, const Pedido& b) {
    return a.peso > b.peso;
}

bool compara_camion(const Camion& a, const Camion& b) {
    return a.peso > b.peso;
}

int buscar_indice(const vector<Pedido>& v_pedido, double limite_RCL) {
    int indice = 0;
    for (int i = 0; i < v_pedido.size(); i++) {
        if (v_pedido[i].peso >= limite_RCL) {
            indice++;
        }
    }
    return (indice == 0) ? 1 : indice;
}

int buscar_indice_camion(const vector<Camion>& v_camion, double limite_RCL_camion) {
    int indice = 0;
    for (int i = 0; i < v_camion.size(); i++) {
        if (v_camion[i].peso >= limite_RCL_camion) {
            indice++;
        }
    }
    return (indice == 0) ? 1 : indice;
}

void grasp_pedidos(Pedido* pedido, int n, int capacidad) {
    srand(time(NULL));
    vector<Camion> v_camion_solucion;
    int total_camiones = INT_MAX;

    sort(pedido, pedido + n, compara);
    for (int i = 0; i < ITERACIONES; i++) {
        vector<Pedido> v_pedido;
        vector<Camion> v_camion;
        int indice_camion = 0;
        int cantidad_camiones = 0;
        v_pedido.insert(v_pedido.begin(), pedido, pedido + n);

        while (not v_pedido.empty()) {
            int beta = v_pedido.front().peso;
            int tau = v_pedido.back().peso;
            double limite_RCL = beta - ALPHA * (beta - tau);
            int indice_RCL = buscar_indice(v_pedido, limite_RCL);
            int indice_random = rand() % indice_RCL;
            Pedido pedido_actual = v_pedido[indice_random];

            bool insertado = false;
            vector<Pedido> v_pedido_actual;
            v_pedido_actual.push_back(pedido_actual);
            if (v_camion.empty()) {
                v_camion.push_back({indice_camion++, pedido_actual.peso, {v_pedido_actual}});
                v_pedido.erase(v_pedido.begin() + indice_random);
                cantidad_camiones++;
                continue;
            }

            vector<Camion> v_camion_valido;
            for (int j = 0; j < v_camion.size(); j++) {
                if (v_camion[j].peso + pedido_actual.peso <= capacidad)
                    v_camion_valido.push_back(v_camion[j]);
            }

            if (v_camion_valido.empty()) {
                v_camion.push_back({indice_camion++, pedido_actual.peso, {v_pedido_actual}});
                v_pedido.erase(v_pedido.begin() + indice_random);
                cantidad_camiones++;
            } else {
                sort(v_camion_valido.begin(), v_camion_valido.end(), compara_camion);
                int beta_camion = v_camion_valido.front().peso;
                int tau_camion = v_camion_valido.back().peso;
                double limite_RCL_camion = beta_camion - ALPHA * (beta_camion - tau_camion);
                int indice_RCL_camion = buscar_indice_camion(v_camion_valido, limite_RCL_camion);
                int indice_random_camion = rand() % indice_RCL_camion;

                for (int k = 0; k < v_camion.size(); k++) {
                    if (v_camion[k].id == v_camion_valido[indice_random_camion].id) {
                        v_camion[k].peso += pedido_actual.peso;
                        v_camion[k].pedido.push_back(pedido_actual);
                        v_pedido.erase(v_pedido.begin() + indice_random);
                    }
                }
            }
        }
        if (total_camiones > cantidad_camiones) {
            total_camiones = cantidad_camiones;
            v_camion_solucion.clear();
            v_camion_solucion.insert(v_camion_solucion.begin(), v_camion.begin(), v_camion.end());
        }
    }
    cout << "=============================================" << endl;
    cout << "   GRASP CONSTRUCCION - MEDIEXPRESS S.A.C " << endl;
    cout << "=============================================" << endl;
    cout << "Pedidos: " << n << endl;
    cout << "Alfa: " << ALPHA << "  | Iteraciones: " << ITERACIONES << endl;
    cout << "=============================================" << endl;
    for (int i = 0; i < v_camion_solucion.size(); i++) {
        cout << "Camion " << i + 1 << " (cap=" << capacidad << "kg): ";
        int peso = 0;
        for (int j = 0; j < v_camion_solucion[i].pedido.size(); j++) {
            cout << v_camion_solucion[i].pedido[j].id << ", ";
            peso += v_camion_solucion[i].pedido[j].peso;
        }
        cout << "   | " << peso << '/' << capacidad << endl;
    }
    cout << "Cantidad de camiones: " << total_camiones << endl;
}


int main() {
    Pedido pedido[] = {
        {'A', 15},
        {'B', 32},
        {'C', 20},
        {'D', 42},
        {'E',  8},
        {'F', 18},
    };
    int n = sizeof(pedido) / sizeof(pedido[0]);
    int capacidad = 50;

    grasp_pedidos(pedido, n, capacidad);
    return 0;
}