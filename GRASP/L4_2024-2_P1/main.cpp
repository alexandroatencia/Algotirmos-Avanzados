#include <iostream>
#include <climits>
#include <algorithm>
#include <vector>
#include <ctime>
using namespace std;
#define ITERACIONES 1000
#define ALPHA 0.3

bool compara(const int& a, const  int& b) {
    return a > b;
}

int buscar_indice(const vector<int>& v_objetos, double indice_RCL) {
    int indice = 0;
    for (int i = 0; i < v_objetos.size(); i++) {
        if (v_objetos[i] >= indice_RCL) {
            indice++;
        }
    }
    return (indice == 0) ? 1 : indice;
}

void grasp_objetos(int*  objetos, int n, int capacidad) {
    srand(time(NULL));
    vector<vector<int>> v_mejor_solucion;
    int minimo_objetos = INT_MAX;

    sort(objetos, objetos + n, compara);
    for (int i = 0; i < ITERACIONES; i++) {
        vector<vector<int>> v_solucion;
        vector<int> v_objetos;
        v_objetos.insert(v_objetos.begin(), objetos, objetos + n);

        while (not v_objetos.empty()) {
            int beta = v_objetos.front();
            int tau = v_objetos.back();
            double limite_RCL = beta - ALPHA * (beta - tau);
            int indice_RCL = buscar_indice(v_objetos, limite_RCL);
            int indice_random = rand() % indice_RCL;

            if (v_solucion.empty()) {
                vector<int> v_aux;
                v_aux.push_back(v_objetos[indice_random]);
                v_solucion.push_back(v_aux);
                v_objetos.erase(v_objetos.begin() + indice_random);
                continue;
            }

            bool insertado = false;
            for (int i = 0; i < v_solucion.size(); i++) {
                int peso_actual = 0;
                for (int j = 0; j < v_solucion[i].size(); j++) {
                    peso_actual += v_solucion[i][j];
                }

                if (peso_actual + v_objetos[indice_random] <= capacidad) {
                    v_solucion[i].push_back(v_objetos[indice_random]);
                    v_objetos.erase(v_objetos.begin() + indice_random);
                    insertado = true;
                    break;
                }
            }
            if (insertado == false) {
                vector<int> v_aux;
                v_aux.push_back(v_objetos[indice_random]);
                v_solucion.push_back(v_aux);
                v_objetos.erase(v_objetos.begin() + indice_random);
            }
        }
        if (minimo_objetos > v_solucion.size()) {
            minimo_objetos = v_solucion.size();
            v_mejor_solucion.clear();
            v_mejor_solucion.insert(v_mejor_solucion.begin(), v_solucion.begin(), v_solucion.end());
        }
    }
    cout << "========= SOLUCION ========= " << endl;
    for (int i = 0; i < v_mejor_solucion.size(); i++) {
        cout << "* Contenedor " << i + 1 << ": [";
        for (int j = 0; j < v_mejor_solucion[i].size(); j++) {
            cout << v_mejor_solucion[i][j];
            if (j < v_mejor_solucion[i].size() - 1) {
                cout << ", ";
            }
        }
        cout << "]" << endl;
    }
}

int main() {
    int objetos[] = {4, 8, 1, 4, 2, 1};
    int n = sizeof(objetos) / sizeof(objetos[0]);
    int capacidad = 10;

    grasp_objetos(objetos, n, capacidad);
    return 0;
}