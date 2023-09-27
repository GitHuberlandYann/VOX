#ifndef BIOMES_HPP
# define BIOMES_HPP

enum biomes {
    VOID,
    PLAINS,
    SUNFLOWER_PLAINS,
    SNOWY_PLAINS,
    ICE_SPIKES,
    DESERT,
    SWAMP,
    MANGROVE_SWAMP,
    FOREST,
    FLOWER_FOREST,
    BIRCH_FOREST,
    DARK_FOREST,
    OLD_GROWTH_BIRCH_FOREST,
    OLD_GROWTH_PINE_TAIGA,
    OLD_GROWTH_SPRUCE_TAIGA,
    TAIGA,
    SNOWY_TAIGA,
    SAVANNA,
    SAVANNA_PLATEAU,
    WINDSWEPT_HILLS,
    WINDSWEPT_GRAVELLY_HILLS,
    WINDSWEPT_FOREST,
    WINDSWEPT_SAVANNA,
    JUNGLE,
    SPARSE_JUNGLE,
    BAMBOO_JUNGLE,
    BADLANDS,
    ERODED_BADLANDS,
    WOODED_BADLANDS,
    MEADOW,
    CHERRY_GROVE,
    GROVE,
    SNOWY_SLOPES,
    FROZEN_PEAKS,
    JAGGED_PEAKS,
    STONY_PEAKS,
    RIVER,
    FROZEN_RIVER,
    BEACH,
    SNOWY_BEACH,
    STONY_SHORE,
    WARM_OCEAN,
    LUKEWARM_OCEAN,
    DEEP_LUKEWARM_OCEAN,
    OCEAN,
    DEEP_OCEAN,
    COLD_OCEAN,
    DEEP_COLD_OCEAN,
    FROZEN_OCEAN,
    DEEP_FROZEN_OCEAN,
    MUSHROOM_FIELDS,
    DRIPSTONE_CAVES,
    LUCH_CAVES,
    DEEP_DARK, //won't be implemented
    // nether
    NETHER_WASTES,
    WARPED_FOREST,
    CRIMSON_FOREST,
    SOUL_SAND_VALLEY,
    BASALT_DELTAS,
    // end
    THE_END,
    END_HIGHLANDS,
    END_MIDLANDS,
    SMALL_END_ISLANDS,
    END_BARRENS
};

// enum Continentalness {
//     C_MUSHROOM,
//     C_DEEP_OCEAN,
//     C_OCEAN,
//     C_COAST,
//     C_NEAR_INLANDS,
//     C_MID_INLANDS,
//     C_FAR_INLANDS
// };

struct Biome {
    public:
        std::string name = "DEFAULT";
        // int T = -1, H = -1, C = -1, E = -1, PV = -1, D = 0;
        // float temp = 0.0f;
        // T for Temperature
        // H for Humidity
        // C for Continentalness
        // E for Erosion
        // PV for Peaks and Valleys = 1 - |3 * weirdness - 2|
        // D for Depth
        // bool isBiome( int t, int h, int c, int e, int pv, int d ) {
        //     return (t == T && h == H && c == C && e == E && pv == PV && d == D);
        // }
};

struct Void : Biome {
    public:
        Void() {
            name = "VOID";
        }
};

struct Plains : Biome { // flat, few oak trees, grass
    public:
        Plains() {
            name = "PLAINS";
            // temp = 0.8f;
        }
};

struct SunflowerPlains : Biome {
    public:
        SunflowerPlains() {
            name = "SUNFLOWER_PLAINS";
        }
};

struct SnowyPlains : Biome {
    public:
        SnowyPlains() {
            name = "SNOWY_PLAINS";
        }
};

struct IceSpikes : Biome {
    public:
        IceSpikes() {
            name = "ICE_SPIKES";
        }
};

struct Desert : Biome {
    public:
        Desert() {
            name = "DESERT";
        }
};

struct Swamp : Biome {
    public:
        Swamp() {
            name = "SWAMP";
        }
};

struct MangroveSwamp : Biome {
    public:
        MangroveSwamp() {
            name = "MANGROVE_SWAMP";
        }
};

struct Forest : Biome {
    public:
        Forest() {
            name = "FOREST";
        }
};

struct FlowerForest : Biome {
    public:
        FlowerForest() {
            name = "FLOWER_FOREST";
        }
};

struct BirchForest : Biome {
    public:
        BirchForest() {
            name = "BIRCH_FOREST";
        }
};

struct DarkForest : Biome {
    public:
        DarkForest() {
            name = "DARK_FOREST";
        }
};

struct OldGrowthBirchForest : Biome {
    public:
        OldGrowthBirchForest() {
            name = "OLD_GROWTH_BIRCH_FOREST";
        }
};

struct OldGrowthPineTaiga : Biome {
    public:
        OldGrowthPineTaiga() {
            name = "OLD_GROWTH_PINE_TAIGA";
        }
};

struct OldGrowthSpruceTaiga : Biome {
    public:
        OldGrowthSpruceTaiga() {
            name = "OLD_GROWTH_SPRUCE_TAIGA";
        }
};

struct Taiga : Biome {
    public:
        Taiga() {
            name = "TAIGA";
        }
};

struct SnowyTaiga : Biome {
    public:
        SnowyTaiga() {
            name = "SNOWY_TAIGA";
        }
};

struct Savanna : Biome {
    public:
        Savanna() {
            name = "SAVANNA";
        }
};

struct SavannaPlateau : Biome {
    public:
        SavannaPlateau() {
            name = "SAVANNA_PLATEAU";
        }
};

struct WindsweptHills : Biome {
    public:
        WindsweptHills() {
            name = "WINDSWEPT_HILLS";
        }
};

struct WindsweptGravellyHills : Biome {
    public:
        WindsweptGravellyHills() {
            name = "WINDSWEPT_GRAVELLY_HILLS";
        }
};

struct WindsweptForest : Biome {
    public:
        WindsweptForest() {
            name = "WINDSWEPT_FOREST";
        }
};

struct WindsweptSavanna : Biome {
    public:
        WindsweptSavanna() {
            name = "WINDSWEPT_SAVANNA";
        }
};

struct Jungle : Biome {
    public:
        Jungle() {
            name = "JUNGLE";
        }
};

struct SparseJungle : Biome {
    public:
        SparseJungle() {
            name = "SPARSE_JUNGLE";
        }
};

struct BambooJungle : Biome {
    public:
        BambooJungle() {
            name = "BAMBOO_JUNGLE";
        }
};

struct Badlands : Biome {
    public:
        Badlands() {
            name = "BADLANDS";
        }
};

struct ErodedBadlands : Biome {
    public:
        ErodedBadlands() {
            name = "ERODED_BADLANDS";
        }
};

struct WoodedBadlands : Biome {
    public:
        WoodedBadlands() {
            name = "WOODED_BADLANDS";
        }
};

struct Meadow : Biome {
    public:
        Meadow() {
            name = "MEADOW";
        }
};

struct CherryGrove : Biome {
    public:
        CherryGrove() {
            name = "CHERRY_GROVE";
        }
};

struct Grove : Biome {
    public:
        Grove() {
            name = "GROVE";
        }
};

struct SnowySlopes : Biome {
    public:
        SnowySlopes() {
            name = "SNOWY_SLOPES";
        }
};

struct FrozenPeaks : Biome {
    public:
        FrozenPeaks() {
            name = "FROZEN_PEAKS";
        }
};

struct JaggedPeaks : Biome {
    public:
        JaggedPeaks() {
            name = "JAGGED_PEAKS";
        }
};

struct StonyPeaks : Biome {
    public:
        StonyPeaks() {
            name = "STONY_PEAKS";
        }
};

struct River : Biome {
    public:
        River() {
            name = "RIVER";
        }
};

struct FrozenRiver : Biome {
    public:
        FrozenRiver() {
            name = "FROZEN_RIVER";
        }
};

struct Beach : Biome {
    public:
        Beach() {
            name = "BEACH";
        }
};

struct SnowyBeach : Biome {
    public:
        SnowyBeach() {
            name = "SNOWY_BEACH";
        }
};

struct StonyShore : Biome {
    public:
        StonyShore() {
            name = "STONY_SHORE";
        }
};

struct WarmOcean : Biome {
    public:
        WarmOcean() {
            name = "WARM_OCEAN";
        }
};

struct LukewarmOcean : Biome {
    public:
        LukewarmOcean() {
            name = "LUKEWARM_OCEAN";
        }
};

struct DeepLukewarmOcean : Biome {
    public:
        DeepLukewarmOcean() {
            name = "DEEP_LUKEWARM_OCEAN";
        }
};

struct Ocean : Biome {
    public:
        Ocean() {
            name = "OCEAN";
        }
};

struct DeepOcean : Biome {
    public:
        DeepOcean() {
            name = "DEEP_OCEAN";
        }
};

struct ColdOcean : Biome {
    public:
        ColdOcean() {
            name = "COLD_OCEAN";
        }
};

struct DeepColdOcean : Biome {
    public:
        DeepColdOcean() {
            name = "DEEP_COLD_OCEAN";
        }
};

struct FrozenOcean : Biome {
    public:
        FrozenOcean() {
            name = "FROZEN_OCEAN";
        }
};

struct DeepFrozenOcean : Biome {
    public:
        DeepFrozenOcean() {
            name = "DEEP_FROZEN_OCEAN";
        }
};

struct MushroomFields : Biome {
    public:
        MushroomFields() {
            name = "MUSHROOM_FIELDS";
        }
};

struct DripstoneCaves : Biome {
    public:
        DripstoneCaves() {
            name = "DRIPSTONE_CAVES";
        }
};

struct LuchCaves : Biome {
    public:
        LuchCaves() {
            name = "LUCH_CAVES";
        }
};

struct DeepDark : Biome {
    public:
        DeepDark() {
            name = "DEEP_DARK";
        }
};

struct NetherWastes : Biome {
    public:
        NetherWastes() {
            name = "NETHER_WASTES";
        }
};

struct WarpedForest : Biome {
    public:
        WarpedForest() {
            name = "WARPED_FOREST";
        }
};

struct CrimsonForest : Biome {
    public:
        CrimsonForest() {
            name = "CRIMSON_FOREST";
        }
};

struct SoulSandValley : Biome {
    public:
        SoulSandValley() {
            name = "SOUL_SAND_VALLEY";
        }
};

struct BasaltDeltas : Biome {
    public:
        BasaltDeltas() {
            name = "BASALT_DELTAS";
        }
};

struct TheEnd : Biome {
    public:
        TheEnd() {
            name = "THE_END";
        }
};

struct EndHighlands : Biome {
    public:
        EndHighlands() {
            name = "END_HIGHLANDS";
        }
};

struct EndMidlands : Biome {
    public:
        EndMidlands() {
            name = "END_MIDLANDS";
        }
};

struct SmallEndIslands : Biome {
    public:
        SmallEndIslands() {
            name = "SMALL_END_ISLANDS";
        }
};

struct EndBarrens : Biome {
    public:
        EndBarrens() {
            name = "END_BARRENS";
        }
};

const Biome s_biomes[64] {
    Void(), Plains(), SunflowerPlains(), SnowyPlains(), IceSpikes(), Desert(), Swamp(), MangroveSwamp(),
    Forest(), FlowerForest(), BirchForest(), DarkForest(), OldGrowthBirchForest(), OldGrowthPineTaiga(), OldGrowthSpruceTaiga(), Taiga(),
    SnowyTaiga(), Savanna(), SavannaPlateau(), WindsweptHills(), WindsweptGravellyHills(), WindsweptForest(), WindsweptSavanna(), Jungle(),
    SparseJungle(), BambooJungle(), Badlands(), ErodedBadlands(), WoodedBadlands(), Meadow(), CherryGrove(), Grove(),
    SnowySlopes(), FrozenPeaks(), JaggedPeaks(), StonyPeaks(), River(), FrozenRiver(), Beach(), SnowyBeach(),
    StonyShore(), WarmOcean(), LukewarmOcean(), DeepLukewarmOcean(), Ocean(), DeepOcean(), ColdOcean(), DeepColdOcean(),
    FrozenOcean(), DeepFrozenOcean(), MushroomFields(), DripstoneCaves(), LuchCaves(), DeepDark(), NetherWastes(), WarpedForest(),
    CrimsonForest(), SoulSandValley(), BasaltDeltas(), TheEnd(), EndHighlands(), EndMidlands(), SmallEndIslands(), EndBarrens()
};

#endif
