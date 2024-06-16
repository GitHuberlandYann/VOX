/**
 * Implementation of A* pathfinding algo, modified to fit my needs
 * 
 * terminoly:
 * 	G cost = distance from starting node
 * 	H cost = distance from end (H stands for heuristic)
 * 	F cost = G cost + H cost
 *
 * base pseudocode:
 * {
 * OPEN // the set of nodes to be evaluated
 * CLOSED // the set of nodes already evaluated
 *
 * add the start node to OPEN
 *
 * loop
 * 	if OPEN is empty
 * 		return failure
 * 
 * 	current = node in OPEN with the lowest f_cost
 * 	remove current from OPEN
 * 	add current to CLOSED
 *
 * 	if current is the target node // path has been found
 * 		return success
 *
 * 	foreach neighbour of the current node
 * 		if neighbour is not traversable or neighbour is in CLOSED
 * 			skip to next neighbour
 *
 * 		if new path to neighbour is shorter OR neighbour is not in OPEN
 * 			set f_cost of neighbour
 * 			set parent of neighbour to current
 * 			if neighbour is not in open
 * 				add neighbour to open
 * 
 * return failure
 * }
 *
 * spec for minecraft:
 *  path must be 2 blocks high
 * 	neighbour must also be air with air above and solid block underneath it
 * 	neighbour is at first 4 cardinals, if solid it becomes block above, if air it becomes block below
 *  else it then checks diag corner, with same two first checks <- means you can only go in diag if air to your righ or left
 * 	limit loop size with incr var
 */

#include "Chunk.hpp"
#include "logs.hpp"

/**
 * @brief generates vector of ivec3 from end node of path, using node->parent to backtrack path
 * path returned represents paht of entities' feet 
 */
static std::vector<glm::ivec3> genPathFromNode( std::shared_ptr<t_pathfinding_node> end )
{
	std::vector<glm::ivec3> res;

	while (end) {
		res.push_back(end->pos);
		end = end->parent;
	}

	std::reverse(res.begin(), res.end());
	return (res);
}

/**
 * @brief compute dist between 2 points in 3D grid.
 * 2 grid cells next to each other have dist of 10
 * 2 grid cells diag next to each other have dist of 14 = round(sqrt(2) * 10)
 */
static int computePathfindDist( glm::ivec3 pos, glm::ivec3 target )
{
	int dx = glm::abs(target.x - pos.x);
	int dy = glm::abs(target.y - pos.y);

	return (glm::min(dx, dy) * 14 + glm::abs(dx - dy) * 10 + glm::abs(target.z - pos.z) * 4);
}

void Chunk::checkNodePathfinding( std::list<std::shared_ptr<t_pathfinding_node>>& open_nodes,
	std::list<std::shared_ptr<t_pathfinding_node>>& visited_nodes, glm::ivec3 pos, bool* trav,
	std::shared_ptr<t_pathfinding_node> current, glm::ivec3 start, glm::ivec3 end )
{
	// check if traversable
	int adj_type = getBlockAtAbsolute(pos) & mask::blocks::type;
	int above_type = getBlockAtAbsolute(pos.x, pos.y, pos.z + 1) & mask::blocks::type;
	if (s_blocks[adj_type]->transparent) {
		if (!s_blocks[above_type]->transparent) { // no space for head, avoid
			return ;
		}
		if (trav) { // enough space to try and travel in diagonal
			*trav = true;
		}
		int below_type = getBlockAtAbsolute(pos.x, pos.y, pos.z - 1) & mask::blocks::type;
		if (s_blocks[below_type]->transparent) { // under feet is air, look below to 'descend'
			below_type = getBlockAtAbsolute(pos.x, pos.y, pos.z - 2) & mask::blocks::type;
			if (s_blocks[below_type]->transparent) { // 2 blocks drop, avoid
				return ;
			}
			--pos.z;
		}
	} else if (s_blocks[above_type]->transparent) { // feet is solid, look above to 'climb'
		above_type = getBlockAtAbsolute(pos.x, pos.y, pos.z + 2) & mask::blocks::type;
		if (!s_blocks[above_type]->transparent) { // no space for head after climb, avoid
			return ;
		}
		++pos.z;
	} else { // no space to 'climb', avoid
		return ;
	}

	// check if cell already visited
	auto search_vis = std::find_if(visited_nodes.begin(), visited_nodes.end(),
		[pos]( std::shared_ptr<t_pathfinding_node> node ) { return (node->pos == pos); });
			
	if (search_vis != visited_nodes.end()) { // node is already in visited nodes
		return ;
	}

	// check if cell already evaluated
	auto search = std::find_if(open_nodes.begin(), open_nodes.end(),
		[pos]( std::shared_ptr<t_pathfinding_node> node ) { return (node->pos == pos); });

	int f_dist = computePathfindDist(pos, start) + computePathfindDist(pos, end);
	if (search != open_nodes.end()) { // node is already in open nodes, we check if needs to be updated
		if (f_dist >= (*search)->f_dist) {
		 	return ;
		}
		(*search)->f_dist = f_dist;
		(*search)->parent = current;
		return ;
	}

	std::shared_ptr<t_pathfinding_node> node = std::make_shared<t_pathfinding_node>(f_dist, pos);
	node->parent = current;
	open_nodes.push_back(node);
}

std::pair<std::vector<glm::ivec3>, int> Chunk::computePathfinding( glm::ivec3 start, glm::ivec3 end )
{
	std::list<std::shared_ptr<t_pathfinding_node>> open_nodes, visited_nodes;
	std::shared_ptr<t_pathfinding_node> current, closest;

	open_nodes.push_back(std::make_shared<t_pathfinding_node>(
		computePathfindDist(start, start) + computePathfindDist(start, end), start));
	closest = NULL;
	
	int count = 0;
	while (true) {
		if (open_nodes.empty() || ++count > 128) {
			// LOG("pathfinding failure with count " << count << " and open size " << open_nodes.size());
			// return {{}, count}; // failure
			return {genPathFromNode(closest), count}; // failure, but return closest path found
		}

		open_nodes.sort([]( auto a, auto b ) { return (a->f_dist < b->f_dist); });

		current = *(open_nodes.begin());
		open_nodes.erase(open_nodes.begin());
		visited_nodes.push_back(current);

		if (current->pos == end) { // path has been found
			return {genPathFromNode(current), count};
		} else if (!closest || current->f_dist <= closest->f_dist) {
			closest = current;
		}

		std::array<bool, 4> adj_trav = {false};

		for (int i = 0; i < 4; ++i) {
			checkNodePathfinding(open_nodes, visited_nodes, current->pos + adj_blocks[i], &adj_trav[i], current, start, end);
		}

		// diag movements
		if (adj_trav[face_dir::minus_x] || adj_trav[face_dir::minus_y]) {
			checkNodePathfinding(open_nodes, visited_nodes, current->pos + glm::ivec3(-1, -1, 0), NULL, current, start, end);
		}
		if (adj_trav[face_dir::minus_x] || adj_trav[face_dir::plus_y]) {
			checkNodePathfinding(open_nodes, visited_nodes, current->pos + glm::ivec3(-1, 1, 0), NULL, current, start, end);
		}
		if (adj_trav[face_dir::plus_x] || adj_trav[face_dir::plus_y]) {
			checkNodePathfinding(open_nodes, visited_nodes, current->pos + glm::ivec3(1, 1, 0), NULL, current, start, end);
		}
		if (adj_trav[face_dir::plus_x] || adj_trav[face_dir::minus_y]) {
			checkNodePathfinding(open_nodes, visited_nodes, current->pos + glm::ivec3(1, -1, 0), NULL, current, start, end);
		}
	}
	return {};
}
