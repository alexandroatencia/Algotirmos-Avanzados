#include <iostream>
#include <vector>
#include <map>
#include <ctime>
#include <cmath>
#include <algorithm>
#define ITERACIONES 1000
#define NUMIND 20
#define TSELECCION 0.3
#define PCASAMIENTO 0.5
#define TMUTACION 0.3

using namespace  std;

struct Job {
    int id;
    int tiempo;
};

int calculafitness(vector<int>cromo,Job* job) {
    int servidor_A=0;
    int servidor_B=0;
    for(int i=0;i<cromo.size();i++) {
        if (cromo[i])
            servidor_A+=job[i].tiempo;
        else
            servidor_B+=job[i].tiempo;
    }
    return max(servidor_A,servidor_B);
}

bool aberracion(vector<int>ind,Job* job) {
    return false;
}

void generapoblacion(vector<vector<int>> &poblacion, Job* job, int n) {
    int cont=0;
    srand(time(NULL));
    while (cont<NUMIND) {
        vector<int> individuo;
        // aqui también cambiar
        for (int i=0; i<n; i++)
            individuo.push_back(rand()%2);
        if (!aberracion(individuo,job)) {
            poblacion.push_back(individuo);
            cont++;
        }
    }
}

void muestrapoblacion(vector<vector<int>>poblacion,Job* job) {
    for(int i=0;i<poblacion.size();i++) {
        for(int j=0;j<poblacion[i].size();j++) {
            cout<<poblacion[i][j]<<" ";
        }
        cout<<" fo= "<< calculafitness(poblacion[i],job)<<endl;
    }
}

void calculasupervivencia(vector<vector<int>> poblacion, vector<int> &supervivencia, Job* job) {
    // Primero encontramos el peor Makespan de la población actual
    int peor = 0;
    for (int i = 0; i < poblacion.size(); i++)
        peor = max(peor, calculafitness(poblacion[i], job));

    // Invertimos: quien tiene Makespan más bajo, obtiene valor más alto
    vector<int> fitness_invertido;
    int sumaInvertida = 0;
    for (int i = 0; i < poblacion.size(); i++) {
        int inv = peor - calculafitness(poblacion[i], job) + 1; // +1 para que nunca sea 0
        fitness_invertido.push_back(inv);
        sumaInvertida += inv;
    }

    for (int i = 0; i < poblacion.size(); i++) {
        int superfit = round(100 * (double)fitness_invertido[i] / sumaInvertida);
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

void seleccion(vector<vector<int>>poblacion,vector<vector<int>> &padres,Job* job) {
    int ruleta[100]{-1};
    vector<int>supervivencia;
    calculasupervivencia(poblacion,supervivencia,job);
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

void casamiento(vector<vector<int>> &poblacion,vector<vector<int>>padres,Job* job) {
    for(int i=0;i<padres.size();i++)
        for(int j=0;j<padres.size();j++) {
            if (i!=j) {
                vector<int>hijo;
                creahijo(padres[i],padres[j],hijo);
                if (!aberracion(hijo,job))
                    poblacion.push_back(hijo);
            }
        }
}

int decimal(vector<int>ind) {
    int num=0;
    for (int i=0;i<ind.size();i++)
        num+=ind[i]*pow(2,i);
    return num;
}

void mataclon(vector<vector<int>> &poblacion) {
    map<int,vector<int>> indunicos;

    for(int i=0;i<poblacion.size();i++) {
        int dec=decimal(poblacion[i]);
        indunicos[dec]=poblacion[i];
    }
    poblacion.clear();
    for(map<int,vector<int>>::iterator it=indunicos.begin();
        it!=indunicos.end();it++) {
        poblacion.push_back(it->second);
        }
}

void mutacion(vector<vector<int>> &poblacion,vector<vector<int>>padres,Job* job) {
    int nmutaciones=round(padres[0].size()*TMUTACION);
    for (int i=0;i<padres.size();i++) {
        int cont=0;
        while (cont<nmutaciones) {
            int gen=rand()%padres[i].size();
            //aqui hay que cambiar si son numeros enteros
            if (padres[i][gen]==0) padres[i][gen]=1;
            else padres[i][gen]=0;
            cont++;
        }
        if (!aberracion(padres[i],job))
            poblacion.push_back(padres[i]);
    }
}

void inversion(vector<vector<int>> &poblacion,vector<vector<int>>padres,Job* job) {
    for (int i=0;i<padres.size();i++) {
        for (int j=0;j<padres[i].size();j++) {
            //cambiar para numeros enteros
            if (padres[i][j]==0) padres[i][j]=1;
            else padres[i][j]=0;
        }
        if (!aberracion(padres[i],job))
            poblacion.push_back(padres[i]);
    }
}

void regenerapoblacion(vector<vector<int>> &poblacion,Job* job){
    mataclon(poblacion);
    // se evita emplear variables globales
    sort(poblacion.begin(), poblacion.end(),
         [job](const vector<int>& a, const vector<int>& b) {
            return calculafitness(a, job) < calculafitness(b, job);
         });
    if (poblacion.size()>NUMIND)
        poblacion.erase(poblacion.begin()+NUMIND,poblacion.end());
}

void muestramejor(vector<vector<int>> poblacion,Job* job){
    int mejor=0;
    for(int i=0;i<poblacion.size();i++)
        if(calculafitness(poblacion[mejor],job) > calculafitness(poblacion[i],job))
            mejor=i;

    cout << "Servidor A: ";
    for(int i=0;i<poblacion[mejor].size();i++)
        if (poblacion[mejor][i])
            cout << job[i].id << "  ";
    cout << endl << "Servidor B: ";
    for(int i=0;i<poblacion[mejor].size();i++)
        if (!poblacion[mejor][i])
            cout << job[i].id << "  ";
    cout << endl<<"La mejor solucion es: " << calculafitness(poblacion[mejor],job)<<endl;
}

void genetico_job(Job *job,int n) {
    vector<vector<int>> poblacion;
    generapoblacion(poblacion,job,n);
    // muestrapoblacion(poblacion,job);

    for(int i=0;i<ITERACIONES;i++) {
        vector<vector<int>>padres;

        seleccion(poblacion,padres,job);
        casamiento(poblacion,padres,job);
        mataclon(poblacion);
        mutacion((poblacion),padres,job);
        inversion(poblacion,padres,job);
        regenerapoblacion(poblacion,job);
    }
    muestramejor(poblacion,job);
}

int main() {
    Job job[] = {
        {1, 45},
        {2, 80},
        {3, 30},
        {4, 55},
        {5, 70},
        {6, 20},
        {7, 90},
        {8, 35},
        {9, 60},
        {10, 25},
    };
    int n=sizeof(job)/sizeof(Job);

    genetico_job(job, n);
    return 0;
}