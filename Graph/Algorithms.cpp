//
// Created by salvo on 11/10/21.
//

#include "Graph.h"

template <typename T>
void asa::Graph<T>::sequential(){
    int16_t color=-1;
    node current_vertex;
    forEachVertex(&current_vertex,[this,&color,&current_vertex](){
        //ciclo su ogni vertice e cerco il colore da usare
        color = searchColor(current_vertex);
        static_cast<T&>(*this).graph[current_vertex].color = color; //coloro il vertice corrente
        color = -1;
    });
    printOutput("sequential-output.txt");
};

template <typename T>
void asa::Graph<T>::JP_mod() {
    int done = 0, range;
    bool firstStepDone = false, secondStepDone = false;

    int v_length = V; // No need for any vector
    unique_ptr<int[]> toColor_set(new int[V]);
    int tcs_length = 0;

    std::function<void(int)> threadFn = [&](int id){
        while(true) {
            static int i=0;
            unique_lock<shared_timed_mutex> ulk(mutex, std::defer_lock);
            shared_lock<shared_timed_mutex> slk(mutex, std::defer_lock);

            // decido quali sono i vertici assegnati al thread (bilanciamento)
            int min = id*range, max = (id+1)*range;
            // l'ultimo thread prende anche l'eccesso
            if (id == concurrentThreadsActive - 1) max = V;

#ifdef MULTITHREAD_DEBUG
            i++;
            cout << '[' << i << "][" << id << "]m\n";
#endif
            // Find the verteces to be colored
            for(int i=min; i<max; i++){
                node neighbor;
                bool major = true;
                if (static_cast<T&>(*this).graph[i].color != -1 ) major = false;
                else forEachNeighbor(i, &neighbor, [this, &neighbor, i, &major]() {
                    // Non necessito lock: i thread agiscono su porzioni di
                    // memoria disgiunte
                    if( static_cast<T&>(*this).graph[neighbor].color == -1 &&
                        (static_cast<T&>(*this).graph[i].random < static_cast<T&>(*this).graph[neighbor].random)
                        )
                        major = false;
                });
                if (major) {
                    ulk.lock();
                    toColor_set[tcs_length] = i;
                    tcs_length += 1;
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
            cv.wait(slk, [&firstStepDone](){return firstStepDone;});
            if(done < 0) break;
            slk.unlock();

#ifdef MULTITHREAD_DEBUG
            cout << '[' << i << "][" << id << "]a1\n";
#endif

            // inizio la colorazione
            min = id * range;
            max = (id+1) * range;
            if(id == concurrentThreadsActive - 1) max = tcs_length;

#ifdef MULTITHREAD_DEBUG
            cout << '[' << i << "]Thread " << id << "start coloring!" << endl;
#endif

            // coloro i vertici
            for(int j=min; j<max; j++){
                int i = toColor_set[j];
                int16_t color = searchColor(i);

                ulk.lock();
                static_cast<T&>(*this).graph[i].color = color;
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
            cv.wait(slk, [&secondStepDone](){return secondStepDone;});
            if(done < 0) break;
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
    for(int i = 0; i < concurrentThreadsActive; i++) {
        threads.emplace_back(threadFn, i);
    }

    unique_lock<shared_timed_mutex> ulk(mutex, std::defer_lock);
    shared_lock<shared_timed_mutex> slk(mutex, std::defer_lock);

    // main thread
    while(v_length > 0){

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
        cv.wait(slk, [&done, this](){return done == concurrentThreadsActive;});
        tcs_length = 0;
        secondStepDone = true;
        range = V/concurrentThreadsActive;
        if(v_length == 0) done = -1;
        else done = 0;
        slk.unlock();
        cv.notify_all();
    }

#ifdef MULTITHREAD_DEBUG
    cout << "Finished!" << endl;
#endif

    for(auto &el : threads) {
        el.join();
#ifdef MULTITHREAD_DEBUG
        static int i=0;
        cout << i++ << " joined!" << endl;
#endif
    }

}

template <typename T>
void asa::Graph<T>::largest_mod() {
    int done = 0, range;
    bool firstStepDone = false, secondStepDone = false;

    set<int> verteces, toColor_set;
    for(int i=0; i<V; i++) verteces.insert(i);



    std::function<void(int)> threadFn = [&](int id){
        int i=0;
        while(true) {
            unique_lock<shared_timed_mutex> ulk(mutex, std::defer_lock);
            shared_lock<shared_timed_mutex> slk(mutex, std::defer_lock);

            // decido quali sono i vertici assegnati al thread (bilanciamento)
            auto begin = verteces.begin();
            auto min = verteces.begin(),
                    max = verteces.begin();
            advance(min, id*range);
            advance(max, (id+1)*range);
            // l'ultimo thread prende anche l'eccesso
            if (id == concurrentThreadsActive - 1) max = verteces.end();

#ifdef MULTITHREAD_DEBUG
            i++;
            cout << '[' << i << "][" << id << "]m\n";
#endif
            // Find the verteces to be colored
            for (; min != max; min++) { // per ogni vertice assegnato al thread
                //cout << "Giro[" << id << "]";
                node neighbor;
                bool major = true;
                int i = *min;
                forEachNeighbor(i, &neighbor, [this, &neighbor, i, &major, &id]() {
                    // Non necessito lock: i thread agiscono su porzioni di
                    // memoria disgiunte
                    if( static_cast<T&>(*this).graph[neighbor].color == -1 &&
                        ( getDegree(i) < getDegree(neighbor) ||
                            ( getDegree(i) == getDegree(neighbor) &&
                            static_cast<T&>(*this).graph[i].random < static_cast<T&>(*this).graph[neighbor].random
                            )
                        )){
                        major = false;
                        return;
                    }
                });
                if (major) {
                    ulk.lock();
                    toColor_set.insert(i);
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
            cv.wait(slk, [&firstStepDone](){return firstStepDone;});
            if(done < 0) break;
            slk.unlock();

#ifdef MULTITHREAD_DEBUG
            cout << '[' << i << "][" << id << "]a1\n";
#endif
            /// SINCRONIZZAZIONE END

            // inizio la colorazione
            min = toColor_set.begin();
            max = toColor_set.begin();
            advance(min, id*range);
            advance(max, (id+1)*range);
            if (id == concurrentThreadsActive - 1) max = toColor_set.end();

#ifdef MULTITHREAD_DEBUG
            cout << '[' << i << "]Thread " << id << "start coloring!" << endl;
#endif

            // coloro i vertici
            for (; min != max; min++) {
                int i = *min;

                slk.lock();
                int16_t color = searchColor(i);
                slk.unlock();

                ulk.lock();
                static_cast<T&>(*this).graph[i].color = color;
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
            cv.wait(slk, [&secondStepDone](){return secondStepDone;});
            if(done < 0) break;
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
    for(int i = 0; i < concurrentThreadsActive; i++) {
        threads.emplace_back(threadFn, i);
    }

    unique_lock<shared_timed_mutex> ulk(mutex, std::defer_lock);
    shared_lock<shared_timed_mutex> slk(mutex, std::defer_lock);

    // main thread
    while(verteces.size() > 0){

#ifdef MULTITHREAD_DEBUG
        static int i = 0;
        cout << "[ " << ++i << "]MAIN THREAD i " << range << " = V(" << verteces.size() << ") / t(" << concurrentThreadsActive << ")" << endl;
        cout << "[ " << i << "]Main thread waiting for the threads" << endl;
#endif

        slk.lock();
        cv.wait(slk, [&done, this]() { return done == concurrentThreadsActive; });
        range = toColor_set.size() / concurrentThreadsActive;
        done = 0;
        firstStepDone = true;
        slk.unlock();
        cv.notify_all(); // sblocco gli altri thread che iniziano la colorazione

#ifdef MULTITHREAD_DEBUG
        cout << "MAIN THREAD ii " << range << " = V(" << toColor_set.size() << ") / t(" << concurrentThreadsActive << ")" << endl;
        cout << "Main thread updating verteces" << endl;
#endif

        // tolgo i vertici di toColor_set da verteces
        for (auto el: toColor_set){
            //unique_lock<shared_timed_mutex> lk(mutex);
            // Non ho bisogno del lock perché nessuno accede a verteces a parte questo thread
            verteces.erase(el);
        }

        //cout << "Rimanenti: " << verteces.size() << endl;

#ifdef MULTITHREAD_DEBUG
        cout << "Main thread waiting for others! " << endl;
#endif

        //aspetto gli altri thread
        slk.lock();
        cv.wait(slk, [&done, this](){return done == concurrentThreadsActive;});
        toColor_set.clear();
        secondStepDone = true;
        if(verteces.size() == 0) done = -1;
        else done = 0;
        slk.unlock();
        cv.notify_all();
    }

#ifdef MULTITHREAD_DEBUG
    cout << "Finished!" << endl;
#endif

    for(auto &el : threads) {
        el.join();
#ifdef MULTITHREAD_DEBUG
        static int i=0;
        cout << i++ << " joined!" << endl;
#endif
    }

}

template <typename T>
void asa::Graph<T>::smallest_mod() {
    int done = 0, range;
    bool firstStepDone = false, secondStepDone = false;

    set<int> verteces, toColor_set;
    for(int i=0; i<V; i++) verteces.insert(i);



    std::function<void(int)> threadFn = [&](int id){
        int i=0;
        while(true) {
            unique_lock<shared_timed_mutex> ulk(mutex, std::defer_lock);
            shared_lock<shared_timed_mutex> slk(mutex, std::defer_lock);

            // decido quali sono i vertici assegnati al thread (bilanciamento)
            auto begin = verteces.begin();
            auto min = verteces.begin(),
                    max = verteces.begin();
            advance(min, id*range);
            advance(max, (id+1)*range);
            // l'ultimo thread prende anche l'eccesso
            if (id == concurrentThreadsActive - 1) max = verteces.end();

#ifdef MULTITHREAD_DEBUG
            i++;
            cout << '[' << i << "][" << id << "]m\n";
#endif
            // Find the verteces to be colored
            for (; min != max; min++) { // per ogni vertice assegnato al thread
                //cout << "Giro[" << id << "]";
                node neighbor;
                bool minor = true;
                int i = *min;
                forEachNeighbor(i, &neighbor, [this, &neighbor, i, &minor]() {
                    // Non necessito lock: i thread agiscono su porzioni di
                    // memoria disgiunte
                    if( static_cast<T&>(*this).graph[neighbor].color == -1 &&
                        ( getDegree(i) > getDegree(neighbor) ||
                          ( getDegree(i) == getDegree(neighbor) &&
                            static_cast<T&>(*this).graph[i].random < static_cast<T&>(*this).graph[neighbor].random
                          )
                        )){
                        minor = false;
                        return;
                    }
                });
                if (minor) {
                    ulk.lock();
                    toColor_set.insert(i);
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
            cv.wait(slk, [&firstStepDone](){return firstStepDone;});
            if(done < 0) break;
            slk.unlock();

#ifdef MULTITHREAD_DEBUG
            cout << '[' << i << "][" << id << "]a1\n";
#endif
            /// SINCRONIZZAZIONE END

            // inizio la colorazione
            min = toColor_set.begin();
            max = toColor_set.begin();
            advance(min, id*range);
            advance(max, (id+1)*range);
            if (id == concurrentThreadsActive - 1) max = toColor_set.end();

#ifdef MULTITHREAD_DEBUG
            cout << '[' << i << "]Thread " << id << "start coloring!" << endl;
#endif

            // coloro i vertici
            for (; min != max; min++) {
                int i = *min;

                slk.lock();
                int16_t color = searchColor(i);
                slk.unlock();

                ulk.lock();
                static_cast<T&>(*this).graph[i].color = color;
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
            cv.wait(slk, [&secondStepDone](){return secondStepDone;});
            if(done < 0) break;
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
    for(int i = 0; i < concurrentThreadsActive; i++) {
        threads.emplace_back(threadFn, i);
    }

    unique_lock<shared_timed_mutex> ulk(mutex, std::defer_lock);
    shared_lock<shared_timed_mutex> slk(mutex, std::defer_lock);

    // main thread
    while(verteces.size() > 0){

#ifdef MULTITHREAD_DEBUG
        static int i = 0;
        cout << "[ " << ++i << "]MAIN THREAD i " << range << " = V(" << verteces.size() << ") / t(" << concurrentThreadsActive << ")" << endl;
        cout << "[ " << i << "]Main thread waiting for the threads" << endl;
#endif

        slk.lock();
        cv.wait(slk, [&done, this]() { return done == concurrentThreadsActive; });
        range = toColor_set.size() / concurrentThreadsActive;
        done = 0;
        firstStepDone = true;
        slk.unlock();
        cv.notify_all(); // sblocco gli altri thread che iniziano la colorazione

#ifdef MULTITHREAD_DEBUG
        cout << "MAIN THREAD ii " << range << " = V(" << toColor_set.size() << ") / t(" << concurrentThreadsActive << ")" << endl;
        cout << "Main thread updating verteces" << endl;
#endif

        // tolgo i vertici di toColor_set da verteces
        for (auto el: toColor_set){
            //unique_lock<shared_timed_mutex> lk(mutex);
            // Non ho bisogno del lock perché nessuno accede a verteces a parte questo thread
            verteces.erase(el);
        }

        //cout << "Rimanenti: " << verteces.size() << endl;

#ifdef MULTITHREAD_DEBUG
        cout << "Main thread waiting for others! " << endl;
#endif

        //aspetto gli altri thread
        slk.lock();
        cv.wait(slk, [&done, this](){return done == concurrentThreadsActive;});
        toColor_set.clear();
        secondStepDone = true;
        if(verteces.size() == 0) done = -1;
        else done = 0;
        slk.unlock();
        cv.notify_all();
    }

#ifdef MULTITHREAD_DEBUG
    cout << "Finished!" << endl;
#endif

    for(auto &el : threads) {
        el.join();
#ifdef MULTITHREAD_DEBUG
        static int i=0;
        cout << i++ << " joined!" << endl;
#endif
    }

}

template <typename T>
void asa::Graph<T>::largestDegree(){
    fillTotalSet();
    /*** creazione thread ***/
    for(int n=0; n < concurrentThreadsActive; n++){
        threads.emplace_back([this, n](){
            node current_vertex;
            bool major = true, doContinueWhile = true;
            while (doContinueWhile) {
                major=true;
                std::unique_lock<std::shared_timed_mutex> ulk(mutex);
                /*** terminazione thread ***/
                if(total_set.size()==0){
#ifdef MULTITHREAD_DEBUG
                    cout << "Thread" << n << "is going to end" << endl;
#endif
                    active_threads--;
                    if(active_threads==0){
                        //riattivo main thread
                        isEnded = true;
                        cv.notify_one();
                    }
                    doContinueWhile = false;
                }else {
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

    for(auto &el : threads) {
        el.join();
#ifdef MULTITHREAD_DEBUG
        static int i=0;
        cout << i++ << " joined!" << endl;
#endif
    }

    printOutput("largestDegree-output.txt");
};

template <typename T>
void asa::Graph<T>::jonesPlassmann(){
    fillTotalSet();
    /*** creazione thread ***/
    for(int n=0; n < concurrentThreadsActive; n++){
        threads.emplace_back([this](){
            node current_vertex;
            bool major = true, doContinueWhile = true;
            while (doContinueWhile) {
                std::unique_lock<std::shared_timed_mutex> ulk(mutex);
                /*** terminazione thread ***/
                if (total_set.size() == 0) {
                    active_threads--;
                    if (active_threads == 0) {
                        if(toColor_set.size() != 0) {
                            //se c'è ancora qualche nodo lo coloro, perchè sono in terminazione
                            for(node current_vertex : toColor_set){
                                toColor_set.pop_front();
                                int16_t color = searchColor(current_vertex);
                                static_cast<T &>(*this).graph[current_vertex].color = color;
                            }
                        }
                        isEnded = true;
                    }
                    cv.notify_all();
                    doContinueWhile = false;
                } else {
                    /*** pop da coda ***/
                    current_vertex = total_set.front();
                    total_set.pop_front();
                    /*** sincronizzazione thread a fine di un giro completo sui nodi ***/
                    /* SE STO INIZIANDO UN NUOVO GIRO, ALLORA ASPETTO CHE TUTTI I NODI NEL SET DA COLORARE LO SIANO */
                    if (static_cast<T &>(*this).graph[current_vertex].num_it > numIteration && !doColor) {
                        increase_numIteration++;
                        cv.notify_all();
                    }
                    /* QUANDO THREAD CHE COLORA FINISCE DI FARLO, AUMENTA numIteration */
                    cv.wait(ulk, [this, current_vertex](){
                        return static_cast<T &>(*this).graph[current_vertex].num_it <= numIteration || !increase_numIteration || doColor;
                    });
                    if (doColor) {
                        /*** COLORAZIONE MULTITHREADING ***/
                        doColor--;
                        /*** undo pop ***/
                        total_set.push_front(current_vertex);
                        /*** coloro tutti con stesso colore ***/
                        current_vertex = toColor_set.front();
                        toColor_set.pop_front();
                        int16_t color = searchColor(current_vertex);
                        static_cast<T &>(*this).graph[current_vertex].color = color;
                        cv.notify_all();
                        continue;
                    }
                    ulk.unlock();
                    /*** ricerca massimo ***/
                    std::shared_lock<std::shared_timed_mutex> slk(mutex);
                    /*** confronto con i vicini ***/
                    node neighbor;
                    forEachNeighbor(current_vertex, &neighbor, [this, &neighbor, current_vertex, &major]() {
                        if (static_cast<T &>(*this).graph[neighbor].color == -1) {
                            if (static_cast<T &>(*this).graph[current_vertex].random <
                                static_cast<T &>(*this).graph[neighbor].random) {
                                major = false;
                                return;
                            } else if (static_cast<T &>(*this).graph[current_vertex].random ==
                                       static_cast<T &>(*this).graph[neighbor].random) {
                                if (current_vertex < neighbor) {
                                    major = false;
                                    return;
                                }
                            }
                        }
                    });
                    slk.unlock();
                    /*** azione in base all'esito ***/
                    ulk.lock();
                    if (major) {
                        /*** da colorare ***/
                        toColor_set.push_back(current_vertex);
                    } else {
                        /*** reinserisco (nodo da colorare successivamente) ***/
                        static_cast<T &>(*this).graph[current_vertex].num_it++;
                        total_set.push_back(current_vertex);
                        major = true;
                    }
                    cv.notify_all();
                }
            }
        });
    }
    /*** main thread ***/
    bool doContinueWhile = true;
    while(doContinueWhile) {
        std::unique_lock<std::shared_timed_mutex> ulk(mutex);
        //cout << "--------->ottenuto unique" << endl;
        cv.wait(ulk, [this]() { return isEnded || increase_numIteration == active_threads; });  //NON concurrent !!!!
        if(isEnded)
            doContinueWhile = false;
        //means -> increase_numIteration == true
        doColor = toColor_set.size();
        cv.notify_all();
        cv.wait(ulk,[this](){return !doColor; });
        increase_numIteration = 0;
        numIteration ++; //posso iniziare un nuovo giro
#ifdef MULTITHREAD_DEBUG
        cout << "rimanenti: " << total_set.size() << endl;
#endif
        cv.notify_all();
    }

    for(auto &el : threads) {
        el.join();
#ifdef MULTITHREAD_DEBUG
        static int i=0;
        cout << i++ << " joined!" << endl;
#endif
    }

    //////////////////////////////////////////////////////////////////////////
    printOutput("jp-output.txt");
};

template <typename T>
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
                }else {
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
                        return static_cast<T &>(*this).graph[current_vertex].num_it <= numIteration || !increase_numIteration;
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
        while (!toColor_set.empty()){
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
                                     }else{
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
            if(total_set.empty())
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

    for(auto &el : threads) {
        el.join();
#ifdef MULTITHREAD_DEBUG
        static int i=0;
        cout << i++ << " joined!" << endl;
#endif
    }

    printOutput("smallestDegree-output.txt");
}

// Necessari perché i template non sono negli header
template class asa::Graph<asa::GraphCSR>;
template class asa::Graph<asa::GraphAdjL>;
template class asa::Graph<asa::GraphAdjM>;
