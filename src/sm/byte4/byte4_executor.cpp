#include <iostream>
#include "byte4_executor.hpp"
#include "utils.hpp"

using namespace std;

void Byte4Executor::execute (map<uint32_t, bool> &input, Byte4CommitPols & pols)
{
    // Based on the content of byte4[], fill the byte4_freeIn and byte4_out polynomials
    uint64_t p = 0;
    uint64_t last = 0;

    // Check that we have enough room in polynomials
    if (input.size()*2 > N)
    {
        cerr << "Error: Byte4Executor::execute() Too many byte4 entries=" << input.size() << " > N/2=" << N/2 << endl;
        exitProcess();
    }
    
    // Generate polynomials content out of byte4 content
    for (map<uint32_t,bool>::iterator it=input.begin(); it!=input.end(); it++)
    {
        uint32_t num = it->first;
        pols.freeIN[p] = fr.fromU64( num >> 16 );
        pols.out[p] = fr.fromU64( last );
        p++;
        pols.freeIN[p] = fr.fromU64( num & 0xFFFF );
        pols.out[p] = fr.fromU64( num >> 16 );
        p++;
        last = num;
    }
    //pols.freeIN[p] = fr.zero(); // Committed pols memory is zero by default
    pols.out[p] = fr.fromU64(last);
    //p++;
    //pols.freeIN[p] = fr.zero(); // Committed pols memory is zero by default
    //pols.out[p] = fr.zero(); // Committed pols memory is zero by default
    //p++;

    cout << "Byte4Executor successfully processed " << input.size() << " Byte4 actions (" << (double(input.size())*2*100)/N << "%)" << endl;
}