// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Copyright (c) 2017 Tom Zander <tomz@freedommail.ch>
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_BLOCKSDB_H
#define BITCOIN_BLOCKSDB_H

#include "dbwrapper.h"

#include <boost/unordered_map.hpp>
#include <string>
#include <vector>

class CBlockFileInfo;
class CBlockIndex;
struct CDiskTxPos;
class CDiskBlockPos;
class CChainParams;
class uint256;

//! -dbcache default (MiB)
static const int64_t nDefaultDbCache = 300;
//! max. -dbcache in (MiB)
static const int64_t nMaxDbCache = sizeof(void*) > 4 ? 16384 : 1024;
//! min. -dbcache in (MiB)
static const int64_t nMinDbCache = 4;

/** Access to the block database (blocks/index/) */
class BlocksDB : public CDBWrapper
{
public:
    /**
     * returns the singleton instance of the BlocksDB. Please be aware that
     *     this will return nullptr until you call createInstance() or createTestInstance();
     */
    static BlocksDB *instance();
    /**
     * Deletes an old and creates a new instance of the BlocksDB singleton.
     * @param[in] nCacheSize  Configures various leveldb cache settings.
     * @param[in] fWipe       If true, remove all existing data.
     * @see instance()
     */
    static void createInstance(size_t nCacheSize, bool fWipe);
    /// Deletes old singleton and creates a new one for unit testing.
    static void createTestInstance(size_t nCacheSize);

private:
    BlocksDB(size_t nCacheSize, bool fMemory = false, bool fWipe = false);
    BlocksDB(const BlocksDB&);
    void operator=(const BlocksDB&);
public:
    bool WriteBatchSync(const std::vector<std::pair<int, const CBlockFileInfo*> >& fileInfo, int nLastFile, const std::vector<const CBlockIndex*>& blockinfo);
    bool ReadBlockFileInfo(int nFile, CBlockFileInfo &fileinfo);
    bool ReadLastBlockFile(int &nFile);
    bool WriteReindexing(bool fReindex);
    bool ReadReindexing(bool &fReindex);
    bool ReadTxIndex(const uint256 &txid, CDiskTxPos &pos);
    bool WriteTxIndex(const std::vector<std::pair<uint256, CDiskTxPos> > &list);
    bool WriteFlag(const std::string &name, bool fValue);
    bool ReadFlag(const std::string &name, bool &fValue);
    /// Reads and caches all info about blocks.
    bool CacheAllBlockInfos();

private:
    static BlocksDB *s_instance;
};

struct BlockHashShortener {
    inline size_t operator()(const uint256& hash) const {
        return hash.GetCheapHash();
    }
};
typedef boost::unordered_map<uint256, CBlockIndex*, BlockHashShortener> BlockMap;
// TODO move this into BlocksDB and protect it with a mutex
extern BlockMap mapBlockIndex;


/** Open a block file (blk?????.dat) */
FILE* OpenBlockFile(const CDiskBlockPos &pos, bool fReadOnly = false);
/** Open an undo file (rev?????.dat) */
FILE* OpenUndoFile(const CDiskBlockPos &pos, bool fReadOnly = false);
/** Translation to a filesystem path */
boost::filesystem::path GetBlockPosFilename(int fileIndex, const char *prefix);
/** Import blocks from an external file */
bool LoadExternalBlockFile(const CChainParams& chainparams, FILE* fileIn, CDiskBlockPos *dbp = 0);


#endif // BITCOIN_TXDB_H
