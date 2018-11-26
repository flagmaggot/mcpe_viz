/*
  Minecraft Pocket Edition (MCPE) World File Visualization & Reporting Tool
  (c) Plethora777, 2015.9.26

  GPL'ed code - see LICENSE

  Main classes for mcpe_viz
*/

#ifndef __MCPE_VIZ_H__
#define __MCPE_VIZ_H__

#include <map>

namespace mcpe_viz {
    
  const int32_t kColorDefault = 0xff00ff;  

  // todo ugly globals
  extern std::string dirExec;
  extern Logger logger;
  extern double playerPositionImageX, playerPositionImageY;
  extern int32_t playerPositionDimensionId;
  extern std::vector<std::string> listGeoJSON;

  extern int32_t globalIconImageId;
  
  // dimensions
  enum DimensionType : int32_t {
    kDimIdOverworld = 0,
      kDimIdNether = 1,
      kDimIdTheEnd = 2,
      // todobig - this is very brittle - consider that add-ons may one day add custome dimensions
      kDimIdCount = 3
      };

  
  void worldPointToImagePoint(int32_t dimId, double wx, double wz, double &ix, double &iy, bool geoJsonFlag);
  void worldPointToGeoJSONPoint(int32_t dimId, double wx, double wz, double &ix, double &iy);

  class BlockInfo {
  public:
    int32_t id;
    std::string name;
    std::vector<std::string> unameList;
    int32_t color;
    bool colorSetFlag;
    bool solidFlag;
    bool opaqueFlag;
    bool liquidFlag;
    bool spawnableFlag;
    int32_t colorSetNeedCount;
    int32_t blockdata;
    std::vector< std::unique_ptr<BlockInfo> > variantList;
    bool valid;

    int32_t userVar1;
    std::string userString1;
    
    BlockInfo() {
      id = -1;
      name = "(unknown)";
      unameList.clear();
      setColor(kColorDefault); // purple
      solidFlag = true;
      opaqueFlag = true;
      liquidFlag = false;
      spawnableFlag = true;
      colorSetFlag = false;
      colorSetNeedCount = 0;
      variantList.clear();
      valid = false;
      userVar1 = 0;
      userString1 = "";
    }

    BlockInfo& setId(int32_t i) {
      id = i;
      return *this;
    }

    BlockInfo& setName(const std::string& s) {
      name = std::string(s);
      valid = true;
      return *this;
    }

    BlockInfo& setUname(const std::string& s) {
      unameList = mysplit(s,';');
      // todonow - update a dictionary for unames?
      return *this;
    }

    bool isValid() { return valid; }

    void setUserVar1(int32_t v) { userVar1 = v; }
    void deltaUserVar1(int32_t d) { userVar1 += d; }
    int32_t getUserVar1() { return userVar1; }

    void setUserString1(const std::string& s) { userString1 = s; }
    std::string& getUserString1() { return userString1; }
    
    BlockInfo& setColor(int32_t rgb) {
      // note: we convert color storage to big endian so that we can memcpy when creating images
      color = htobe32(rgb);
      colorSetFlag = true;
      return *this;
    }

    BlockInfo& setSolidFlag(bool f) {
      solidFlag = f;
      return *this;
    }
    bool isSolid() { return solidFlag; }

    BlockInfo& setOpaqueFlag(bool f) {
      opaqueFlag = f;
      return *this;
    }
    bool isOpaque() { return opaqueFlag; }

    BlockInfo& setLiquidFlag(bool f) {
      liquidFlag = f;
      return *this;
    }
    bool isLiquid() { return liquidFlag; }

    BlockInfo& setSpawnableFlag(bool f) {
      spawnableFlag = f;
      return *this;
    }
    bool isSpawnable(int32_t bd) {
      if (hasVariants()) {
        for (const auto& itbv : variantList) {
          if ( itbv->blockdata == bd ) {
            return itbv->spawnableFlag;
          }
        }
        fprintf(stderr, "WARNING: did not find bd=%d (0x%x) for block='%s'\n", bd, bd, name.c_str());
      }
      return spawnableFlag;
    }

    bool hasVariants() {
      return (variantList.size() > 0);
    }
    
    void setBlockData(int32_t bd) {
      blockdata = bd;
    }

    BlockInfo& addVariant(int32_t bd, const std::string& n) {
      std::unique_ptr<BlockInfo> bv(new BlockInfo());
      bv->setName(n);
      bv->setBlockData(bd);
      variantList.push_back( std::move(bv) );
      return *(variantList.back());
    }

    std::string toString() {
      char tmpstring[1024];
      sprintf(tmpstring,"Block: name=%s color=0x%06x solid=%d opaque=%d liquid=%d spawnable=%d"
              , name.c_str()
              , color
              , (int)solidFlag
              , (int)opaqueFlag
              , (int)liquidFlag
              , (int)spawnableFlag
              );
      // todo variants?
      return std::string(tmpstring);
    }
  };
  
  extern BlockInfo blockInfoList[512];

  //BlockInfo* getBlockInfo(int32_t id, int32_t blockData);
  std::string getBlockName(int32_t id, int32_t blockdata);

  int32_t getBlockByUname(const std::string& uname, int32_t& blockId, int32_t& blockData);
  

  class ItemInfo {
  public:
    std::string name;
    int32_t extraData;
    std::vector< std::unique_ptr<ItemInfo> > variantList;
    int32_t userVar1;
    std::string userString1;
    
    ItemInfo(const char* n) {
      setName(n);
      extraData = 0;
      variantList.clear();
      userVar1 = 0;
      userString1 = "";
    }

    ItemInfo& setName (const std::string& s) {
      name = std::string(s);
      return *this;
    }

    void setUserVar1(int32_t v) { userVar1 = v; }
    void deltaUserVar1(int32_t d) { userVar1 += d; }
    int32_t getUserVar1() { return userVar1; }
    
    void setUserString1(const std::string& s) { userString1 = s; }
    std::string& getUserString1() { return userString1; }

    bool hasVariants() {
      return (variantList.size() > 0);
    }

    void setExtraData(int32_t ed) {
      extraData = ed;
    }
    
    ItemInfo& addVariant(int32_t ed, const std::string& n) {
      std::unique_ptr<ItemInfo> iv(new ItemInfo(n.c_str()));
      iv->setExtraData(ed);
      variantList.push_back( std::move(iv) );
      return *(variantList.back());
    }
  };

  typedef std::map<int, std::unique_ptr<ItemInfo> > ItemInfoList;
  extern ItemInfoList itemInfoList;
  bool has_key(const ItemInfoList &m, int32_t k);

  std::string getItemName(int32_t id, int32_t extraData);
  
  
  class EntityInfo {
  public:
    std::string idString;
    std::string name;
    std::string etype;
    
    EntityInfo(const std::string& n, const std::string& is, const std::string& e ) {
      setName(n);
      setIdString(is);
      setEtype(e);
    }

    EntityInfo& setName (const std::string& s) {
      name = std::string(s);
      return *this;
    }

    EntityInfo& setIdString (const std::string& s) {
      idString = std::string(s);
      return *this;
    }

    EntityInfo& setEtype (const std::string& e) {
      etype = std::string(e);
      return *this;
    }
  };

  typedef std::map<int, std::unique_ptr<EntityInfo> > EntityInfoList;
  extern EntityInfoList entityInfoList;
  bool has_key(const EntityInfoList &m, int32_t k);
  int32_t findIdString(const EntityInfoList &m, std::string& ids);
  int32_t findIdByIdentifier(const EntityInfoList &m, std::string& identifier);

  class BiomeInfo {
  public:
    std::string name;
    int32_t color;
    bool colorSetFlag;

    BiomeInfo(const char* n) {
      setName(n);
      setColor(kColorDefault);
      colorSetFlag = false;
    }

    BiomeInfo(const char* n, int32_t rgb) {
      setName(n);
      setColor(rgb);
    }

    BiomeInfo& setName (const std::string& s) {
      name = std::string(s);
      return *this;
    }

    BiomeInfo& setColor(int32_t rgb) {
      // note: we convert color storage to big endian so that we can memcpy when creating images
      color = htobe32(rgb);
      colorSetFlag=true;
      return *this;
    }
  };

  typedef std::map<int, std::unique_ptr<BiomeInfo> > BiomeInfoList;
  extern BiomeInfoList biomeInfoList;
  bool has_key(const BiomeInfoList &m, int32_t k);



  class EnchantmentInfo {
  public:
    std::string name;
    std::string officialName;

    EnchantmentInfo(const char* n) {
      setName(n);
      officialName="";
    }

    EnchantmentInfo& setName (const std::string& s) {
      name = std::string(s);
      return *this;
    }

    EnchantmentInfo& setOfficialName (const std::string& s) {
      officialName = std::string(s);
      return *this;
    }
  };

  typedef std::map<int, std::unique_ptr<EnchantmentInfo> > EnchantmentInfoList;
  extern EnchantmentInfoList enchantmentInfoList;
  bool has_key(const EnchantmentInfoList &m, int32_t k);



  typedef std::map<int32_t, int32_t> IntIntMap;

  extern IntIntMap mcpcToMcpeBlock;
  extern IntIntMap mcpeToMcpcBlock;
  extern IntIntMap mcpcToMcpeItem;
  extern IntIntMap mcpeToMcpcItem;

  bool has_key(const IntIntMap &m, int32_t k);



  typedef std::map<std::string, int32_t> StringIntMap;

  extern StringIntMap imageFileMap;

  bool has_key(const StringIntMap &m, const std::string& k);
  
} // namespace mcpe_viz

#endif // __MCPE_VIZ_H__
