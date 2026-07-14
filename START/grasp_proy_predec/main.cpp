#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <climits>
using namespace std;
#define ITERACIONES 1000
#define ALPHA 0.3

struct Proyecto {
    int id;
    int costo;
    int ganancia;
    double beneficio;
    vector<int> predecesores;
};

void asignar_ganancia(Proyecto* proyecto, int n) {
    for (int i = 0; i < n; i++) {
        proyecto[i].beneficio = (double) proyecto[i].ganancia / proyecto[i].costo;
    }
}

int buscar_indice(const vector<Proyecto>& v_proyectos_validos, double limite_RCL) {
    int indice = 0;
    for (int i = 0; i < v_proyectos_validos.size(); i++) {
        if (v_proyectos_validos[i].beneficio >= limite_RCL) {
            indice++;
        }
    }
    return (indice == 0) ? 1 : indice;
}

bool validar_proyecto(const Proyecto& proyecto, const vector<Proyecto>& v_proyecto_solucion) {
    if (proyecto.predecesores.empty()) return true;

    int cantidad_predecesores = 0;
    for (int i = 0; i < proyecto.predecesores.size(); i++) {
        for (int j = 0; j < v_proyecto_solucion.size(); j++) {
            if (v_proyecto_solucion[j].id == proyecto.predecesores[i])
                cantidad_predecesores++;
        }
    }
    if (cantidad_predecesores == proyecto.predecesores.size()) return true;

    return false;
}

void grasp_proyecto(Proyecto* proyecto, int n, int presupuesto) {
    srand(time(NULL));
    vector<Proyecto> v_proyecto_mejor_solucion;
    double beneficio = INT_MIN;

    asignar_ganancia(proyecto, n);
    sort(proyecto, proyecto + n, [](const Proyecto& p1, const Proyecto& p2) {
        return p1.beneficio > p2.beneficio;
    });
    for (int i = 0; i < ITERACIONES; i++) {
        vector<Proyecto> v_proyectos(proyecto, proyecto + n);
        vector<Proyecto> v_proyecto_solucion;
        int residual = presupuesto;
        double beneficio_parcial = 0;

        while (not v_proyectos.empty()) {
            vector<Proyecto> v_proyectos_validos;
            for (int j = 0; j < v_proyectos.size(); j++) {
                if (v_proyectos[j].costo <= residual and validar_proyecto(v_proyectos[j], v_proyecto_solucion)) {
                    v_proyectos_validos.push_back(v_proyectos[j]);
                }
            }

            if (v_proyectos_validos.empty()) break;

            sort(v_proyectos_validos.begin(), v_proyectos_validos.end(), [](const Proyecto& p1, const Proyecto& p2) {
                return p1.beneficio > p2.beneficio;
            });
            double beta = v_proyectos_validos.front().beneficio;
            double tau = v_proyectos_validos.back().beneficio;
            double limite_RCL = beta - ALPHA * (beta - tau);
            int indice_RCL = buscar_indice(v_proyectos_validos, limite_RCL);
            int indice_random = rand() % indice_RCL;

            residual -= v_proyectos_validos[indice_random].costo;
            beneficio_parcial += v_proyectos_validos[indice_random].ganancia;
            v_proyecto_solucion.push_back(v_proyectos_validos[indice_random]);

            for (int j = 0; j < v_proyectos.size(); j++) {
                if (v_proyectos[j].id == v_proyectos_validos[indice_random].id) {
                    v_proyectos.erase(v_proyectos.begin() + j);
                }
            }
        }
        if (beneficio < beneficio_parcial) {
            beneficio = beneficio_parcial;
            v_proyecto_mejor_solucion.clear();
            v_proyecto_mejor_solucion.insert(v_proyecto_mejor_solucion.begin(),
                v_proyecto_solucion.begin(), v_proyecto_solucion.end());
        }
    }
    cout << "=== PROYECTOS SELECCIONADOS ===" << endl;
    for (int i = 0; i < v_proyecto_mejor_solucion.size(); i++) {
        cout << v_proyecto_mejor_solucion[i].id << "  ";
    }
    cout << endl;
    cout << "Beneficio total: " << beneficio << " (Millones de $)" << endl;
}

int main() {
    Proyecto proyecto[] = {
        {1, 100, 200, 0, {}},
        {2,  50, 300, 0, {1}},
        {3, 150, 300, 0, {1}},
        {4,  50, 400, 0, {}},
        {5,  50, 200, 0, {4}},
        {6, 150, 800, 0, {2, 4}},
        {7, 100, 250, 0, {}},
    };
    int n = sizeof(proyecto) / sizeof(proyecto[0]);
    int presupuesto = 400;

    grasp_proyecto(proyecto, n, presupuesto);
    return 0;
}














