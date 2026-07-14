#include <iostream>
#include <vector>
#include <map>
#include <ctime>
#include <cmath>
#include <algorithm>
using namespace  std;
#define ITERACIONES 1000
#define NUMIND 20
#define TSELECCION 0.3
#define PCASAMIENTO 0.5
#define TMUTACION 0.3

struct Proyecto {
    int id;
    int costo;
    int beneficio;
    vector<int> precesesores;
};

bool aberracion(vector<int>ind, Proyecto* proyecto, int presupuesto) {
    int costo = 0;
    for(int i=0;i<ind.size();i++) {
        if (ind[i] == 1)
            costo += proyecto[i].costo;
    }
    if (costo>presupuesto) return true; // validación del costo

    for (int i=0;i<ind.size();i++) {
        for (int j=0;j<proyecto[i].precesesores.size();j++) {
            int indice = proyecto[i].precesesores[j] - 1;
            if (ind[indice]==0) return true; // validación si tiene o no predecesor
        }
    }
    return false;
}

void generapoblacion(vector<vector<int>> &poblacion, Proyecto* proyecto,int n, int presupuesto) {
    int cont=0;
    srand(time(NULL));
    while (cont<NUMIND) {
        vector<int> individuo;
        // aqui también cambiar
        for (int i=0; i<n; i++)
            individuo.push_back(rand()%2);
        if (!aberracion(individuo,proyecto,presupuesto)) {
            poblacion.push_back(individuo);
            cont++;
        }
    }
}

int calculafitness(vector<int>cromo,Proyecto* proyecto, int presupuesto) {
    int sumap=0;
    int costo=0;
    for(int i=0;i<cromo.size();i++) {
        sumap+=cromo[i]*proyecto[i].beneficio;
        costo+=cromo[i]*proyecto[i].costo;
    }
    if (costo>presupuesto) return 0;
    return sumap;
}

void muestrapoblacion(vector<vector<int>>poblacion,Proyecto* proyecto, int presupuesto) {
    for(int i=0;i<poblacion.size();i++) {
        for(int j=0;j<poblacion[i].size();j++) {
            cout<<poblacion[i][j]<<" ";
        }
        cout<<" fo="<< calculafitness(poblacion[i],proyecto,presupuesto)<<endl;
    }
}

void calculasupervivencia(vector<vector<int>>poblacion,
    vector<int> &supervivencia,Proyecto* proyecto, int presupuesto) {
    int sumafitness=0;

    for(int i=0;i<poblacion.size();i++)
        sumafitness+=calculafitness(poblacion[i],proyecto,presupuesto);
    for (int i=0;i<poblacion.size();i++) {
        int superfit;
        superfit=round(100*(double)calculafitness(poblacion[i],proyecto,presupuesto)/sumafitness);
        supervivencia.push_back(superfit);
    }
}

void cargaruleta(vector<int >supervivencia,int *ruleta) {
    int cont=0;
    for (int i=0;i<supervivencia.size();i++)
        for (int j=0;j<supervivencia[i];j++) {
            ruleta[cont++]=i;
        }
    while (cont<100) ruleta[cont++]=-1;
}

void seleccion(vector<vector<int>>poblacion,vector<vector<int>> &padres,Proyecto* proyecto, int presupuesto) {
    int ruleta[100]{-1};
    vector<int>supervivencia;
    calculasupervivencia(poblacion,supervivencia,proyecto,presupuesto);
    cargaruleta(supervivencia,ruleta);
    int npadres=round(poblacion.size()*TSELECCION);

    for (int i=0;i<npadres;i++) {
        int ticket=rand()%100;
        if (ruleta[ticket]!=-1)
            padres.push_back(poblacion[ruleta[ticket]]);
    }
}

void creahijo(vector<int>padre,vector<int>madre,
    vector<int>&hijo) {
    int posi=round(padre.size()*PCASAMIENTO);

    for (int i=0;i<posi;i++)
        hijo.push_back(padre[i]);
    for (int i=posi;i<madre.size();i++)
        hijo.push_back(madre[i]);
}

void casamiento(vector<vector<int>> &poblacion,vector<vector<int>>padres,
    Proyecto* proyecto, int presupuesto) {
    for(int i=0;i<padres.size();i++)
        for(int j=0;j<padres.size();j++) {
            if (i!=j) {
                vector<int>hijo;
                creahijo(padres[i],padres[j],hijo);
                if (!aberracion(hijo,proyecto,presupuesto))
                    poblacion.push_back(hijo);
            }
        }
}

long long decimal(const vector<int>& individuo) {
    long long numero = 0;
    for (int i = 0; i < individuo.size(); i++) {
        numero += individuo[i] * pow(2, i);
    }
    return numero;
}

void mataclon(vector<vector<int>> &poblacion) {
    map<long long,vector<int>> indunicos;

    for(int i=0;i<poblacion.size();i++) {
        long long dec=decimal(poblacion[i]);
        indunicos[dec]=poblacion[i];
    }
    poblacion.clear();
    for(map<long long,vector<int>>::iterator it=indunicos.begin();
        it!=indunicos.end();it++) {
        poblacion.push_back(it->second);
        }
}

void mutacion(vector<vector<int>> &poblacion,vector<vector<int>>padres,
    Proyecto* proyecto, int presupuesto) {
    int nmutaciones=round(padres[0].size()*TMUTACION);
    for (int i=0;i<padres.size();i++) {
        int cont=0;
        while (cont<nmutaciones) {
            int gen=rand()%padres[i].size();
            if (padres[i][gen]==0) padres[i][gen]=1;
            else padres[i][gen]=0;
            cont++;
        }
        if (!aberracion(padres[i],proyecto,presupuesto))
            poblacion.push_back(padres[i]);
    }
}

void inversion(vector<vector<int>> &poblacion,vector<vector<int>>padres,
    Proyecto* proyecto, int presupuesto) {

    for (int i=0;i<padres.size();i++) {
        for (int j=0;j<padres[i].size();j++) {
            //cambiar para numeros enteros
            if (padres[i][j]==0) padres[i][j]=1;
            else padres[i][j]=0;
        }
        if (!aberracion(padres[i],proyecto,presupuesto))
            poblacion.push_back(padres[i]);
    }
}

void regenerapoblacion(vector<vector<int>> &poblacion,
        Proyecto* proyecto, int presupuesto){
    mataclon(poblacion);
    // se evita emplear variables globales
    sort(poblacion.begin(), poblacion.end(),
         [proyecto, presupuesto](const vector<int>& a, const vector<int>& b) {
            return calculafitness(a, proyecto, presupuesto) > calculafitness(b, proyecto, presupuesto);
         });
    if (poblacion.size()>NUMIND)
        poblacion.erase(poblacion.begin()+NUMIND,poblacion.end());

}

void muestramejor(vector<vector<int>> poblacion,
        Proyecto* proyecto, int presupuesto){
    int mejor=0;
    for(int i=0;i<poblacion.size();i++)
        if(calculafitness(poblacion[mejor],proyecto,presupuesto) <
            calculafitness(poblacion[i],proyecto,presupuesto))
            mejor=i;
    cout << "=== SOLUCION ===" << endl;
    for(int i=0;i<poblacion[mejor].size();i++)
        if (poblacion[mejor][i]==1)
            cout << poblacion[mejor][i] * proyecto[i].id << "  ";
    cout << endl<<"La mejor solucion es: " << calculafitness(poblacion[mejor],proyecto,presupuesto);
    cout << " (Millones de $)" << endl;
}

void genetico_proyecto(Proyecto* proyecto, int n, int presupuesto) {
    vector<vector<int>> poblacion;
    generapoblacion(poblacion,proyecto,n,presupuesto);
    // muestrapoblacion(poblacion,proyecto,presupuesto);

    for(int i=0;i<ITERACIONES;i++) {
        vector<vector<int>>padres;
        seleccion(poblacion,padres,proyecto,presupuesto);
        casamiento(poblacion,padres,proyecto,presupuesto);
        mataclon(poblacion);
        mutacion((poblacion),padres,proyecto,presupuesto);
        inversion(poblacion,padres,proyecto,presupuesto);
        regenerapoblacion(poblacion,proyecto,presupuesto);
    }
    muestramejor(poblacion,proyecto,presupuesto);
}


int main() {
    Proyecto proyecto[] = {
        {1, 100, 200, {}},
        {2,  50, 300, {1}},
        {3, 150, 300, {1}},
        {4,  50, 400, {}},
        {5,  50, 200, {4}},
        {6, 150, 800, {2, 4}},
        {7, 100, 250, {}},
    };
    int n = sizeof(proyecto)/sizeof(Proyecto);
    int presupuesto = 400;

    genetico_proyecto(proyecto, n, presupuesto);
    return 0;
}