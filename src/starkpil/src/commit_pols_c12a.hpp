#ifndef COMMIT_POLS_C12_A_HPP
#define COMMIT_POLS_C12_A_HPP

#include <cstdint>
#include "goldilocks_base_field.hpp"
#include "zkassert.hpp"

class CommitPolC12a
{
private:
    Goldilocks::Element * _pAddress;
    uint64_t _degree;
    uint64_t _index;
public:
    CommitPolC12a(Goldilocks::Element * pAddress, uint64_t degree, uint64_t index) : _pAddress(pAddress), _degree(degree), _index(index) {};
    Goldilocks::Element & operator[](uint64_t i) { return _pAddress[i*12]; };
    Goldilocks::Element * operator=(Goldilocks::Element * pAddress) { _pAddress = pAddress; return _pAddress; };

    Goldilocks::Element * address (void) { return _pAddress; }
    uint64_t degree (void) { return _degree; }
    uint64_t index (void) { return _index; }
};

class CompressorCommitPolsC12a
{
public:
    CommitPolC12a a[12];
private:
    void * _pAddress;
    uint64_t _degree;
public:

    CompressorCommitPolsC12a (void * pAddress, uint64_t degree) :
        a{
            CommitPolC12a((Goldilocks::Element *)((uint8_t *)pAddress + 0), degree, 0),
            CommitPolC12a((Goldilocks::Element *)((uint8_t *)pAddress + 8), degree, 1),
            CommitPolC12a((Goldilocks::Element *)((uint8_t *)pAddress + 16), degree, 2),
            CommitPolC12a((Goldilocks::Element *)((uint8_t *)pAddress + 24), degree, 3),
            CommitPolC12a((Goldilocks::Element *)((uint8_t *)pAddress + 32), degree, 4),
            CommitPolC12a((Goldilocks::Element *)((uint8_t *)pAddress + 40), degree, 5),
            CommitPolC12a((Goldilocks::Element *)((uint8_t *)pAddress + 48), degree, 6),
            CommitPolC12a((Goldilocks::Element *)((uint8_t *)pAddress + 56), degree, 7),
            CommitPolC12a((Goldilocks::Element *)((uint8_t *)pAddress + 64), degree, 8),
            CommitPolC12a((Goldilocks::Element *)((uint8_t *)pAddress + 72), degree, 9),
            CommitPolC12a((Goldilocks::Element *)((uint8_t *)pAddress + 80), degree, 10),
            CommitPolC12a((Goldilocks::Element *)((uint8_t *)pAddress + 88), degree, 11)
        },
        _pAddress(pAddress),
        _degree(degree) {};

    static uint64_t pilDegree (void) { return 16777216; }
    static uint64_t pilSize (void) { return 96; }
    static uint64_t numPols (void) { return 12; }

    void * address (void) { return _pAddress; }
    uint64_t degree (void) { return _degree; }
    uint64_t size (void) { return _degree*12*sizeof(Goldilocks::Element); }
};

class CommitPolsC12a
{
public:
    CompressorCommitPolsC12a Compressor;
private:
    void * _pAddress;
    uint64_t _degree;
public:

    CommitPolsC12a (void * pAddress, uint64_t degree) :
        Compressor(pAddress, degree),
        _pAddress(pAddress),
        _degree(degree) {}

    static uint64_t pilSize (void) { return 1610612736; }
    static uint64_t pilDegree (void) { return 16777216; }
    static uint64_t numPols (void) { return 12; }

    void * address (void) { return _pAddress; }
    uint64_t degree (void) { return _degree; }
    uint64_t size (void) { return _degree*12*sizeof(Goldilocks::Element); }

    Goldilocks::Element &getElement (uint64_t pol, uint64_t evaluation)
    {
        zkassert((pol < numPols()) && (evaluation < degree()));
        return ((Goldilocks::Element *)_pAddress)[pol + evaluation * numPols()];
    }
};

#endif // COMMIT_POLS_HPP
