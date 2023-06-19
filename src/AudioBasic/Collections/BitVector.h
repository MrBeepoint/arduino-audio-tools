#pragma once
#include <stdint.h>
#include "Vector.h"

namespace audio_tools {

/**
 * @brief Space optimized vector which stores the boolean values as bits
 * @ingroup collections
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class BitVector {
  public:
    BitVector() = default;

    inline bool operator[](size_t index) {
        return get(index);
    }

    bool get(int64_t index){
        if (index<0) return false;
        bool result = false;
        int offset = index / sizeof(uint64_t);
        int bit = index % sizeof(uint64_t);
        if (offset < vector.size()){
            uint64_t value = vector[offset];
            // get bit
            result = (value >> bit) & 1U;
        }
        return result;
    }

    void set(int64_t index, bool value){
        if (index<0) return;
        max_idx = max(max_idx, index+1);
        int offset = index / sizeof(uint64_t);
        int bit = index % sizeof(uint64_t);
        while(offset>=vector.size()){
            vector.push_back(0l);
        }
        // check if we need to updte the value
        if (((vector[offset] >> bit) & 1U) != value) {
            // needs update
            if (value) {
                // set bit
                vector[offset] |= 1UL << bit;
            } else {
                // clear bit
                vector[offset] &= ~(1UL << bit);
            }
            // call change handler to notify about change
            if (changeHandler!=nullptr){
                changeHandler(index, value, ref);
            }
        }
    }

    void clear() {
        max_idx = 0;
        vector.clear();
        vector.shrink_to_fit();
    }

    int size() {
        return max_idx;
    }

    /// Defines a callback method which is called when a value is updated
    void setChangeCallback(void (*hdler)(int idx, bool value, void*ref), void*ref=nullptr){
        changeHandler = hdler;
        this->ref = ref;
    }

    void resize(int size){
        max_idx = size;
        int round_up = size%64 != 0;
        vector.resize(size/64 + round_up);
    }

    // shifts n bits: + to the left; - to the right
    void shift(int n){
        if (n==0) return;
        for (int j=0; j < max_idx -n; j++){
            set(j, get(j+n));
        }
    }

    int8_t toInt8(int n){
        int8_t result;
        for (int j=0;j<8;j++){
            bool x = get(n);
            result ^= (-x ^ result) & (1UL << j);

        }
        return result;
    }

    int16_t toInt16(int n){
        int16_t result;
        for (int j=0;j<16;j++){
            bool x = get(n);
            result ^= (-x ^ result) & (1UL << j);

        }
        return result;
    }

    template <typename T>
    T toInt(int n){
        T result;
        for (int j=0;j<sizeof(T)*8;j++){
            bool x = get(n);
            // set bit at pos
            result ^= (-x ^ result) & (1UL << j);

        }
        return result;
    }


  protected:
    Vector<uint64_t> vector;
    void (*changeHandler)(int idx, bool value, void*ref) = nullptr;
    void *ref;
    int64_t max_idx = 0;

};

}