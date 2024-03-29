#pragma once
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
    TheoryChecker(int init = 4, int finish = 1000, int num_threads = 10)
        : search{ init, finish }, num_threads(num_threads), s_th{ new Search[num_threads] } {}

    ~TheoryChecker() {
        delete[] s_th;
    }

    void runSimulation() {
        prepareData();
        runTheoryCheck();
        printTimingResults();
    }

private:
    mutex m_f1, m_f2, m_f3;
    mutex m_console;

    vector<int> vec_prime, vec_even;

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
        // Simulation task
        bool theory_check;
        int check_number;
        do {
            theory_check = false;

            // pick number from vector and delete it
            // back pop_back
            m_f1.lock();
            if (vec_even.empty()) {
                // abort this task if vector is empty
                m_f1.unlock();
                break;
            }
            // take number
            check_number = vec_even.back();
            // delete this number from vector
            vec_even.pop_back();
            m_f1.unlock();
            if (check_number == 2) {
                // Skip value 2
                m_f1.lock();
                if (vec_even.empty()) {
                    // abort if there is no data stored
                    m_f1.unlock();
                    break;
                }
                // take another number
                check_number = vec_even.back();
                vec_even.pop_back();
                m_f1.unlock();
            }

            // verify theory, from vector try to make sum of two prime numbers equal to prime number
            for (auto it = vec_prime.begin(); it != vec_prime.end(); ++it) {
                for (auto itt = vec_prime.begin(); itt != vec_prime.end(); ++itt) {
                    if (*it + *itt == check_number) {
                        // if theory for this example numbers is true
                        theory_check = true;
                        break;
                    }
                }
                if (theory_check) {
                    // founded numbers
                    break;
                }
            }
            if (!theory_check) {
                // theory failed
                m_console.lock();
                cout << "Theory fail: " << check_number << endl;
                m_console.unlock();
                break;
            }
        } while (!vec_even.empty());
    }

    void dataGenerationTask(Search& s) {
        // Task to generate data for simulation
        for (int i = s.init; i <= s.finish; i++) {
            if (firstNum(i)) {
                // Write to vector prime number
                m_f2.lock();
                vec_prime.push_back(i);
                m_f2.unlock();
            }
            if (evenNum(i)) {
                // Write to vector even number
                m_f3.lock();
                vec_even.push_back(i);
                m_f3.unlock();
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

        // Time stamp
        auto start1 = high_resolution_clock::now();

        // Prepare data with threads
        for (int i = 0; i < num_threads; i++)
            thr.push_back(thread(&TheoryChecker::dataGenerationTask, this, ref(s_th[i])));
        for (auto& t : thr)
            t.join();
        // Clear vector
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

    void printTimingResults() {
        // End Time stamp
        auto stop1 = high_resolution_clock::now();
        // Timing Results
        cout << "Time taken: " << duration_cast<microseconds>(stop1 - start1).count() / 1000.0 << " ms" << endl;
        system("pause");
    }
};