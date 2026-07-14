#include <iostream>
#include <ctime>
#include <algorithm>
#include <climits>
#include <vector>
using namespace std;
#define ITERACIONES 1000
#define ALPHA 0.2

struct Pedido {
    char tipo;
    int peso;
};

struct Camion {
    int capacidad;
    vector<Pedido> pedidos;
};

int buscar_indice(const vector<Pedido>& v_pedidos, double limite_RCL) {
    int indice = 0;
    for (int i = 0; i < v_pedidos.size(); i++) {
        if (v_pedidos[i].peso >= limite_RCL) {
            indice++;
        }
    }
    return (indice == 0) ? 1 : indice;
}

void grasp_camiones(Pedido* pedido, int n, int capacidad_camiones) {
    srand(time(NULL));
    vector<Camion> v_camion_solucion;
    int cantidad_camiones = INT_MAX;

    sort(pedido, pedido + n, [](const Pedido& pedido1, const Pedido& pedido2) {
        return pedido1.peso > pedido2.peso;
    });
    for (int i = 0; i < ITERACIONES; i++) {
        vector<Pedido> v_pedidos(pedido, pedido + n);
        vector<Camion> v_camiones;
        int camiones = 0;

        while (not v_pedidos.empty()) {
            double beta = v_pedidos.front().peso;
            double tau = v_pedidos.back().peso;
            double limite_RCL = beta - ALPHA * (beta - tau);
            int indice_RCL = buscar_indice(v_pedidos, limite_RCL);
            int indice_random = rand() % indice_RCL;

            int capacidad = v_pedidos[indice_random].peso;
            vector<Pedido> v_pedido_actual;
            v_pedido_actual.push_back(v_pedidos[indice_random]);

            if (v_camiones.empty()) {
                v_camiones.push_back({capacidad, v_pedido_actual});
                camiones++;
                v_pedidos.erase(v_pedidos.begin() + indice_random);
                continue;
            }

            bool insertado = false;
            for (int j = 0; j < v_camiones.size(); j++) {
                if (v_camiones[j].capacidad + v_pedidos[indice_random].peso <= capacidad_camiones) {
                    v_camiones[j].capacidad += v_pedidos[indice_random].peso;
                    v_camiones[j].pedidos.push_back(v_pedidos[indice_random]);
                    v_pedidos.erase(v_pedidos.begin() + indice_random);
                    insertado = true;
                    break;
                }
            }

            if (insertado == false) {
                v_camiones.push_back({capacidad, v_pedido_actual});
                camiones++;
                v_pedidos.erase(v_pedidos.begin() + indice_random);
            }
        }
        if (cantidad_camiones > camiones) {
            cantidad_camiones = camiones;
            v_camion_solucion.clear();
            v_camion_solucion.insert(v_camion_solucion.begin(),
                v_camiones.begin(), v_camiones.end());
        }
    }
    cout << "==========================================" << endl;
    cout << "  GRASP CONSTRUCCION - MEDIEXPRESS S.A.C" << endl;
    cout << "==========================================" << endl;
    cout << "Pedidos: " << n << endl;
    cout << "Alfa: " << ALPHA << "     |     Iteraciones: " << ITERACIONES << endl;
    cout << "==========================================" << endl;
    for (int i = 0; i < v_camion_solucion.size(); i++) {
        cout << "Camion " << i + 1 << " (cap=" << capacidad_camiones << "): ";
        int peso = 0;
        for (int j = 0; j < v_camion_solucion[i].pedidos.size(); j++) {
            cout << v_camion_solucion[i].pedidos[j].tipo;
            if (j < v_camion_solucion[i].pedidos.size() - 1) {
                cout << ", ";
            }
            peso += v_camion_solucion[i].pedidos[j].peso;
        }
        cout << "  |  Peso: " << peso << '/' << capacidad_camiones << "kg" << endl;
    }
    cout << "--------------------------------------------" << endl;
    cout << "Cantidad total de camiones =  " << cantidad_camiones << endl;
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
    int capacidad_camiones = 50;

    grasp_camiones(pedido, n, capacidad_camiones);
    return 0;
}