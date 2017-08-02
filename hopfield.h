#ifndef HOPFIELD_H
#define HOPFIELD_H
#include <vector>
#include <QDebug>
#include <armadillo>

enum LearningMethod
{
    HEBBIAN,
    PSEUDO_INVERSE,
    STORKEY
};

class Hopfield
{
private:
    double** W = nullptr; // uteži
    int N; // dimenzija Hopfieldove mreže (dim(W) == N x N)
    int* indexArr = nullptr;

    double Hopfield::energy(int* state);
    double Q(int u, int v, std::vector< std::vector<int> > &samples);
    double h(int v, int i, int j, std::vector< std::vector<int> > &samples);

public:
    Hopfield(int);
    ~Hopfield();
    void Learn(std::vector< std::vector<int> >, LearningMethod method);
    std::vector<int> Recognise(std::vector<int> x, int absoluteEpochLimit, int noChangeLimit);
};

#endif // HOPFIELD_H
