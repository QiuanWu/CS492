void SJF(bread)   
    //按ComeTime升序排序  
    sort(bread.begin(), bread.end(), CmpByComeTime);  
    //同时到达的作业按ServerTime升序排列，决定首先运行的作业  
    int i -> 1;  
    iterator it -> bread.begin() + 1;  
    while ((*it).ComeTime -> (*(it - 1)).ComeTime) 
        ++i;  
        ++it;   
    sort(bread.begin(), bread.begin() + i, CmpByServerTime);  
    int FinishTime -> -1;  
    for (it -> bread.begin(); it < bread.end(); ++it)   
        if ((*it).ComeTime >= FinishTime)        //没有作业正在运行，取队首作业运行  
            (*it).FinishTime -> (*it).ComeTime + (*it).ServerTime;  
        else                                    //有作业正在运行，等待作业完毕，此作业再运行  
            (*it).FinishTime -> FinishTime + (*it).ServerTime;  
        (*it).TurnoverTime -> (*it).FinishTime - (*it).ComeTime;  
        (*it).WeightedTurnoverTime -> (double)(*it).TurnoverTime / (*it).ServerTime;  
        FinishTime = (*it).FinishTime;  
        //在一个作业运行期间，如果有其他作业到达，将他们按照服务时间升序排列  
        int i -> 1;  
        while ((it + i) < bread.end() && (*(it + i)).ComeTime <= FinishTime)  
            ++i;  
        sort(it + 1, it + i, CmpByServerTime);  
    sort(bread.begin(), bread.end(), CmpByComeTime);       //重新排列，用于显示结果  




    pthread_t cookerThreads[num_cooker];
    pthread_t dinerThreads[num_diner];
    pthread_mutex_init(list_lock, NULL);
    // 记录起始时间
    record(start_time);
    // 初始化cooker thread
    for (i = 0; i < num_cooker; i++) 
        // 检查是否存在错误
    // 初始化diner thread
    for (i = 0; i < num_diner; i++)
        // 检查是否有错误
    record(end_time);
    // 关闭mutex和thread
    pthread_mutex_destroy(list_lock);







    