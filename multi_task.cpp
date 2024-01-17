#include<iostream>
#include<cstdlib>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <windows.h>
#include <chrono>

using namespace std;
using namespace chrono;

class TheoryChecker {
public:
    TheoryChecker(int init, int finish, int num_threads)
        : search{ init, finish }, num_threads(num_threads), s_th{ new Search[num_threads] } {}

    ~TheoryChecker() {
        delete[] s_th;
        delete[] buff_prime;
    }

    void runSimulation() {
        prepareData();
        runTheoryCheck();
    }

private:
    mutex m_f1, m_f2, m_f3;
    mutex m_console;

    vector<int> vec_prime;
    int* buff_prime;
    int max_prime = 0;

    // Declare range
    struct Search {
        int init;
        int finish;
    };

    Search search;
    const int num_threads;
    Search* s_th;

    static bool firstNum(int n) {
        // Check if number is a prime number
        if (n < 2)
            return false;

        for (int i = 2; i * i <= n; i++)
            if (n % i == 0)
                return false;
        return true;
    }

    static bool evenNum(int i) {
        // Check if number is an even number
        return i % 2 == 0;
    }

    void simulationTask(Search& s) {
        vector<int> local_prime;

        // Fill local_prime with the initial 10 prime numbers
        for (int i = max(0, max_prime - 10); i < max_prime; ++i) {
            local_prime.push_back(buff_prime[i]);
        }

        for (int check_number = max(4, s.init); check_number <= s.finish; ++check_number) {
            if (evenNum(check_number)) {
                bool theory_check = false;

                // Iterate over half of local_prime
                int half_it = (int)(max_prime / 2);

                for (int it = 0; it < half_it; ++it) {
                    for (int itt = 0; itt < max_prime; ++itt) {
                        if (buff_prime[it] > buff_prime[itt]) {
                            // Skip the lower half of the matrix (including the diagonal)
                            continue;
                        }

                        if (buff_prime[it] + buff_prime[itt] == check_number) {
                            theory_check = true;
                            break;
                        }
                    }
                    if (theory_check) {
                        break;
                    }
                }

                if (!theory_check) {
                    // theory failed
                    lock_guard<mutex> lock(m_console);
                    cout << "Theory fail: " << check_number << endl;
                    break;
                }
            }
        }
    }

    void dataGenerationTask(Search& s) {
        // Task to generate data for simulation
        for (int i = s.init; i <= s.finish; i++) {
            if (firstNum(i)) {
                // Write to vector prime number
                lock_guard<mutex> lock(m_f2);
                vec_prime.push_back(i);
            }
        }
    }

    void prepareData() {
        // Vector with threads
        vector<thread> thr;
        // Init ranges for threads
        int range = (search.finish - search.init) / num_threads;
        s_th[0].init = 1;
        s_th[0].finish = s_th[0].init + range;
        if (num_threads != 1) {
            for (int i = 1; i < num_threads; i++) {
                s_th[i].init = s_th[i - 1].finish + 1;
                s_th[i].finish = s_th[i].init + range;
            }
        }

        // Prepare data with threads
        for (int i = 0; i < num_threads; i++)
            thr.push_back(thread(&TheoryChecker::dataGenerationTask, this, ref(s_th[i])));
        for (auto& t : thr)
            t.join();

        // FIXED ISSUE WITH BAD TIMMINGS IN MULTITHREAD
        // DO NOT USE VECTORS WHILE SHARE DATA WITH MULTITHREADINGS APPLICATION
        // Rewrite vector into array
        buff_prime = new int[vec_prime.size()];
        int i = 0;
        for (auto it = vec_prime.begin(); it != vec_prime.end(); ++it) {
            buff_prime[i++] = *it;
            max_prime++;
        }

        if (num_threads == 1) {
            // cout vectors
            cout << "numTh: " << num_threads << " prime vec: " << vec_prime.size() << endl;
        }

        thr.clear();
    }

    void runTheoryCheck() {
        // Vector with threads
        vector<thread> thr;

        // Run simulation to check theory
        for (int i = 0; i < num_threads; i++)
            thr.push_back(thread(&TheoryChecker::simulationTask, this, ref(s_th[i])));
        for (auto& t : thr)
            t.join();
    }
};

int main() {

    const int EndRange = 100000;
    const int StartRange = 4;

    for (int numThreads = 1; numThreads < 10; numThreads++) {
        {
            TheoryChecker theoryChecker(StartRange, EndRange, numThreads);
            auto start = high_resolution_clock::now();
            theoryChecker.runSimulation();
            auto stop = high_resolution_clock::now();
            cout << "Range: " << EndRange << ", Threads: " << numThreads << ", Time taken: "
                << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " ms" << endl;
        }
    }

    system("pause");

    return 0;
}
