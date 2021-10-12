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
    bool canOtherRun = false;

    set<int> verteces, toColor_set;
    for(int i=0; i<V; i++) verteces.insert(i);

    unique_lock<shared_timed_mutex> ulk(mutex, std::defer_lock);
    shared_lock<shared_timed_mutex> slk(mutex, std::defer_lock);

    std::function<void(int)> threadFn = [&](int id){
        while(true) {
            // decido quali sono i vertici assegnati al thread (bilanciamento)
            auto min = verteces.begin(),
                 max = verteces.begin();
            advance(min, id*range);
            advance(max, (id+1)*range);
            // l'ultimo thread prende anche l'eccesso
            if (id == concurrentThreadsActive - 1) max = verteces.end();

#ifdef MULTITHREAD_DEBUG
            cout << "Thread " << id << " search for maxima!" << endl;
#endif
            // Find the verteces to be colored
            for (; min != max; min++) { // per ogni vertice assegnato al thread
                node neighbor;
                bool major = true;
                int i = *min;
                forEachNeighbor(i, &neighbor, [this, &neighbor, i, &major]() {
                    // Non necessito lock: i thread agiscono su porzioni di
                    // memoria disgiunte
                    if (static_cast<T&>(*this).graph[i].random < static_cast<T&>(*this).graph[neighbor].random)
                        major = false;
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
            ulk.unlock();
            cv.notify_all(); // main thread

#ifdef MULTITHREAD_DEBUG
            cout << "Thread " << id << " waiting for all the others! [am]" << endl;
#endif
            // The shared_lock is acquired before the notify in or
            slk.lock();
            cv.wait(slk, [&canOtherRun](){return canOtherRun;});
            slk.unlock();
#ifdef MULTITHREAD_DEBUG
            cout << "Thread " << id << " continue for colorization!" << endl;
#endif
            /// SINCRONIZZAZIONE END

            // inizio la colorazione
            min = toColor_set.begin();
            max = toColor_set.begin();
            advance(min, id*range);
            advance(max, (id+1)*range);
            if (id == concurrentThreadsActive - 1) max = toColor_set.end();

#ifdef MULTITHREAD_DEBUG
            cout << "Thread " << id << "start coloring!" << endl;
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

            /// SINCRONIZZAZIONE
            ulk.lock();
            done++;
            ulk.unlock();
            cv.notify_all();

            slk.lock();
            cv.wait(slk, [&canOtherRun](){return canOtherRun;});
            slk.unlock();
            if(done < 0) break;
        }
    };

    // attivo i threads
    range = V / concurrentThreadsActive;
    for(int i = 0; i < concurrentThreadsActive; i++) {
        threads.emplace_back(threadFn, i);
    }

    // main thread
    while(verteces.size() > 0){

#ifdef MULTITHREAD_DEBUG
        cout << "MAIN THREAD i " << range << " = V(" << verteces.size() << ") / t(" << concurrentThreadsActive << ")" << endl;
        cout << "Main thread waiting for the threads" << endl;
#endif

        slk.lock();
        cv.wait(slk, [&done, this]() { return done == concurrentThreadsActive; });
        range = toColor_set.size() / concurrentThreadsActive;
        done = 0;
        canOtherRun = true;
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

#ifdef MULTITHREAD_DEBUG
        cout << "Main thread waiting for others! " << endl;
#endif

        //aspetto gli altri thread
        lk.lock();
        cv.wait(lk, [&done, this](){return done == concurrentThreadsActive;});
        canOtherRun = true;
        lk.unlock();
    }

    {
        unique_lock<shared_timed_mutex> lk(mutex);
        done = -1;
        cv.notify_all();
    }

    for(auto &el : threads) el.join();

}

template class asa::Graph<asa::GraphCSR>;
template class asa::Graph<asa::GraphAdjL>;
template class asa::Graph<asa::GraphAdjM>;
