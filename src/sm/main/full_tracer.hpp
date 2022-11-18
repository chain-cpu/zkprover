#ifndef FULL_TRACER_HPP
#define FULL_TRACER_HPP

#include <gmpxx.h>
#include <unordered_map>
#include "rom_command.hpp"
#include "goldilocks_base_field.hpp"
#include "config.hpp"
#include "rom_line.hpp"

// Tracer service to output the logs of a batch of transactions. A complete log is created with all the transactions embedded
// for each batch and also a log is created for each transaction separatedly. The events are triggered from the zkrom and handled
// from the zkprover

class Context;

class OpcodeContract
{
public:
    string address;
    string caller;
    mpz_class value;
    string data;
    uint64_t gas;
    OpcodeContract() : value(0), gas(0) {};
};

class Opcode
{
public:
    uint64_t remaining_gas;
    int64_t gas_cost;
    string state_root;
    uint64_t depth;
    uint64_t pc;
    uint8_t op;
    string opcode;
    uint64_t refund;
    string error;
    OpcodeContract contract;
    vector<mpz_class> stack;
    string memory;
    uint64_t memory_size;
    unordered_map<string,string> storage;
    vector<string> return_data;
    struct timeval startTime;
    uint64_t duration;
    Opcode() : remaining_gas(0), gas_cost(0), depth(0), pc(0), op(0), refund(0), memory_size(0), startTime({0,0}), duration(0) {};
};

class Log
{
public:
    string address;
    uint64_t batch_number;
    string tx_hash;
    uint64_t tx_index;
    string batch_hash;
    uint64_t index;
    vector<string> data;
    vector<string> topics;
    Log() : batch_number(0), tx_index(0), index(0) {};
};

class TxTraceContext
{
public:
    string type;
    string from;
    string to;
    string data;
    uint64_t gas;
    uint64_t gas_used;
    mpz_class value;
    string batch;
    string output;
    uint64_t nonce;
    mpz_class gasPrice;
    uint64_t chainId;
    string old_state_root;
    //string newStateRoot;
    uint64_t execution_time; // In us
    string error;
    vector<Log> logs;
    TxTraceContext() : gas(0), gas_used(0), nonce(0), chainId(0), execution_time(0) {};
};

/*class TxTrace
{
public:
    string to;
    TxTraceContext context;
    vector<Opcode> steps;
    TxTrace() : to("0x00") {};
};*/

class CallTrace
{
public:
    TxTraceContext context;
    vector<Opcode> steps;
};

class Response
{
public:
    CallTrace call_trace;
    string tx_hash;
    string rlp_tx;
    uint64_t type;
    string return_value;
    uint64_t gas_left;
    uint64_t gas_used;
    uint64_t gas_refunded;
    string error;
    string create_address;
    string state_root;
    vector<Log> logs;
    vector<Opcode> execution_trace;
    Response() : type(0), gas_left(0), gas_used(0), gas_refunded(0) {};
};

class FinalTrace
{
public:
    bool bInitialized;
    string batchHash;
    string old_state_root;
    string new_state_root;
    string new_local_exit_root;
    string globalHash;
    uint64_t numBatch;
    uint64_t timestamp;
    string sequencerAddr;
    uint64_t cumulative_gas_used;
    vector<Response> responses;
    FinalTrace() : bInitialized(false), numBatch(0), timestamp(0), cumulative_gas_used(0) {};
};

class FullTracer
{
public:
    Goldilocks &fr;
    uint64_t depth;
    uint64_t initGas;
    unordered_map<uint64_t,unordered_map<string,string>> deltaStorage;
    FinalTrace finalTrace;
    unordered_map<uint64_t,uint64_t> txGAS;
    uint64_t txCount;
    uint64_t txTime; // in us
    vector<Opcode> info; // Opcode step traces of the all the processed tx
    vector<vector<mpz_class>> fullStack;// Stack of the transaction
    uint64_t accBatchGas;
    unordered_map<uint64_t,unordered_map<uint64_t,Log>> logs;
    vector<Opcode> call_trace; // TODO: Can we remove this attribute?
    vector<Opcode> execution_trace; // TODO: Can we remove this attribute?
    string lastError;
private:
    void onError (Context &ctx, const RomCommand &cmd);
    void onStoreLog (Context &ctx, const RomCommand &cmd);
    void onProcessTx (Context &ctx, const RomCommand &cmd);
    void onUpdateStorage (Context &ctx, const RomCommand &cmd);
    void onFinishTx (Context &ctx, const RomCommand &cmd);
    void onStartBatch (Context &ctx, const RomCommand &cmd);
    void onFinishBatch (Context &ctx, const RomCommand &cmd);
    void onOpcode (Context &ctx, const RomCommand &cmd);
    void getFromMemory(Context &ctx, mpz_class &offset, mpz_class &length, string &result);
    void getVarFromCtx(Context &ctx, bool global, const char * pVarLabel, mpz_class &result);
    void getCalldataFromStack (Context &ctx, uint64_t offset, uint64_t length, string &result);
    void getRegFromCtx(Context &ctx, tReg reg, mpz_class &result);
    uint64_t findOffsetLabel (Context &ctx, const char * pLabel);
    uint64_t getCurrentTime (void);
    void getTransactionHash(string &to, mpz_class value, uint64_t nonce, uint64_t gasLimit, mpz_class gasPrice, string &data, mpz_class &r, mpz_class &s, uint64_t v, string &txHash, string &rlpTx);
public:
    FullTracer(Goldilocks &fr) : fr(fr), depth(1), initGas(0), txCount(0), txTime(0), accBatchGas(0) { };
    void handleEvent (Context &ctx, const RomCommand &cmd);
};

#endif