#include "hopfield.h"
#include <algorithm>
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock
#include <iostream>

Hopfield::Hopfield(int pixelCount) : N(pixelCount)
{
    // ustvari matriko uteži
    this->W = new double*[N];
    for (int i = 0; i != N; i++)
        this->W[i] = new double[N];

    indexArr = new int[N];
    for (int i = 0; i != N; i++)
        indexArr[i] = i;
}

Hopfield::~Hopfield()
{
    delete[] W;
}

double Hopfield::Q(int u, int v, std::vector< std::vector<int> > &samples)
{
    double q = 0.0;
    for (int k = 0; k != N; k++)
    {
        q += samples[v][k] * samples[u][k];
    }
    q = q / N;

    return q;
}

void Hopfield::Learn(std::vector< std::vector<int> > samples, LearningMethod method)
{
    // HEBBIAN RULE
    if (method == LearningMethod::HEBBIAN)
    {
        arma::mat w(N, N);
        for (int i = 0; i != N; i++)
        {
            for (int j = 0; j != N; j++)
            {
                w(i, j) = 0; // zero-init
                if (i != j) // ker so diagonalci 0 ...
                {
                    for (int l = 0; l != samples.size(); l++)
                    {
                        w(i, j) += (1.0 / N) * (samples[l][i] * samples[l][j]);
                    }
                    w(j, i) = w(i, j);
                }
            }
        }

        // + pseudo inverse from hebbian ... forks NOT! :@
        //w = w.t() * (w * w.t()).i();

        // prekopiraj iz Armadillo mat objekta v 2D array
        for (int i = 0; i != N; i++)
        {
            for (int j = 0; j != N; j++)
            {
                W[i][j] = w(i, j);
            }
        }
    }




    // PSEUDO-INVERSE RULE : http://homepages.inf.ed.ac.uk/amos/publications/Storkey1997IncreasingtheCapacityoftheHopfieldNetworkwithoutSacrificingFunctionality.pdf
    if (method == LearningMethod::PSEUDO_INVERSE)
    {
        arma::mat w(N, N);
        int m = samples.size(); // št. učnih vzorcev
        for (int i = 0; i != N; i++)
        {
            for (int j = 0; j != N; j++)
            {
                w(i, j) = 0.0;
                for (int v = 0; v != m; v++)
                {
                    for (int u = 0; u != m; u++)
                    {
                        w(i, j) += samples[v][i] * (1.0 / Q(u, v, samples)) * samples[u][j];
                    }
                }
                w(i, j) = w(i, j) / N;
            }
        }
        std::cout << w << endl;

        // prekopiraj iz Armadillo mat objekta v 2D array
        for (int i = 0; i != N; i++)
        {
            for (int j = 0; j != N; j++)
            {
                W[i][j] = w(i, j);
            }
        }
    }


    // STORKEY : http://homepages.inf.ed.ac.uk/amos/publications/Storkey1997IncreasingtheCapacityoftheHopfieldNetworkwithoutSacrificingFunctionality.pdf
    if (method == LearningMethod::STORKEY)
    {
        for (int i = 0; i != N; i++)
        {
            for (int j = 0; j != N; j++)
            {
                W[i][j] = 0.0;
            }
        }

        int m = samples.size(); // št. učnih vzorcev
        for (int i = 0; i != N; i++)
        {
            for (int j = i + 1; j < N; j++)
            {
                for (int v = 0; v != m; v++)
                {
                    W[i][j] = W[i][j] + (1.0 / N)*
                    (samples[v][i] * samples[v][j]
                     - samples[v][i] * h(v, j, i, samples)
                     - h(v, i, j, samples) * samples[v][j]);
                }

                W[j][i] = W[i][j];
            }
        }
    }
}

double Hopfield::h(int v, int i, int j, std::vector< std::vector<int> > &samples)
{
    double sum = 0.0;
    for (int k = 0; k != N; k++)
    {
        if (k == i || k == j)
            continue;

        sum += W[i][k] * samples[v][k];
    }

    return sum;
}

double Hopfield::energy(int* state) // izračuna energijo stanja
{
    double E = 0;
    for (int i = 0; i != N; i++)
    {
        for (int j = 0; j != N; j++)
        {
            E += W[i][j] * state[i] * state[j];
        }
    }

    E *= -0.5;
    return E;
}

std::vector<int> Hopfield::Recognise(std::vector<int> x, int absoluteEpochLimit, int noChangeLimit)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> rnd(0, N - 1);

    // ustvari izhodni vzorec
    int* y = new int[N];
    for (int i = 0; i != N; i++)
        y[i] = x[i]; // inicializiraj na vhod

    int epochCount = 0; // števec iteracij
    int epochSinceChange = 0; // števec iteracij od zadnje spremembe
    do
    {
        int i = rnd(gen); // naključen nevron

        // izračunaj izhod nevrona
        double y_new = 0.0;
        for (int j = 0; j != N; j++)
        {
            y_new += y[j] * W[i][j];
        }

        // računanje novega izhoda stanja (-1 ali +1)
        int y_new_int = 0;
        if (y_new > 0.0)
            y_new_int = 1;
        else if (y_new < 0.0)
            y_new_int = -1;

        // smo spremenili mrežo ?
        if (y_new_int != y[i])
            epochSinceChange = 0;
        else
            epochSinceChange++;

        // posodobi vrednost
        y[i] = y_new_int;

        epochCount++;
        qDebug() << "EpochCount: " << epochCount << endl;
        qDebug() << "EpochSinceLastChange: " << epochSinceChange << endl;
    } while(epochCount < absoluteEpochLimit && epochSinceChange < noChangeLimit);

    return std::vector<int>(y, y + N);
}
