#include "threadpool.h"
#include <unistd.h>




void print(shared_ptr<void> num)
{

    shared_ptr<int> n = static_pointer_cast<int>(num);
    cout << *n << endl;

}


int main()
{

    MythreadPool pool;
    pool.warm_up();
    for(int i = 0; i < 10;i++)
    {
        pool.add(shared_ptr<int>(new int(i)),print);
    }
    sleep(1);
    pool.close();
    return 0;
}

