#ifndef MAIN_ZKEVM__VERIFIER_MOCK_C12B_HPP
#define MAIN_ZKEVM__VERIFIER_MOCK_C12B_HPP

#include <string>
#include <nlohmann/json.hpp>
#include "starkpil/test/zkevm_c12b_verifier_cpp/calcwit.c12b.hpp"
#include "starkpil/test/zkevm_c12b_verifier_cpp/circom.c12b.hpp"
#include "fr.hpp"

namespace MockCircomC12b
{
    using json = nlohmann::json;

    Circom_Circuit *loadCircuit(std::string const &datFileName);
    void freeCircuit(Circom_Circuit *circuit);
    void loadJson(Circom_CalcWit *ctx, std::string filename);
    void loadJsonImpl(Circom_CalcWit *ctx, json &j);
    void writeBinWitness(Circom_CalcWit *ctx, std::string wtnsFileName);
    void getBinWitness(Circom_CalcWit *ctx, RawFr::Element *&pWitness, uint64_t &witnessSize);
}
#endif