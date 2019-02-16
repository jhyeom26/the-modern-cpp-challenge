#include <algorithm>
#include <future>
#include <iostream>
#include <iterator>
#include <vector>

template<typename RandomAccessIter, typename UnaryOp>
void parallelTransform(
    RandomAccessIter begin, RandomAccessIter end, UnaryOp&& unaryOp){

    auto size = std::distance(begin, end);
    if(size <= 10000){
        std::transform(begin, end, begin, std::forward<UnaryOp>(unaryOp));
    }else{
        auto threadCount = std::thread::hardware_concurrency();
        auto lenOfPieces = size / threadCount;

        auto first = begin;
        for(auto i = 0; i < threadCount - 1; i++){
            std::async(
                std::launch::async,
                [first = first, last = first + lenOfPieces, &unaryOp]{
                    std::transform(
                        first, last, first, std::forward<UnaryOp>(unaryOp));
            });
            std::advance(first, lenOfPieces);
        }
        std::transform(
            end - lenOfPieces, end,
            end - lenOfPieces, std::forward<UnaryOp>(unaryOp));
    }
}

int main(){
    std::vector<uint64_t> v1(1000000);
    for(int i = 0 ; i < v1.size(); i++){
        v1[i] = i;
    }
    auto v2(v1);

    auto func = [](int const e){return e*2;};

    std::transform(std::begin(v1), std::end(v1), std::begin(v1), func);
    parallelTransform(std::begin(v2), std::end(v2), func);

    for(int i = 0; i < v1.size(); i++){
        if(v1[i] != v2[i]){
            std::cerr << "parallel_transform has a bug on "
                      << i << std::endl
                      << v1[i] << " : " << v2[i]
                      << std::endl;
            break;
        }
    }

    return 0;
}
