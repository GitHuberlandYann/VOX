#ifndef INVENTORY_HPP
# define INVENTORY_HPP

# include "Blocks.hpp"
# include <list>
# include <array>
class Chunk;
class FurnaceInstance;
class ChestInstance;
class UI;

# define IRECEIPT_SIZE 29
# define RECEIPT_SIZE 123

const int ireceipt[IRECEIPT_SIZE][6] = {
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_LOG, blocks::OAK_PLANKS, 4},
	{blocks::AIR, blocks::AIR, blocks::OAK_LOG, blocks::AIR, blocks::OAK_PLANKS, 4},
	{blocks::AIR, blocks::OAK_LOG, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, 4},
	{blocks::OAK_LOG, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::COAL_BLOCK, blocks::COAL, 9},
	{blocks::AIR, blocks::AIR, blocks::COAL_BLOCK, blocks::AIR, blocks::COAL, 9},
	{blocks::AIR, blocks::COAL_BLOCK, blocks::AIR, blocks::AIR, blocks::COAL, 9},
	{blocks::COAL_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::COAL, 9},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::IRON_BLOCK, blocks::IRON_INGOT, 9},
	{blocks::AIR, blocks::AIR, blocks::IRON_BLOCK, blocks::AIR, blocks::IRON_INGOT, 9},
	{blocks::AIR, blocks::IRON_BLOCK, blocks::AIR, blocks::AIR, blocks::IRON_INGOT, 9},
	{blocks::IRON_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::IRON_INGOT, 9},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::DIAMOND_BLOCK, blocks::DIAMOND, 9},
	{blocks::AIR, blocks::AIR, blocks::DIAMOND_BLOCK, blocks::AIR, blocks::DIAMOND, 9},
	{blocks::AIR, blocks::DIAMOND_BLOCK, blocks::AIR, blocks::AIR, blocks::DIAMOND, 9},
	{blocks::DIAMOND_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::DIAMOND, 9},
	{blocks::OAK_PLANKS, blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::STICK, 4},
	{blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::OAK_PLANKS, blocks::STICK, 4},
	{blocks::COAL, blocks::AIR, blocks::STICK, blocks::AIR, blocks::TORCH, 4},
	{blocks::AIR, blocks::COAL, blocks::AIR, blocks::STICK, blocks::TORCH, 4},
	{blocks::CHARCOAL, blocks::AIR, blocks::STICK, blocks::AIR, blocks::TORCH, 4},
	{blocks::AIR, blocks::CHARCOAL, blocks::AIR, blocks::STICK, blocks::TORCH, 4},
	{blocks::REDSTONE_DUST, blocks::AIR, blocks::STICK, blocks::AIR, blocks::REDSTONE_TORCH, 4},
	{blocks::AIR, blocks::REDSTONE_DUST, blocks::AIR, blocks::STICK, blocks::REDSTONE_TORCH, 4},
	{blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::CRAFTING_TABLE, 1},
	{blocks::STONE, blocks::STONE, blocks::STONE, blocks::STONE, blocks::STONE_BRICKS, 4},
	{blocks::IRON_INGOT, blocks::AIR, blocks::AIR, blocks::FLINT, blocks::FLINT_AND_STEEL, 1},
	{blocks::STICK, blocks::AIR, blocks::COBBLESTONE, blocks::AIR, blocks::LEVER, 1},
	{blocks::AIR, blocks::STICK, blocks::AIR, blocks::COBBLESTONE, blocks::LEVER, 1},
};

const int receipt[RECEIPT_SIZE][11] = {
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_LOG, blocks::OAK_PLANKS, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_LOG, blocks::AIR, blocks::OAK_PLANKS, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_LOG, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_LOG, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_LOG, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_LOG, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, 4},
	{blocks::AIR, blocks::AIR, blocks::OAK_LOG, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, 4},
	{blocks::AIR, blocks::OAK_LOG, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, 4},
	{blocks::OAK_LOG, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, 4},

	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::STICK, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::STICK, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::STICK, 4},
	{blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::AIR, blocks::STICK, 4},
	{blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::STICK, 4},
	{blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::STICK, 4},

	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::COAL, blocks::AIR, blocks::AIR, blocks::STICK, blocks::TORCH, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::COAL, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::TORCH, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::COAL, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::TORCH, 4},
	{blocks::AIR, blocks::AIR, blocks::COAL, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::TORCH, 4},
	{blocks::AIR, blocks::COAL, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::TORCH, 4},
	{blocks::COAL, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::TORCH, 4},

	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::REDSTONE_DUST, blocks::AIR, blocks::AIR, blocks::STICK, blocks::REDSTONE_TORCH, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::REDSTONE_DUST, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::REDSTONE_TORCH, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::REDSTONE_DUST, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::REDSTONE_TORCH, 4},
	{blocks::AIR, blocks::AIR, blocks::REDSTONE_DUST, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::REDSTONE_TORCH, 4},
	{blocks::AIR, blocks::REDSTONE_DUST, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::REDSTONE_TORCH, 4},
	{blocks::REDSTONE_DUST, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::REDSTONE_TORCH, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::REDSTONE_TORCH, blocks::REDSTONE_DUST, blocks::REDSTONE_TORCH, blocks::STONE, blocks::STONE, blocks::STONE, blocks::REPEATER, 1},

	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::CHARCOAL, blocks::AIR, blocks::AIR, blocks::STICK, blocks::TORCH, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::CHARCOAL, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::TORCH, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::CHARCOAL, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::TORCH, 4},
	{blocks::AIR, blocks::AIR, blocks::CHARCOAL, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::TORCH, 4},
	{blocks::AIR, blocks::CHARCOAL, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::TORCH, 4},
	{blocks::CHARCOAL, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::TORCH, 4},

	{blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::CRAFTING_TABLE, 1},
	{blocks::AIR, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::AIR, blocks::CRAFTING_TABLE, 1},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::CRAFTING_TABLE, 1},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::CRAFTING_TABLE, 1},
	
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::OAK_SLAB_BOTTOM, 6},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::STONE, blocks::STONE, blocks::STONE, blocks::STONE_SLAB_BOTTOM, 6},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::SMOOTH_STONE, blocks::SMOOTH_STONE, blocks::SMOOTH_STONE, blocks::SMOOTH_STONE_SLAB_BOTTOM, 6},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::COBBLESTONE, blocks::COBBLESTONE, blocks::COBBLESTONE, blocks::COBBLESTONE_SLAB_BOTTOM, 6},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::STONE_BRICKS, blocks::STONE_BRICKS, blocks::STONE_BRICKS, blocks::STONE_BRICKS_SLAB_BOTTOM, 6},

	{blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::OAK_STAIRS_BOTTOM, 4},
	{blocks::STONE, blocks::AIR, blocks::AIR, blocks::STONE, blocks::STONE, blocks::AIR, blocks::STONE, blocks::STONE, blocks::STONE, blocks::STONE_STAIRS_BOTTOM, 4},
	{blocks::SMOOTH_STONE, blocks::AIR, blocks::AIR, blocks::SMOOTH_STONE, blocks::SMOOTH_STONE, blocks::AIR, blocks::SMOOTH_STONE, blocks::SMOOTH_STONE, blocks::SMOOTH_STONE, blocks::SMOOTH_STONE_STAIRS_BOTTOM, 4},
	{blocks::COBBLESTONE, blocks::AIR, blocks::AIR, blocks::COBBLESTONE, blocks::COBBLESTONE, blocks::AIR, blocks::COBBLESTONE, blocks::COBBLESTONE, blocks::COBBLESTONE, blocks::COBBLESTONE_STAIRS_BOTTOM, 4},
	{blocks::STONE_BRICKS, blocks::AIR, blocks::AIR, blocks::STONE_BRICKS, blocks::STONE_BRICKS, blocks::AIR, blocks::STONE_BRICKS, blocks::STONE_BRICKS, blocks::STONE_BRICKS, blocks::STONE_BRICKS_STAIRS_BOTTOM, 4},

	{blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::OAK_DOOR, 3},
	{blocks::AIR, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::OAK_DOOR, 3},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::OAK_TRAPDOOR, 2},
	{blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_TRAPDOOR, 2},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_PLANKS, blocks::STICK, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::STICK, blocks::OAK_PLANKS, blocks::OAK_FENCE, 3},
	{blocks::OAK_PLANKS, blocks::STICK, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::STICK, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::AIR, blocks::OAK_FENCE, 3},

	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::GLASS, blocks::GLASS, blocks::GLASS, blocks::GLASS, blocks::GLASS, blocks::GLASS, blocks::GLASS_PANE, 16},
	{blocks::GLASS, blocks::GLASS, blocks::GLASS, blocks::GLASS, blocks::GLASS, blocks::GLASS, blocks::AIR, blocks::AIR, blocks::AIR, blocks::GLASS_PANE, 16},

	{blocks::WHEAT, blocks::WHEAT, blocks::WHEAT, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::BREAD, 1},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::WHEAT, blocks::WHEAT, blocks::WHEAT, blocks::AIR, blocks::AIR, blocks::AIR, blocks::BREAD, 1},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::WHEAT, blocks::WHEAT, blocks::WHEAT, blocks::BREAD, 1},

	{blocks::STONE, blocks::STONE, blocks::AIR, blocks::STONE, blocks::STONE, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::STONE_BRICKS, 4},
	{blocks::AIR, blocks::STONE, blocks::STONE, blocks::AIR, blocks::STONE, blocks::STONE, blocks::AIR, blocks::AIR, blocks::AIR, blocks::STONE_BRICKS, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::STONE, blocks::STONE, blocks::AIR, blocks::STONE, blocks::STONE, blocks::AIR, blocks::STONE_BRICKS, 4},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::STONE, blocks::STONE, blocks::AIR, blocks::STONE, blocks::STONE, blocks::STONE_BRICKS, 4},
	
	{blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::CHEST, 1},
	{blocks::COBBLESTONE, blocks::COBBLESTONE, blocks::COBBLESTONE, blocks::COBBLESTONE, blocks::AIR, blocks::COBBLESTONE, blocks::COBBLESTONE, blocks::COBBLESTONE, blocks::COBBLESTONE, blocks::FURNACE, 1},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::IRON_INGOT, blocks::AIR, blocks::IRON_INGOT, blocks::AIR, blocks::IRON_INGOT, blocks::AIR, blocks::BUCKET, 1},

	{blocks::AIR, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::WOODEN_SHOVEL, 1},
	{blocks::AIR, blocks::COBBLESTONE, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::STONE_SHOVEL, 1},
	{blocks::AIR, blocks::IRON_INGOT, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::IRON_SHOVEL, 1},
	{blocks::AIR, blocks::DIAMOND, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::DIAMOND_SHOVEL, 1},
	{blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::OAK_PLANKS, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::WOODEN_AXE, 1},
	{blocks::COBBLESTONE, blocks::COBBLESTONE, blocks::AIR, blocks::COBBLESTONE, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::STONE_AXE, 1},
	{blocks::IRON_INGOT, blocks::IRON_INGOT, blocks::AIR, blocks::IRON_INGOT, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::IRON_AXE, 1},
	{blocks::DIAMOND, blocks::DIAMOND, blocks::AIR, blocks::DIAMOND, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::DIAMOND_AXE, 1},
	{blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::WOODEN_PICKAXE, 1},
	{blocks::COBBLESTONE, blocks::COBBLESTONE, blocks::COBBLESTONE, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::STONE_PICKAXE, 1},
	{blocks::IRON_INGOT, blocks::IRON_INGOT, blocks::IRON_INGOT, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::IRON_PICKAXE, 1},
	{blocks::DIAMOND, blocks::DIAMOND, blocks::DIAMOND, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::DIAMOND_PICKAXE, 1},
	{blocks::OAK_PLANKS, blocks::OAK_PLANKS, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::WOODEN_HOE, 1},
	{blocks::COBBLESTONE, blocks::COBBLESTONE, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::STONE_HOE, 1},
	{blocks::IRON_INGOT, blocks::IRON_INGOT, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::IRON_HOE, 1},
	{blocks::DIAMOND, blocks::DIAMOND, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::AIR, blocks::STICK, blocks::AIR, blocks::DIAMOND_HOE, 1},
	{blocks::AIR, blocks::STICK, blocks::STICK, blocks::STICK, blocks::AIR, blocks::STICK, blocks::AIR, blocks::STICK, blocks::STICK, blocks::BOW, 1},
	
	{blocks::COAL, blocks::COAL, blocks::COAL, blocks::COAL, blocks::COAL, blocks::COAL, blocks::COAL, blocks::COAL, blocks::COAL, blocks::COAL_BLOCK, 1},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::COAL_BLOCK, blocks::COAL, 9},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::COAL_BLOCK, blocks::AIR, blocks::COAL, 9},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::COAL_BLOCK, blocks::AIR, blocks::AIR, blocks::COAL, 9},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::COAL_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::COAL, 9},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::COAL_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::COAL, 9},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::COAL_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::COAL, 9},
	{blocks::AIR, blocks::AIR, blocks::COAL_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::COAL, 9},
	{blocks::AIR, blocks::COAL_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::COAL, 9},
	{blocks::COAL_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::COAL, 9},
	{blocks::IRON_INGOT, blocks::IRON_INGOT, blocks::IRON_INGOT, blocks::IRON_INGOT, blocks::IRON_INGOT, blocks::IRON_INGOT, blocks::IRON_INGOT, blocks::IRON_INGOT, blocks::IRON_INGOT, blocks::IRON_BLOCK, 1},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::IRON_BLOCK, blocks::IRON_INGOT, 9},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::IRON_BLOCK, blocks::AIR, blocks::IRON_INGOT, 9},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::IRON_BLOCK, blocks::AIR, blocks::AIR, blocks::IRON_INGOT, 9},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::IRON_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::IRON_INGOT, 9},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::IRON_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::IRON_INGOT, 9},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::IRON_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::IRON_INGOT, 9},
	{blocks::AIR, blocks::AIR, blocks::IRON_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::IRON_INGOT, 9},
	{blocks::AIR, blocks::IRON_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::IRON_INGOT, 9},
	{blocks::IRON_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::IRON_INGOT, 9},
	{blocks::DIAMOND, blocks::DIAMOND, blocks::DIAMOND, blocks::DIAMOND, blocks::DIAMOND, blocks::DIAMOND, blocks::DIAMOND, blocks::DIAMOND, blocks::DIAMOND, blocks::DIAMOND_BLOCK, 1},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::DIAMOND_BLOCK, blocks::DIAMOND, 9},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::DIAMOND_BLOCK, blocks::AIR, blocks::DIAMOND, 9},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::DIAMOND_BLOCK, blocks::AIR, blocks::AIR, blocks::DIAMOND, 9},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::DIAMOND_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::DIAMOND, 9},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::DIAMOND_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::DIAMOND, 9},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::DIAMOND_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::DIAMOND, 9},
	{blocks::AIR, blocks::AIR, blocks::DIAMOND_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::DIAMOND, 9},
	{blocks::AIR, blocks::DIAMOND_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::DIAMOND, 9},
	{blocks::DIAMOND_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::DIAMOND, 9},
	{blocks::REDSTONE_DUST, blocks::REDSTONE_DUST, blocks::REDSTONE_DUST, blocks::REDSTONE_DUST, blocks::REDSTONE_DUST, blocks::REDSTONE_DUST, blocks::REDSTONE_DUST, blocks::REDSTONE_DUST, blocks::REDSTONE_DUST, blocks::REDSTONE_BLOCK, 1},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::REDSTONE_BLOCK, blocks::REDSTONE_DUST, 9},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::REDSTONE_BLOCK, blocks::AIR, blocks::REDSTONE_DUST, 9},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::REDSTONE_BLOCK, blocks::AIR, blocks::AIR, blocks::REDSTONE_DUST, 9},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::REDSTONE_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::REDSTONE_DUST, 9},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::REDSTONE_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::REDSTONE_DUST, 9},
	{blocks::AIR, blocks::AIR, blocks::AIR, blocks::REDSTONE_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::REDSTONE_DUST, 9},
	{blocks::AIR, blocks::AIR, blocks::REDSTONE_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::REDSTONE_DUST, 9},
	{blocks::AIR, blocks::REDSTONE_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::REDSTONE_DUST, 9},
	{blocks::REDSTONE_BLOCK, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::AIR, blocks::REDSTONE_DUST, 9},
};

typedef struct s_item {
	int type = blocks::AIR;
	int amount = 0;
	glm::ivec2 dura = {0, 0};
	// enchantements will come here
}			t_item;

class Inventory
{
    private:
        std::array<t_item, 9> _content;
		std::array<t_item, 27> _backpack;
		std::array<t_item, 4> _icraft;
		std::array<t_item, 9> _craft;
		t_item _crafted;
        int _slot;
		bool _modif;
		UI *_ui;

		t_item *getBlockPtr( int value, int & craft_place, FurnaceInstance *furnace, ChestInstance *chest );
		void changeCrafted( int craft );
		void produceCraft( int craft );
		t_item pickCrafted( int craft, t_item block );
		int findEmptyCell( t_item block, bool swap = false );
		int findBlockCell( int type );
		void pickAllCrafted( int craft );

    public:
        Inventory( void );
        ~Inventory( void );

        int getCurrentSlot( void );
		t_item getSlotBlock( int slot );
		t_item getBackpackBlock( int slot );
		t_item getiCraftBlock( int slot );
		t_item getCraftBlock( int slot );
		t_item getCrafted( void );
		int getSlotNum( void );
        void setSlot( int value );
		t_item pickBlockAt( int craft, int value, FurnaceInstance *furnace, ChestInstance *chest );
		t_item pickHalfBlockAt( int craft, int value, FurnaceInstance *furnace, ChestInstance *chest );
		t_item putBlockAt( int craft, int value, t_item block, FurnaceInstance *furnace, ChestInstance *chest );
		t_item putOneBlockAt( int craft, int value, t_item block, FurnaceInstance *furnace, ChestInstance *chest );
		void restoreBlock( t_item block, bool swap = false );
		bool absorbItem( t_item block );
		void restoreiCraft( void );
		void restoreCraft( void );
		bool getModif( void );
		void setModif( bool value );
        void addBlock( int type );
		// void removeBlockAt( int value, FurnaceInstance *furnace, ChestInstance *chest );
        t_item removeBlock( bool thrown );
		void replaceSlot( int type, bool creative );
		void swapCells( int slot, int location );
		void decrementDurabitilty( void );

		void spillInventory( Chunk *chunk );

        std::string getInventoryString( void );
		std::string getSlotString( void );
		std::string saveString( void );
		void setUIPtr( UI *ui );
		void loadWorld( std::ofstream & ofs, std::ifstream & indata );
};

#endif
