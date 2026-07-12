#include <iostream>
#include <climits>
#include <ctime>
#include <algorithm>
#include <vector>
#include <random>
using namespace std;
#define ITERACIONES 100
#define ALPHA 0.3

bool compara(const int&a, const int& b) {
    return a < b;
}

int buscar_indice(const vector<int>& v_paquetes, int limite_RCL) {
    int indice = 0;
    for (int i = 0; i < v_paquetes.size(); i++) {
        if (v_paquetes[i] <= limite_RCL) {
            indice++;
        }
    }
    return indice;
}

void imprimir_mochila(const vector<int>& v_mejor_solucion, int mejor_solucion) {
    for (int i = 0; i < v_mejor_solucion.size(); i++) {
        cout << v_mejor_solucion[i] << " ";
    }
    cout << "\nMinimo espacio no utilizado: " << mejor_solucion << '\n';
}

void grasp_mochila(int* paquetes, int n, int peso) {
    srand(time(NULL));
    vector<int> v_mejor_solucion;
    int mejor_solucion = INT_MAX;

    sort(paquetes, paquetes + n, compara);

    for (int i = 0; i < ITERACIONES; i++) {
        vector<int> v_paquetes;
        vector<int> v_solucion;
        int residual = peso;

        v_paquetes.insert(v_paquetes.begin(), paquetes, paquetes + n);
        cout << "======== VECTOR ORDENADO ========\n";
        imprimir_mochila(v_paquetes, peso);
        while (not v_paquetes.empty()) {
            cout << "----- VECTOR SOBRANTE -----\n";
            imprimir_mochila(v_paquetes, residual);
            int beta = v_paquetes.front();
            int tau = v_paquetes.back();
            double limite_RCL = beta + ALPHA * (tau - beta);
            int indice_RCL = buscar_indice(v_paquetes, limite_RCL);
            int indice_random = rand() % indice_RCL;

            if (residual >= v_paquetes[indice_random]) {
                residual -= v_paquetes[indice_random];
                v_solucion.push_back(v_paquetes[indice_random]);
            }

            cout << "\nEliminando paquete con peso: " << v_paquetes[indice_random] << '\n';
            v_paquetes.erase(v_paquetes.begin() + indice_random);
        }

        if (mejor_solucion > residual) {
            mejor_solucion = residual;
            v_mejor_solucion.clear();
            v_mejor_solucion.insert(v_mejor_solucion.begin(), v_solucion.begin(), v_solucion.end());
        }
    }
    cout << "\n======== VECTOR SOLUCION ========\n";
    imprimir_mochila(v_mejor_solucion, mejor_solucion);
}

int main() {
    int paquetes[] = {14, 12, 15, 14, 20, 11, 5, 17, 14, 28, 15, 27, 12, 10, 30, 16, 10};
    int n = sizeof(paquetes) / sizeof(paquetes[0]);
    int peso = 19;

    grasp_mochila(paquetes, n, peso);
    return 0;
}