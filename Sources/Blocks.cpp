#include "Blocks.hpp"

const Block *s_blocks[S_BLOCKS_SIZE] = {
	new Air(), new GrassBlock(), new OakLog(), new Cactus(), new Farmland(), new DirtPath(), new TNT(), new TBD(),
	new CraftingTable(), new Furnace(), new OakStairsBottom(), new OakStairsTop(), new OakDoor(), new OakTrapdoor(), new TBD(), new TBD(),
	new Bedrock(), new Dirt(), new SmoothStone(), new Stone(), new Cobblestone(), new StoneBrick(), new CrackedStoneBrick(), new Sand(),
	new Gravel(), new OakLeaves(), new OakPlanks(), new Glass(), new TBD(), new TBD(), new TBD(), new TBD(),
	new CoalOre(), new IronOre(), new DiamondOre(), new CoalBlock(), new IronBlock(), new DiamondBlock(), new TBD(), new TBD(),
	new OakSlabBottom(), new OakSlabTop(), new TBD(), new TBD(), new TBD(), new TBD(), new TBD(), new TBD(),
	new Poppy(), new Dandelion(), new BlueOrchid(), new Allium(), new CornFlower(), new PinkTulip(), new Grass(), new SugarCane(),
	new DeadBush(), new OakSapling(), new Torch(), new TBD(), new TBD(), new TBD(), new TBD(), new Chest(),
	new WheatCrop(), new WheatCrop1(), new WheatCrop2(), new WheatCrop3(), new WheatCrop4(), new WheatCrop5(), new WheatCrop6(), new WheatCrop7(),
	new Water(), new Water1(), new Water2(), new Water3(), new Water4(), new Water5(), new Water6(), new Water7(),
	new Stick(), new WoodenShovel(), new StoneShovel(), new IronShovel(), new DiamondShovel(), new WoodenAxe(), new StoneAxe(), new IronAxe(),
	new DiamondAxe(), new WoodenPickaxe(), new StonePickaxe(), new IronPickaxe(), new DiamondPickaxe(), new Bow(), new Arrow(), new TBD(),
	new Coal(), new Charcoal(), new IronIngot(), new Diamond(), new Bucket(), new WaterBucket(), new WoodenHoe(), new StoneHoe(),
	new IronHoe(), new DiamondHoe(), new WheatSeeds(), new Wheat(), new Bread(), new Apple(), new Flint(), new FlintAndSteel()
};
