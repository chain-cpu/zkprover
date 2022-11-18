#include "zkresult.hpp"

struct {
    int code;
    const char* message;
} resultdesc[] = {
    { ZKR_UNSPECIFIED, "Unspecified error" },
    { ZKR_SUCCESS, "Success" },
    { ZKR_DB_KEY_NOT_FOUND, "Key not found in the database" },
    { ZKR_DB_ERROR, "Database error" },
    { ZKR_INTERNAL_ERROR, "Internal error" },
    { ZKR_SM_MAIN_ASSERT, "Main state machine executor assert failed" },
    { ZKR_SM_MAIN_ARITH, "Main state machine executor arith condition failed" },
    { ZKR_SM_MAIN_BINARY, "Main state machine executor binary condition failed" },
    { ZKR_SM_MAIN_HASHP, "Main state machine executor hash poseidon condition failed" },
    { ZKR_SM_MAIN_HASHK, "Main state machine executor hash Keccak condition failed" },
    { ZKR_SM_MAIN_STORAGE, "Main state machine executor storage condition failed" },
    { ZKR_SM_MAIN_MEMORY, "Main state machine executor memory condition failed" },
    { ZKR_SM_MAIN_MEMALIGN, "Main state machine executor memalign condition failed" },
    { ZKR_SM_MAIN_ADDRESS, "Main state machine executor address condition failed" },
    { ZKR_SMT_INVALID_DATA_SIZE, "Invalid size data for a MT node"},
    { ZKR_SM_MAIN_BATCH_L2_DATA_TOO_BIG, "Input batch L2 data is too big"}
};

const char* zkresult2string (int code)
{
    for (int i = 0; resultdesc[i].message; i++)
        if (resultdesc[i].code == code)
            return resultdesc[i].message;
    return "unknown";
}