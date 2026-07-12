#include <iostream>
#include <climits>
#include <ctime>
#include <vector>
#include <algorithm>
using namespace std;
#define ITERACIONES 10000
#define ALPHA 0.4

struct Tarea {
    int id;
    int duracion;
    vector<int> ventana;
    int penalizacion;
};

bool compara(const Tarea& a, const Tarea& b) {
    return a.ventana[1] < b.ventana[1] or
        a.ventana[1] == b.ventana[1] and a.ventana[0] < b.ventana[0];
}

void mostrar_tareas(const vector<Tarea>& v_tareas) {
    for (int i = 0; i < v_tareas.size(); i++) {
        cout << "T" << v_tareas[i].id << ": [";
        for (int j = 0; j < v_tareas[i].ventana.size(); j++) {
            cout << v_tareas[i].ventana[j];
            if (j < v_tareas[i].ventana.size() - 1) {
                cout << ", ";
            }
        }
        cout << "]" << endl;
    }
}

int buscar_indice(const vector<Tarea>& v_tareas, double limite_RCL) {
    int indice = 0;
    for (int i = 0; i < v_tareas.size(); i++) {
        if (v_tareas[i].ventana[1] <= limite_RCL) {
            indice++;
        }
    }
    return (indice == 0) ? 1 : indice;
}

void grasp_tareas_programadas(Tarea* tarea, int n) {
    srand(time(NULL));
    vector<int> v_solucion_tareas_programadas;
    vector<int> v_solucion_tareas_no_programadas;
    int penalizacion = INT_MAX;

    sort(tarea, tarea + n, compara);
    for (int i = 0; i < ITERACIONES; i++) {
        vector<int> tareas_programadas;
        vector<int> tareas_no_programadas;
        int penalizacion_parcial = 0;
        vector<Tarea> v_tareas(tarea, tarea + n);
        // mostrar_tareas(v_tareas);

        int estado_actual = 0;
        while (not v_tareas.empty()) {
            int beta = v_tareas.front().ventana[1];
            int tau = v_tareas.back().ventana[1];
            double limite_RCL = beta + ALPHA * (tau - beta);
            int indice_RCL = buscar_indice(v_tareas, limite_RCL);
            int indice_random = rand() % indice_RCL;
            Tarea tarea_actual = v_tareas[indice_random];

            int inicio_real = max(estado_actual, tarea_actual.ventana[0] - 1);
            if (tarea_actual.duracion + inicio_real <= tarea_actual.ventana[1]) {
                estado_actual = inicio_real + tarea_actual.duracion;
                tareas_programadas.push_back(tarea_actual.id);
            } else {
                tareas_no_programadas.push_back(tarea_actual.id);
                penalizacion_parcial += tarea_actual.penalizacion;
            }
            v_tareas.erase(v_tareas.begin() + indice_random);
        }

        if (penalizacion > penalizacion_parcial) {
            penalizacion = penalizacion_parcial;
            v_solucion_tareas_programadas.clear();
            v_solucion_tareas_no_programadas.clear();
            v_solucion_tareas_programadas.insert(v_solucion_tareas_programadas.begin(),
                tareas_programadas.begin(), tareas_programadas.end());
            v_solucion_tareas_no_programadas.insert(v_solucion_tareas_no_programadas.begin(),
                tareas_no_programadas.begin(), tareas_no_programadas.end());
        }
    }
    cout << "======== RESULTADO =======" << endl;
    cout << "Tareas programadas: ";
    for (int i = 0; i < v_solucion_tareas_programadas.size(); i++) {
        cout << 'T' << v_solucion_tareas_programadas[i];
        if (i < v_solucion_tareas_programadas.size() - 1) {
            cout << ", ";
        }
    }
    cout << "\nTareas no completadas: ";
    for (int i = 0; i < v_solucion_tareas_no_programadas.size(); i++) {
        cout << 'T' << v_solucion_tareas_no_programadas[i];
        if (i < v_solucion_tareas_no_programadas.size() - 1) {
            cout << ", ";
        }
    }
    cout << "\nPenalizacion total: " << penalizacion << endl;
}

int main() {
    Tarea tarea[] = {
        { 1, 2, {1,  4}, 10},
        { 2, 3, {3,  6}, 15},
        { 3, 1, {5,  7},  8},
        { 4, 2, {2,  5}, 12},
        { 5, 4, {6, 10}, 20},
        { 6, 2, {8, 11},  7},
        { 7, 1, {9, 10},  6},
        { 8, 3, {1,  3}, 18},
        { 9, 2, {4,  7}, 14},
        {10, 3, {6,  9}, 16},
    };
    int n = sizeof(tarea) / sizeof(tarea[0]);

    grasp_tareas_programadas(tarea, n);
    return 0;
}