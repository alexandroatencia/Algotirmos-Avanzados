#include <iostream>
#include <climits>
#include <ctime>
#include <algorithm>
#include <vector>
using namespace std;
#define ITERACIONES 1000
#define ALPHA 0.3

struct Proyecto {
    int id;
    int costo;
    int ganancia;
    double ratio;
    vector<int> predecesores;
};

bool compara(const Proyecto &p1, const Proyecto &p2) {
    return p1.ratio > p2.ratio;
}

int buscar_indice(const vector<Proyecto>& v_proyecto, double limite_RCL) {
    int indice = 0;
    for (int i = 0; i < v_proyecto.size(); i++) {
        if (v_proyecto[i].ratio >= limite_RCL) {
            indice++;
        }
    }
    return (indice == 0) ? 1 : indice;
}

bool validar_predecesor(const Proyecto& proyecto, const vector<int>& v_proyecto_solucion) {
    int cantidad_predecesores = 0;
    if (proyecto.predecesores.size() == 0) return true;

    for (int i = 0; i < proyecto.predecesores.size(); i++) {
        for (int j = 0; j < v_proyecto_solucion.size(); j++) {
            if (proyecto.predecesores[i] == v_proyecto_solucion[j]) {
                cantidad_predecesores++;
            }
        }
    }

    if (cantidad_predecesores == proyecto.predecesores.size()) return true;
    return false;
}

void grasp_moviliaria(Proyecto* proyecto, int n, int presupuesto) {
    srand(time(NULL));
    vector<int> v_proyecto_mejor_solucion;
    int maximo_beneficio = INT_MIN;

    sort(proyecto, proyecto + n, compara);
    for (int i = 0; i < ITERACIONES; i++) {
        vector<Proyecto> v_proyecto(proyecto, proyecto + n);
        vector<int> v_proyecto_solucion;
        int beneficio_parcial = 0;
        int presupuesto_residual = presupuesto;

        while (true) {
            vector<Proyecto> v_proyectos_validos;
            for (int j = 0; j < v_proyecto.size(); j++) {
                if (v_proyecto[j].costo <= presupuesto_residual and
                    validar_predecesor(v_proyecto[j], v_proyecto_solucion)) {
                    v_proyectos_validos.push_back(v_proyecto[j]);
                }
            }

            if (v_proyectos_validos.empty()) break;

            // Solo a los proyectos validos
            double beta = v_proyectos_validos.front().ratio;
            double tau = v_proyectos_validos.back().ratio;
            double limite_RCL = beta - ALPHA * (beta - tau);
            int indice_RCL = buscar_indice(v_proyectos_validos, limite_RCL);
            int indice_random = rand() % indice_RCL;
            Proyecto proyecto_actual = v_proyectos_validos[indice_random];

            presupuesto_residual -= proyecto_actual.costo;
            beneficio_parcial += proyecto_actual.ganancia;
            v_proyecto_solucion.push_back(proyecto_actual.id);

            for (int k = 0; k < v_proyecto.size(); k++) {
                if (v_proyecto[k].id == proyecto_actual.id) {
                    v_proyecto.erase(v_proyecto.begin() + k);
                }
            }
        }
        if (maximo_beneficio < beneficio_parcial) {
            maximo_beneficio = beneficio_parcial;
            v_proyecto_mejor_solucion.clear();
            v_proyecto_mejor_solucion.insert(v_proyecto_mejor_solucion.begin(),
                v_proyecto_solucion.begin(), v_proyecto_solucion.end());
        }
    }
    cout << "======= RESULTADO =======" << endl;
    cout << "Proyectos seleccionados: ";
    for (int i = 0; i < v_proyecto_mejor_solucion.size(); i++) {
        cout << v_proyecto_mejor_solucion[i] << " ";
    }
    cout << "\nBeneficio total: " << maximo_beneficio << " (Millones de $)\n";
}

int main() {
    Proyecto proyecto[] = {
        {1, 100, 200, 200 / 100.0, {}},
        {2,  50, 300, 300 /  50.0, {1}},
        {3, 150, 300, 300 / 150.0, {1}},
        {4,  50, 400, 400 /  50.0, {}},
        {5,  50, 200, 200 /  50.0, {4}},
        {6, 150, 800, 800 / 150.0, {2, 4}},
        {7, 100, 250, 250 / 100.0, {}},
    };
    int n = sizeof(proyecto) / sizeof(proyecto[0]);
    int presupuesto = 400;

    grasp_moviliaria(proyecto, n, presupuesto);
    return 0;
}