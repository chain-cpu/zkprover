#include "keccak2_state.hpp"

/*
Input: integer t
Output: bit rc(t)
Steps:
1. If t mod 255 = 0, return 1
2. Let R = 10000000b
3. For i from 1 to t mod 255
    a. R = 0 || R
    b. R[0] = R[0] ⊕ R[8]
    c. R[4] = R[4] ⊕ R[8]
    d. R[5] = R[5] ⊕ R[8]
    e. R[6] = R[6] ⊕ R[8]
    f. R =Trunc8[R]
4. Return R[0]
*/

uint8_t rc (uint64_t t, Keccak2State &Sout)
{
    uint64_t tmod255 = t%255;
    Sout.mods++;
    // If t mod 255 = 0, return 1
    if (tmod255 == 0) return 1;

    // Let R = 10000000b
    uint8_t R[9];
    memset(R, 0, sizeof(R));
    R[0] = 1;

    // For i from 1 to t mod 255
    for (uint64_t i=0; i<tmod255; i++)
    {
        // R = 0 || R
        for (uint64_t j=8; j>0; j--)
        {
            R[j] = R[j-1];
            Sout.adds++;
        }
        R[0] = 0;

        // R[0] = R[0] ⊕ R[8]
        R[0] = R[0] ^ R[8];

        // R[4] = R[4] ⊕ R[8]
        R[4] = R[4] ^ R[8];

        // R[5] = R[5] ⊕ R[8]
        R[5] = R[5] ^ R[8];

        // R[6] = R[6] ⊕ R[8]
        R[6] = R[6] ^ R[8];

        Sout.xors += 4;

        // R =Trunc8[R]
        R[8] = 0;
    }

    Sout.ands++;
    return R[0] & 0x01;
}

/*
Keccak-f Iota permutation.
Steps:
1. For all triples (x, y, z) such that 0 ≤ x <5, 0 ≤ y < 5, and 0 ≤ z < w, let A′[x, y, z] = A[x, y, z]
2. Let RC = 0w.
3. For j from 0 to l, let RC[2^j – 1] = rc(j + 7ir).
4. For all z such that 0 ≤ z <w, let A′ [0, 0, z] = A′[0, 0, z] ⊕ RC[z].
5. Return A′
*/

void Keccak2Iota(Keccak2State &Sin, Keccak2State &Sout, uint64_t ir)
{
    Sout.copyCounters(Sin);

    // A′[x, y, z] = A[x, y, z]
    for (uint64_t x=0; x<5; x++)
    {
        for (uint64_t y=0; y<5; y++)
        {
            for (uint64_t z=0; z<64; z++)
            {
                Sout.setBit(x, y, z, Sin.getBit(x, y, z));
            }
        }
    }

    // Let RC = 0w
    uint8_t RC[64];
    memset(RC, 0, 64);

    // For j from 0 to l, let RC[2^j – 1] = rc(j + 7ir)
    for (uint64_t j=0; j<=6; j++)
    {
        RC[(1<<j) - 1] = rc(j + (7*ir), Sout);
        Sout.adds += 2;
        Sout.shifts++;
        Sout.mults++;
    }

    // For all z such that 0 ≤ z <w, let A′ [0, 0, z] = A′[0, 0, z] ⊕ RC[z]
    for (uint64_t z=0; z<64; z++)
    {
        Sout.setBit(0, 0, z, Sout.getBit(0, 0, z)^RC[z]);
        Sout.xors++;
    }
}