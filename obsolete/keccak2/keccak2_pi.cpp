#include "keccak2_state.hpp"

/*
Keccak-f Pi permutation.
Steps:
1. For all triples (x, y, z) such that 0 ≤ x <5, 0 ≤ y < 5, and 0 ≤ z < w, let
A′[x, y, z]= A[(x + 3y) mod 5, x, z].
2. Return A′.
*/

void Keccak2Pi (Keccak2State &Sin, Keccak2State &Sout)
{
    Sout.copyCounters(Sin);

    // A′[x, y, z]= A[(x + 3y) mod 5, x, z]
    for (uint64_t x=0; x<5; x++)
    {
        for (uint64_t y=0; y<5; y++)
        {
            for (uint64_t z=0; z<64; z++)
            {
                Sout.setBit(x, y, z, Sin.getBit((x+3*y)%5, x, z));
                Sout.adds++;
                Sout.mults++;
                Sout.mods++;
            }
        }
    } 
}