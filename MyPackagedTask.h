//
// Created by antonio_vespa on 23/09/21.
//

#ifndef GRAPHCOLORING_MYPACKAGEDTASK_H
#define GRAPHCOLORING_MYPACKAGEDTASK_H

#include <tuple>

template<typename F, typename ...Args>
class MyPackagedTask {
    F& f;
    std::tuple<Args...> args;
    typedef decltype(apply(f,args)) R;
public:
    explicit MyPackagedTask(F&&fn,Args&&... args): f(std::forward<F>(fn)), args(std::forward<Args>(args)...){}
    R operator() (){
        return appply(f, args);
    }
};


#endif //GRAPHCOLORING_MYPACKAGEDTASK_H
