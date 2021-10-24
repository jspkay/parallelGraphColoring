//
// Created by salvo on 11/10/21.
//

#include "Graph.h"

template<typename T>
void asa::Graph<T>::sequential() {
    int16_t color = -1;
    node current_vertex;
    forEachVertex(&current_vertex, [this, &color, &current_vertex]() {
        //ciclo su ogni vertice e cerco il colore da usare
        color = searchColor(current_vertex);
        static_cast<T &>(*this).graph[current_vertex].color = color; //coloro il vertice corrente
        color = -1;
    });
    printOutput("sequential-output.txt");
};

/// START JP, LDF Section
template<typename T>
void asa::Graph<T>::jp_structure(function<bool(int, node)> isMinor) {
    int done = 0;
    int roundMain = 0;
    int range = V / concurrentThreadsActive;
    int v_length = V; // No need for any vector
    int tcs_length[concurrentThreadsActive];
    bool toTerminate = false;
    std::function<void(int)> threadFn = [&](int id) {
        int round = 0;
        while (!toTerminate) {
            unique_lock<shared_timed_mutex> ulk(mutex, std::defer_lock);
            shared_lock<shared_timed_mutex> slk(mutex, std::defer_lock);

            // decido quali sono i vertici assegnati al thread (bilanciamento)
            int min = id * range, max = (id + 1) * range;
            // l'ultimo thread prende anche l'eccesso
            if (id == concurrentThreadsActive - 1) max = V;

            // allocazione dinamica
            unique_ptr<int[]> toColor_set(new int[max - min]);
            tcs_length[id] = 0;


#ifdef MULTITHREAD_DEBUG
            cout << '[' << round << "][" << id << "]m\n";
#endif
            // Find the verteces to be colored
            for (int i = min; i < max; i++) {
                node neighbor;
                bool major = true;
                if (static_cast<T &>(*this).graph[i].color != -1) major = false;
                else
                    forEachNeighbor(i, &neighbor, [this, &neighbor, i, &major, &isMinor]() {
                        // Non necessito lock: i thread agiscono su porzioni di memoria disgiunte
                        if (static_cast<T &>(*this).graph[neighbor].color == -1 && isMinor(i, neighbor))
                            major = false;
                    });
                if (major) {
                    toColor_set[tcs_length[id]++] = i;
                }
            }

#ifdef MULTITHREAD_DEBUG
            cout << '[' << round << "]Thread " << id << "start coloring!" << endl;
#endif

            // coloro i vertici
            for (int j = 0; j < tcs_length[id]; j++) {
                int i = toColor_set[j];
                int16_t color = searchColor(i);
                //non serve il lock perchè i vicini non sono stati selezionati per essere colorati
                //ulk.lock();
                static_cast<T &>(*this).graph[i].color = color;
                //ulk.unlock();
            }
#ifdef MULTITHREAD_DEBUG
            cout << '[' << round << "][" << id << "]ac\n";
#endif
            /// SINCRONIZZAZIONE FINE GIRO
            ulk.lock();
            done++;
            ulk.unlock();
            cv.notify_all();

#ifdef MULTITHREAD_DEBUG
            cout << '[' << round << "][ " << id << "]w2\n";
#endif

            slk.lock();
#if MTD == 1
            cout << id << "s\n";
#endif
            cv.wait(slk, [&roundMain, &round]() { return round < roundMain; });
            round++;
            slk.unlock();
        }
        cv.notify_all(); // necessary for the other threads to finish
#if MULTITHREAD_DEBUG || MTD == 1
        cout << id ;
#endif
    };

    // attivo i threads dopo averli definiti
    for (int i = 0; i < concurrentThreadsActive; i++) {
        threads.emplace_back(threadFn, i);
    }

    unique_lock<shared_timed_mutex> ulk(mutex, std::defer_lock);
    shared_lock<shared_timed_mutex> slk(mutex, std::defer_lock);

    /*** main thread ***/
    while (v_length > 0) {

#ifdef MULTITHREAD_DEBUG
        cout << "[ " << roundMain << "]MAIN THREAD i " << range << " = V(" << v_length << ") / t(" << concurrentThreadsActive << ")" << endl;
        cout << "Main thread waiting for others! " << endl;
#endif

        //aspetto gli altri thread
        slk.lock();
        cv.wait(slk, [&done, this]() { return done == concurrentThreadsActive; });
        // tolgo i vertici di toColor_set da verteces
        for (int i = 0; i < concurrentThreadsActive; i++)
            v_length -= tcs_length[i];
        if (v_length == 0)
            toTerminate = true;
        roundMain++;
        done = 0;
        slk.unlock();

#ifdef MULTITHREAD_DEBUG
        cout << "Rimanenti: " << v_length << endl;
#endif
        cv.notify_all();
    }

#ifdef MULTITHREAD_DEBUG
    cout << "Finished!" << endl;
    int j=0;
#endif
    for (auto &el : threads) {
        el.join();
#ifdef MULTITHREAD_DEBUG
        cout << j++ << " joined!" << endl;
#endif
    }
}

template<typename T>
void asa::Graph<T>::jp_structure_old(function<bool(int, node)> isMinor) {
    int done = 0, range;
    bool firstStepDone = false, secondStepDone = false;

    int v_length = V; // No need for any vector
    unique_ptr<int[]> toColor_set(new int[V]);
    int tcs_length = 0;

    std::function<void(int)> threadFn = [&](int id) {
        while (done >= 0) {
            unique_lock<shared_timed_mutex> ulk(mutex, std::defer_lock);
            shared_lock<shared_timed_mutex> slk(mutex, std::defer_lock);

            // decido quali sono i vertici assegnati al thread (bilanciamento)
            int min = id * range, max = (id + 1) * range;
            // l'ultimo thread prende anche l'eccesso
            if (id == concurrentThreadsActive - 1) max = V;

#ifdef MULTITHREAD_DEBUG
            static int i=0;
            i++;
            cout << '[' << i << "][" << id << "]m\n";
#endif
            // Find the verteces to be colored
            for (int i = min; i < max; i++) {
                node neighbor;
                bool major = true;
                if (static_cast<T &>(*this).graph[i].color != -1) major = false;
                else
                    forEachNeighbor(i, &neighbor, [this, &neighbor, i, &major, &isMinor]() {
                        // Non necessito lock: i thread agiscono su porzioni di
                        // memoria disgiunte
                        if (static_cast<T &>(*this).graph[neighbor].color == -1 &&
                            isMinor(i, neighbor)
                            //&&  (static_cast<T&>(*this).graph[i].random < static_cast<T&>(*this).graph[neighbor].random)
                                )
                            major = false;
                    });
                if (major) {
                    ulk.lock();
                    toColor_set[tcs_length++] = i;
                    ulk.unlock();
                }
            }

            /// SINCRONIZZAZIONE - aspetto il main thread
            ulk.lock();
            done++;
            firstStepDone = false;
            ulk.unlock();
            cv.notify_all(); // main thread

#ifdef MULTITHREAD_DEBUG
            cout << '[' << i << "][" << id << "]w1\n";
#endif
            // The shared_lock is acquired before the notify in or
            slk.lock();
            cv.wait(slk, [&firstStepDone]() { return firstStepDone; });
            slk.unlock();

#ifdef MULTITHREAD_DEBUG
            cout << '[' << i << "][" << id << "]a1\n";
#endif

            // inizio la colorazione
            min = id * range;
            max = (id + 1) * range;
            if (id == concurrentThreadsActive - 1) max = tcs_length;

#ifdef MULTITHREAD_DEBUG
            cout << '[' << i << "]Thread " << id << "start coloring!" << endl;
#endif

            // coloro i vertici
            for (int j = min; j < max; j++) {
                int i = toColor_set[j];
                int16_t color = searchColor(i);

                ulk.lock();
                static_cast<T &>(*this).graph[i].color = color;
                ulk.unlock();
            }
#ifdef MULTITHREAD_DEBUG
            cout << '[' << i << "][" << id << "]ac\n";
#endif
            /// SINCRONIZZAZIONE
            ulk.lock();
            done++;
            secondStepDone = false;
            ulk.unlock();
            cv.notify_all();

#ifdef MULTITHREAD_DEBUG
            cout << '[' << i << "][ " << id << "]w2\n";
#endif

            slk.lock();
#if MTD == 1
            cout << id << "s\n";
#endif
            cv.wait(slk, [&secondStepDone]() { return secondStepDone; });
            slk.unlock();
        }
        cv.notify_all(); // necessary for the other threads to finish
#if MULTITHREAD_DEBUG || MTD == 1
        cout << id << "r"<< secondStepDone<<'\n';
#endif
        return;
    };

    // attivo i threads
    range = V / concurrentThreadsActive;
    for (int i = 0; i < concurrentThreadsActive; i++) {
        threads.emplace_back(threadFn, i);
    }

    unique_lock<shared_timed_mutex> ulk(mutex, std::defer_lock);
    shared_lock<shared_timed_mutex> slk(mutex, std::defer_lock);

    // main thread
    while (v_length > 0) {

#ifdef MULTITHREAD_DEBUG
        static int i = 0;
        cout << "[ " << ++i << "]MAIN THREAD i " << range << " = V(" << v_length << ") / t(" << concurrentThreadsActive << ")" << endl;
        cout << "[ " << i << "]Main thread waiting for the threads" << endl;
#endif

        slk.lock();
        cv.wait(slk, [&done, this]() { return done == concurrentThreadsActive; });
        range = tcs_length / concurrentThreadsActive;
        done = 0;
        firstStepDone = true;
        slk.unlock();
        cv.notify_all(); // sblocco gli altri thread che iniziano la colorazione

#ifdef MULTITHREAD_DEBUG
        cout << "MAIN THREAD ii " << range << " = V(" << tcs_length << ") / t(" << concurrentThreadsActive << ")" << endl;
        cout << "Main thread updating verteces" << endl;
#endif

        // tolgo i vertici di toColor_set da verteces
        v_length -= tcs_length;

        //cout << "Rimanenti: " << verteces.size() << endl;

#ifdef MULTITHREAD_DEBUG
        cout << "Main thread waiting for others! " << endl;
#endif

        //aspetto gli altri thread
        slk.lock();
        cv.wait(slk, [&done, this]() { return done == concurrentThreadsActive; });
        tcs_length = 0;
        secondStepDone = true;
        range = V / concurrentThreadsActive;
        if (v_length == 0) done = -1;
        else done = 0;
        slk.unlock();
#ifdef MULTITHREAD_DEBUG
        cout << "Rimanenti: " << v_length << endl;
#endif
        cv.notify_all();
    }

#ifdef MULTITHREAD_DEBUG
    cout << "Finished!" << endl;
#endif

    for (auto &el : threads) {
        el.join();
#ifdef MULTITHREAD_DEBUG
        static int i=0;
        cout << i++ << " joined!" << endl;
#endif
    }
}

template<typename T>
void asa::Graph<T>::jonesPlassmann() {
    cout << "I am the new algorithm!" << endl;
    jp_structure([this](int i, node neighbor) {
        return static_cast<T &>(*this).graph[neighbor].color == -1 &&
               (static_cast<T &>(*this).graph[i].random < static_cast<T &>(*this).graph[neighbor].random);
    });
}

template<typename T>
void asa::Graph<T>::jonesPlassmann_old() {
    cout << "I am the old algorithm!" << endl;
    jp_structure_old([this](int i, node neighbor) {
        return static_cast<T &>(*this).graph[neighbor].color == -1 &&
               (static_cast<T &>(*this).graph[i].random < static_cast<T &>(*this).graph[neighbor].random);
    });
}

template<typename T>
void asa::Graph<T>::ldf() {
    jp_structure([this](int i, node neighbor) {
        return getDegree(i) < getDegree(neighbor) ||
               (getDegree(i) == getDegree(neighbor) &&
                static_cast<T &>(*this).graph[i].random < static_cast<T &>(*this).graph[neighbor].random
               );
    });
}
/// END JP, LDF Section

template<typename T>
void asa::Graph<T>::ldf_mod() {
    int done = 0, range;
    bool firstStepDone = false, secondStepDone = false;

    int v_length = V; // No need for any vector
    unique_ptr<int[]> toColor_set(new int[V]);
    int tcs_length = 0;

    std::function<void(int)> threadFn = [&](int id) {
        unique_lock<shared_timed_mutex> ulk(mutex, std::defer_lock);

        int v = rand() % V;
        int c;
        bool major = true;
        while (v_length > 0) {
            c = static_cast<T &>(*this).graph[v].color;
            if (c == -1) { // se non è colorato
                node neighbor;
                major = true;
                int d = getDegree(v);
                forEachNeighbor(v, &neighbor, [d, v, &neighbor, &major, this]() {
                    if (static_cast<T &>(*this).graph[neighbor].color != -1) return;
                    if (d < getDegree(neighbor))
                        major = false;
                    else if (d == getDegree(neighbor) &&
                             static_cast<T &>(*this).graph[v].random < static_cast<T &>(*this).graph[neighbor].random
                            )
                        major = false;
                });
                if (major) {
                    ulk.lock();
                    if (static_cast<T &>(*this).graph[v].color == -1) {
                        int color = searchColor(v);
                        static_cast<T &>(*this).graph[v].color = color;
                        v_length--;
                    }
                    ulk.unlock();
                }
            }
            v++;
            v %= V;
        }
        return;
    };

    for (int i = 0; i < concurrentThreadsActive; i++) {
        threads.emplace_back(threadFn, i);
    }

    for (auto &el : threads) {
        el.join();
#ifdef MULTITHREAD_DEBUG
        static int i=0;
        cout << i++ << " joined!" << endl;
#endif
    }
}

template<typename T>
void asa::Graph<T>::largestDegree() {
    fillTotalSet();
    /*** creazione thread ***/
    for (int n = 0; n < concurrentThreadsActive; n++) {
        threads.emplace_back([this, n]() {
            node current_vertex;
            bool major = true, doContinueWhile = true;
            while (doContinueWhile) {
                major = true;
                std::unique_lock<std::shared_timed_mutex> ulk(mutex);
                /*** terminazione thread ***/
                if (total_set.size() == 0) {
#ifdef MULTITHREAD_DEBUG
                    cout << "Thread" << n << "is going to end" << endl;
#endif
                    active_threads--;
                    if (active_threads == 0) {
                        //riattivo main thread
                        isEnded = true;
                        cv.notify_one();
                    }
                    doContinueWhile = false;
                } else {
                    /*** pop ***/
                    //cout << "Thread " << n << " started" << endl;
                    current_vertex = total_set.front();
                    total_set.pop_front();
                    ulk.unlock();
                    /*** ricerca massimo locale ***/
                    std::shared_lock<std::shared_timed_mutex> slk(mutex);
                    node neighbor;
                    forEachNeighbor(current_vertex, &neighbor, [this, &neighbor, current_vertex, &major]() {
                        //confronto con il vicino solo se è non-colorato -> color = -1
                        if (static_cast<T &>(*this).graph[neighbor].color == -1) {
                            if (getDegree(current_vertex) < getDegree(neighbor)) {
                                major = false;
                                return;
                            } else if (getDegree(current_vertex) == getDegree(neighbor))
                                if (static_cast<T &>(*this).graph[current_vertex].random <
                                    static_cast<T &>(*this).graph[neighbor].random) {
                                    major = false;  //A PARITA' DI DEGREE VEDO RANDOM
                                    return;
                                }
                        }
                    });
                    slk.unlock();
                    /*** azione in base all'esito ***/
                    if (major) {
                        std::shared_lock<std::shared_timed_mutex> slk(mutex);
                        /*** colorazione ***/
                        int16_t color = -1;
                        color = searchColor(current_vertex);
                        slk.unlock();
                        //qui devo modificare grafo e mi serve unique lock
                        ulk.lock();
                        static_cast<T &>(*this).graph[current_vertex].color = color; //coloro il vertice corrente
                    } else {
                        ulk.lock();
                        /*** reinserisco (nodo da colorare successivamente) ***/
                        total_set.push_back(current_vertex);
                    }
                    cv.notify_all();
                }
#ifdef MULTITHREAD_DEBUG
                cout << "rimanenti: " << total_set.size() << endl;
#endif
            }
#ifdef MULTITHREAD_DEBUG
            cout << "Thread " << n << " finished" << endl;
#endif
        });
    }
    /*** main thread ***/
    std::unique_lock<std::shared_timed_mutex> ulk(mutex);
    //aspetto termini algoritmo
    cv.wait(ulk, [this]() { return isEnded; });

    for (auto &el : threads) {
        el.join();
#ifdef MULTITHREAD_DEBUG
        static int i=0;
        cout << i++ << " joined!" << endl;
#endif
    }

    printOutput("largestDegree-output.txt");
};

template<typename T>
void asa::Graph<T>::smallestDegree() {
    /***
         weighting phase
         k = numIteration - i = current_color
     ***/
    numIteration = 1;
    int current_weigth = 0;
    node current_vertex;
    fillTotalSet();
    /*** creazione thread ***/
    for (int n = 0; n < concurrentThreadsActive; n++) {
        threads.emplace_back([this]() {
            node current_vertex;
            bool minor, doContinueWhile = true;
            while (doContinueWhile) {
                minor = false;
                std::unique_lock<std::shared_timed_mutex> ulk(mutex);
                /*** terminazione thread ***/
                if (total_set.empty()) {
                    active_threads--;
                    if (active_threads == 0) {
                        isEnded = true;
                        cv.notify_one();
                    }
                    doContinueWhile = false;
                } else {
                    /*** pop dalla coda ***/
                    current_vertex = total_set.front();
                    total_set.pop_front();
                    //SE STO INIZIANDO UN NUOVO GIRO, ALLORA ASPETTO CHE TUTTI PRIMA SIANO PESATI
                    if (static_cast<T &>(*this).graph[current_vertex].num_it > numIteration) {
                        increase_numIteration++;
                        cv.notify_all();
                    }
                    //SE TUTTI SONO STATI PESATI, increase_numIteration = 0
                    cv.wait(ulk, [this, current_vertex]() {
                        return static_cast<T &>(*this).graph[current_vertex].num_it <= numIteration ||
                               !increase_numIteration;
                    });
                    ulk.unlock();
                    /*** confronto con i vicini ***/
                    std::shared_lock<std::shared_timed_mutex> slk(mutex);
                    /*** calcolo degree current_vertex (non posso usare getDegree) ***/
                    int degreeCurrVertex = computeDegree(current_vertex);
                    /*** current_vertex ha smallest degree? ***/
                    if (degreeCurrVertex <= numIteration)
                        minor = true;
                    slk.unlock();
                    /*** azione in base all'esito ***/
                    ulk.lock();
                    if (minor) {
                        /*** aggiungo a nodi da pesare se minore tra i vicini ***/
                        toColor_set.push_back(current_vertex);
                    } else {
                        /*** reinserisco in coda,valuto al prossimo giro ***/
                        static_cast<T &>(*this).graph[current_vertex].num_it++;
                        total_set.push_back(current_vertex);
                    }
                    cv.notify_all();
                }
            }
        });
    }
    /*** main thread ***/
    bool doContinueWhile = true;
    while (doContinueWhile) {
        std::unique_lock<std::shared_timed_mutex> ulk(mutex);
        cv.wait(ulk, [this]() { return isEnded || increase_numIteration == active_threads; });
        bool doIHaveToIncreaseColor = toColor_set.size() != 0;
        while (!toColor_set.empty()) {
            /*** pop da coda + weight + mark deleted ***/
            current_vertex = toColor_set.front();
            toColor_set.pop_front();
            static_cast<T &>(*this).graph[current_vertex].weight = current_weigth; //peso il vertice corrente
            static_cast<T &>(*this).graph[current_vertex].toBeDeleted = true;
        }
        if (isEnded)
            doContinueWhile = false;
        //se almeno un nodo aveva il current_weigth, allora devo incrementarlo per il prossimo giro
        if (doIHaveToIncreaseColor)
            current_weigth++;
        increase_numIteration = 0;
        numIteration++;
        cv.notify_all();
        std::cout << "rimanenti: " << total_set.size() << std::endl;
    }
    /*** reset prima di colorare ***/
    total_set.clear();
    toColor_set.clear();
    for (std::thread &t : threads)
        t.join();
    threads.clear();
    active_threads = concurrentThreadsActive;
    isEnded = false;
    /***
         coloring phase
     ***/
    fillTotalSet();
    int C[256]{}, wei;
    /*** creazione thread ***/
    for (int n = 0; n < concurrentThreadsActive; n++) {
        threads.emplace_back([this, &wei]() {
                                 node current_vertex;
                                 bool doContinueWhile = true;
                                 while (doContinueWhile) {
                                     std::unique_lock<std::shared_timed_mutex> ulk(mutex);
                                     cv.wait(ulk, [this]() { return !toColor_set.empty() || total_set.empty(); });
                                     /*** terminazione finale thread ***/
                                     if (toColor_set.empty() && total_set.empty()) {
                                         active_threads--;
                                         if (active_threads == 0) {
                                             isEnded = true;
                                             cv.notify_one();
                                         }
                                         cv.notify_all();
                                         doContinueWhile = false;
                                     } else {
                                         /*** pop dalla coda ***/
                                         current_vertex = toColor_set.front();
                                         toColor_set.pop_front();
                                         ulk.unlock();
                                         /*** coloring ***/
                                         std::shared_lock<std::shared_timed_mutex> slk(mutex);
                                         int16_t color = searchColor(current_vertex);
                                         slk.unlock();
                                         ulk.lock();
                                         static_cast<T &>(*this).graph[current_vertex].color = color; //coloro il vertice corrente
                                         cv.notify_all();
                                     }
                                 }
                             }
        );
    }
    /*** coloro per peso decrescente ***/
    for (wei = current_weigth; wei >= 0; wei--) {
        forEachVertex(&current_vertex, [this, &current_vertex, current_weigth, wei, &C]() {
            std::unique_lock<std::shared_timed_mutex> ulk(mutex);
            /*** terminazione lambda***/
            if (total_set.empty())
                return;
            /*** pop coda ***/
            current_vertex = total_set.front();
            total_set.pop_front();
            if (static_cast<T &>(*this).graph[current_vertex].weight == wei) {
                /*** inserisco nei vertici da colorare ***/
                toColor_set.push_back(current_vertex);
            } else
                /*** reinserisco se weight minore ***/
                total_set.push_back(current_vertex);
            cv.notify_all();
        });
        /*** sincronizzazione a ogni fine ciclo, prima di aggiornare wei ***/
        std::unique_lock<std::shared_timed_mutex> ulk(mutex);
#ifdef MULTITHREAD_DEBUG
        std::cout << "rimanenti: " << total_set.size() << std::endl;
#endif
        cv.wait(ulk, [this]() { return toColor_set.empty(); });
    }
    std::unique_lock<std::shared_timed_mutex> ulk(mutex);
    cv.wait(ulk, [this]() { return isEnded; });

    for (auto &el : threads) {
        el.join();
#ifdef MULTITHREAD_DEBUG
        static int i=0;
        cout << i++ << " joined!" << endl;
#endif
    }

    printOutput("smallestDegree-output.txt");
}

// Necessari perché i template non sono negli header
template
class asa::Graph<asa::GraphCSR>;

template
class asa::Graph<asa::GraphAdjL>;

template
class asa::Graph<asa::GraphAdjM>;
