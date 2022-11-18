#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <chrono>
#include "starkpil/test/zkevm_c12a_verifier_cpp/main.c12a.hpp"
#include "starkpil/test/zkevm_c12a_verifier_cpp/calcwit.c12a.hpp"
#include "starkpil/test/zkevm_c12a_verifier_cpp/circom.c12a.hpp"

using json = nlohmann::json;

namespace MockCircomC12a
{
#define handle_error(msg) \
  do                      \
  {                       \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

  Circom_Circuit *loadCircuit(std::string const &datFileName)
  {
    Circom_Circuit *circuit = new Circom_Circuit;

    int fd;
    struct stat sb;

    fd = open(datFileName.c_str(), O_RDONLY);
    if (fd == -1)
    {
      std::cout << ".dat file not found: " << datFileName << "\n";
      throw std::system_error(errno, std::generic_category(), "open");
    }

    if (fstat(fd, &sb) == -1)
    { /* To obtain file size */
      throw std::system_error(errno, std::generic_category(), "fstat");
    }

    u8 *bdata = (u8 *)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    circuit->InputHashMap = new HashSignalInfo[get_size_of_input_hashmap()];
    uint dsize = get_size_of_input_hashmap() * sizeof(HashSignalInfo);
    memcpy((void *)(circuit->InputHashMap), (void *)bdata, dsize);

    circuit->witness2SignalList = new u64[get_size_of_witness()];
    uint inisize = dsize;
    dsize = get_size_of_witness() * sizeof(u64);
    memcpy((void *)(circuit->witness2SignalList), (void *)(bdata + inisize), dsize);

    circuit->circuitConstants = new FrGElement[get_size_of_constants()];
    if (get_size_of_constants() > 0)
    {
      inisize += dsize;
      dsize = get_size_of_constants() * sizeof(FrGElement);
      memcpy((void *)(circuit->circuitConstants), (void *)(bdata + inisize), dsize);
    }

    std::map<u32, IODefPair> templateInsId2IOSignalInfo1;
    if (get_size_of_io_map() > 0)
    {
      u32 index[get_size_of_io_map()];
      inisize += dsize;
      dsize = get_size_of_io_map() * sizeof(u32);
      memcpy((void *)index, (void *)(bdata + inisize), dsize);
      inisize += dsize;
      assert(inisize % sizeof(u32) == 0);
      assert(sb.st_size % sizeof(u32) == 0);
      u32 dataiomap[(sb.st_size - inisize) / sizeof(u32)];
      memcpy((void *)dataiomap, (void *)(bdata + inisize), sb.st_size - inisize);
      u32 *pu32 = dataiomap;

      for (uint i = 0; i < get_size_of_io_map(); i++)
      {
        u32 n = *pu32;
        IODefPair p;
        p.len = n;
        IODef defs[n];
        pu32 += 1;
        for (u32 j = 0; j < n; j++)
        {
          defs[j].offset = *pu32;
          u32 len = *(pu32 + 1);
          defs[j].len = len;
          defs[j].lengths = new u32[len];
          memcpy((void *)defs[j].lengths, (void *)(pu32 + 2), len * sizeof(u32));
          pu32 += len + 2;
        }
        p.defs = (IODef *)calloc(10, sizeof(IODef));
        for (u32 j = 0; j < p.len; j++)
        {
          p.defs[j] = defs[j];
        }
        templateInsId2IOSignalInfo1[index[i]] = p;
      }
    }
    circuit->templateInsId2IOSignalInfo = move(templateInsId2IOSignalInfo1);

    munmap(bdata, sb.st_size);

    return circuit;
  }

  void freeCircuit(Circom_Circuit *circuit)
  {
    delete[] circuit->InputHashMap;
    delete[] circuit->witness2SignalList;
    delete[] circuit->circuitConstants;
    delete circuit;
  }

  void json2FrGElements(json val, std::vector<FrGElement> &vval)
  {
    if (!val.is_array())
    {
      FrGElement v;
      std::string s;
      if (val.is_string())
      {
        s = val.get<std::string>();
      }
      else if (val.is_number())
      {
        double vd = val.get<double>();
        std::stringstream stream;
        stream << std::fixed << std::setprecision(0) << vd;
        s = stream.str();
      }
      else
      {
        throw new std::runtime_error("Invalid JSON type");
      }
      FrG_str2element(&v, s.c_str());
      vval.push_back(v);
    }
    else
    {
      for (uint i = 0; i < val.size(); i++)
      {
        json2FrGElements(val[i], vval);
      }
    }
  }

  void loadJsonImpl(Circom_CalcWit *ctx, json &j)
  {
    /*
      std::ifstream inStream(filename);
      json j;
      inStream >> j;

      u64 nItems = j.size();
      // printf("Items : %llu\n",nItems);
      if (nItems == 0){
        ctx->tryRunCircuit();
      }
    */
    for (json::iterator it = j.begin(); it != j.end(); ++it)
    {
      // std::cout << it.key() << " => " << it.value() << '\n';
      u64 h = fnv1a(it.key());
      std::vector<FrGElement> v;
      json2FrGElements(it.value(), v);
      uint signalSize = ctx->getInputSignalSize(h);
      if (v.size() < signalSize)
      {
        std::ostringstream errStrStream;
        errStrStream << "Error loading signal " << it.key() << ": Not enough values\n";
        throw std::runtime_error(errStrStream.str());
      }
      if (v.size() > signalSize)
      {
        std::ostringstream errStrStream;
        errStrStream << "Error loading signal " << it.key() << ": Too many values\n";
        throw std::runtime_error(errStrStream.str());
      }
      for (uint i = 0; i < v.size(); i++)
      {
        try
        {
          // std::cout << it.key() << "," << i << " => " << FrG_element2str(&(v[i])) << '\n';
          ctx->setInputSignal(h, i, v[i]);
        }
        catch (std::runtime_error &e)
        {
          std::ostringstream errStrStream;
          errStrStream << "Error setting signal: " << it.key() << "\n"
                       << e.what();
          throw std::runtime_error(errStrStream.str());
        }
      }
    }
  }

  void writeBinWitness(Circom_CalcWit *ctx, std::string wtnsFileName)
  {
    FILE *write_ptr;

    write_ptr = fopen(wtnsFileName.c_str(), "wb");

    fwrite("wtns", 4, 1, write_ptr);

    u32 version = 2;
    fwrite(&version, 4, 1, write_ptr);

    u32 nSections = 2;
    fwrite(&nSections, 4, 1, write_ptr);

    // Header
    u32 idSection1 = 1;
    fwrite(&idSection1, 4, 1, write_ptr);

    u32 n8 = FrG_N64 * 8;

    u64 idSection1length = 8 + n8;
    fwrite(&idSection1length, 8, 1, write_ptr);

    fwrite(&n8, 4, 1, write_ptr);

    fwrite(FrG_q.longVal, FrG_N64 * 8, 1, write_ptr);

    uint Nwtns = get_size_of_witness();

    u32 nVars = (u32)Nwtns;
    fwrite(&nVars, 4, 1, write_ptr);

    // Data
    u32 idSection2 = 2;
    fwrite(&idSection2, 4, 1, write_ptr);

    u64 idSection2length = (u64)n8 * (u64)Nwtns;
    fwrite(&idSection2length, 8, 1, write_ptr);

    FrGElement v;

    for (uint i = 0; i < Nwtns; i++)
    {
      ctx->getWitness(i, &v);
      FrG_toLongNormal(&v, &v);
      fwrite(v.longVal, FrG_N64 * 8, 1, write_ptr);
    }
    fclose(write_ptr);
  }

  void getBinWitness(Circom_CalcWit *ctx, RawFr::Element *&pWitness, uint64_t &witnessSize)
  {
    
    witnessSize = get_size_of_witness();

    pWitness = (RawFr::Element *)malloc(witnessSize * sizeof(FrGElement));
    if (pWitness == NULL)
    {
      std::cout << "Error: getBinWitness() failed allocating memory size: " << witnessSize << std::endl;
      exit(-1);
    }

    FrGElement v;

    for (uint i = 0; i < witnessSize; i++)
    {
      ctx->getWitness(i, &v);
      FrG_toLongNormal(&v, &v);
      memcpy(pWitness + i, &v.longVal, sizeof(v.longVal));
    }
  }

  void loadJson(Circom_CalcWit *ctx, std::string filename)
  {
    std::ifstream inStream(filename);
    json j;
    inStream >> j;
    inStream.close();
    loadJsonImpl(ctx, j);
  }
}