#include <algorithm>
#include <iostream>
#include <ctime>
#include <climits>
#include <vector>
using namespace std;
#define ITERACIONES 1000
#define ALPHA 0.15

struct Contenedor {
    char objeto;
    int ancho;
    int alto;
    double area;
};

bool compara(const Contenedor& a, const Contenedor& b) {
    return a.ancho > b.ancho;
}

int buscar_indice(const vector<Contenedor>& matriz, double limite) {
    int indice = 0;
    for (int i = 0; i < matriz.size(); i++) {
        if (matriz[i].ancho > limite) indice++;
    }
    return (indice == 0) ? 1 : indice;
}

void imprimir(const vector<Contenedor>& matriz) {
    for (int i = 0; i < matriz.size(); i++) {
        cout << "|  " << matriz[i].objeto << '(' << matriz[i].ancho << 'x' << matriz[i].alto << ")  ";
    }
}

void lineas(const vector<Contenedor>& matriz, int cantidad) {
    cout << "\n+";
    for (int j = 0; j < cantidad; j++) {
        for (int k = 0; k < 10; k++) cout.put('-');
        cout << "+";
    }
    cout << '\n';
}

void grasp_contenedor(Contenedor* contenedor, int n, int ancho, int alto) {
    srand(time(NULL));
    vector<vector<Contenedor>> v_mejor_solucion;
    int mejor_solucion = INT_MAX;

    sort(contenedor, contenedor + n, compara);
    for (int i = 0; i < ITERACIONES; i++) {
        vector<Contenedor> v_contenedor;
        vector<vector<Contenedor>> v_solucion;

        v_contenedor.insert(v_contenedor.begin(), contenedor, contenedor + n);
        while (not v_contenedor.empty()) {
            int beta = v_contenedor.front().ancho;
            int tau = v_contenedor.back().ancho;
            double limite_RCL = beta - ALPHA * (beta - tau);
            int indice_RCL = buscar_indice(v_contenedor, limite_RCL);
            int indice_random = rand() % indice_RCL;

            if (v_solucion.empty()) {
                vector<Contenedor> v_auxiliar;
                v_auxiliar.push_back(v_contenedor[indice_random]);
                v_solucion.push_back(v_auxiliar);
                v_contenedor.erase(v_contenedor.begin() + indice_random);
                continue;
            }

            bool insertado = false;
            for (int i = 0; i < v_solucion.size(); i++) {
                int suma_ancho = 0;
                for (int j = 0; j < v_solucion[i].size(); j++) {
                    suma_ancho += v_solucion[i][j].ancho;
                }
                if (ancho - suma_ancho >= v_contenedor[indice_random].ancho) {
                    v_solucion[i].push_back(v_contenedor[indice_random]);
                    v_contenedor.erase(v_contenedor.begin() + indice_random);
                    insertado = true;
                    break;
                }
            }
            if (insertado == false) {
                vector<Contenedor> v_auxiliar;
                v_auxiliar.push_back(v_contenedor[indice_random]);
                v_solucion.push_back(v_auxiliar);
                v_contenedor.erase(v_contenedor.begin() + indice_random);
            }
        }

        if (v_solucion.size() < mejor_solucion) {
            mejor_solucion = v_solucion.size();
            v_mejor_solucion.clear();
            v_mejor_solucion.insert(v_mejor_solucion.begin(), v_solucion.begin(), v_solucion.end());
        }
    }
    cout << "========= CONTENEDOR (" << ancho << "x" << alto << ") ==========";
    for (int i = 0; i < v_mejor_solucion.size(); i++) {
        lineas(v_mejor_solucion[i], v_mejor_solucion[i].size());
        imprimir(v_mejor_solucion[i]);
        cout << '|';
    }
    lineas(v_mejor_solucion.back(), v_mejor_solucion.back().size());
}

int main() {
    Contenedor contenedor[] = {
        {'A', 6, 5, 30.0},
        {'B', 4, 5, 20.0},
        {'C', 3, 3,  9.0},
        {'D', 5, 5, 25.0},
        {'E', 2, 1,  2.0},
        {'F', 7, 3, 21.0},
        {'G', 3, 7, 21.0},
        {'H', 2, 6, 12.0},
        {'I', 4, 4, 16.0},
        {'J', 5, 2, 10.0},
        {'K', 1, 7,  7.0},
        {'L', 7, 1,  7.0}
    };
    int n = sizeof(contenedor) / sizeof(contenedor[0]);
    int ancho = 10, alto = 10;

    grasp_contenedor(contenedor, n, ancho, alto);
    return 0;
}